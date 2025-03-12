#include "main.h"
#include "lcd_i2c.h"
#include "usart.h"
#include <stdbool.h>
#include <stdio.h>
#include "tim.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN 0 */
/* Tutaj możesz umieścić swoje zmienne, które nie będą nadpisywane. */

/* Struktura LCD */
struct lcd_disp disp = {
    .addr = (0x27 << 1),
    .bl = true
};

/* Struktura menu */
typedef struct MenuItem {
    const char* name;
    struct MenuItem* subMenu;
    void (*action)(void);
} MenuItem;


/* Prototypy funkcji */
void actionSpeed(void);
void actionAcceleration(void);
void actionDeceleration(void);
void actionViewParameters(void);
void DisplayMenu(bool forceUpdate);
void HandleMenuInput(char input);

/* Definicja menu */

// Podmenu Motion Settings
MenuItem motionSettingsMenu[] = {
    { "Speed", NULL, actionSpeed },
    { "Acceleration", NULL, actionAcceleration },
    { "Deceleration", NULL, actionDeceleration },
    { NULL, NULL, NULL } // Koniec menu
};

// Menu główne
MenuItem mainMenu[] = {
    { "Autonomous Mode", NULL, NULL },
    { "User Control", NULL, NULL },
    { "Motion Settings", motionSettingsMenu, NULL },
    { "View Parameters", NULL, actionViewParameters },
    { NULL, NULL, NULL } // Koniec menu
};

/* Zmienne do przechowywania wartości */
int speed = 0;
int acceleration = 0;
int deceleration = 0;
MenuItem* previousMenu = NULL;

MenuItem* currentMenu = mainMenu;
int currentIndex = 0;
int lastIndex = -1; // Zapamiętana ostatnia pozycja

/* USER CODE END 0 */

/* USER CODE BEGIN 1 */
/* Tutaj możesz dodać swoje funkcje, które nie będą nadpisywane. */

/* Funkcja wyświetlania menu tylko przy zmianie */
void DisplayMenu(bool forceUpdate)
{
    if (forceUpdate || currentIndex != lastIndex) { // Aktualizuj tylko jeśli się zmieniło
        sprintf((char*)disp.f_line, ">%s", currentMenu[currentIndex].name);
        if (currentMenu[currentIndex + 1].name) {
            sprintf((char*)disp.s_line, "%s", currentMenu[currentIndex + 1].name);
        } else {
            sprintf((char*)disp.s_line, " ");
        }
        lcd_display(&disp);
        lastIndex = currentIndex; // Zapisz nową pozycję
    }
}

/* Obsługa klawiszy */
void HandleMenuInput(char input) {
    bool updated = false;

    if (input == '8' && currentMenu[currentIndex + 1].name) { // W dół
        currentIndex++;
        updated = true;
    }
    if (input == '2' && currentIndex > 0) { // W górę
        currentIndex--;
        updated = true;
    }
    if (input == '#') { // Wybór opcji
        if (currentMenu[currentIndex].subMenu) {
            previousMenu = currentMenu;
            currentMenu = currentMenu[currentIndex].subMenu;
            currentIndex = 0;
            updated = true;
        } else if (currentMenu[currentIndex].action) {
            currentMenu[currentIndex].action();
            updated = true;
            DisplayMenu(false);
            return;
        }
    }
    if (input == '*') { // Powrót
        if (previousMenu) {
            currentMenu = previousMenu;
            previousMenu = NULL;
        } else {
            currentMenu = mainMenu; // Resetujemy menu do głównego
        }
        currentIndex = 0;       // Resetujemy pozycję kursora
        updated = true;
        DisplayMenu(true); // Odświeżamy menu natychmiast po powrocie
    }
    if (updated) {
        DisplayMenu(false);
    }
}


void editValue(const char* label, int* value) {
    int prevValue = -1;
    DisplayMenu(true);

    sprintf((char*)disp.f_line, "%s: %d", label, *value);
    sprintf((char*)disp.s_line, "Press # or * to exit");
    lcd_display(&disp);

    while (1) {
        char key = read_keyboard();
        if (key == '#' || key == '*') break; // Wyjście z edycji

        if (key == '2') (*value)++;  // Zwiększ wartość
        if (key == '8' && *value > 0) (*value)--;  // Zmniejsz wartość

        if (*value != prevValue) {  // Tylko jeśli wartość się zmieniła
            sprintf((char*)disp.f_line, "%s: %d", label, *value);
            sprintf((char*)disp.s_line, "Press # or * to exit");
            lcd_display(&disp);
            prevValue = *value;  // Aktualizujemy poprzednią wartość
        }

        HAL_Delay(200);  // Krótkie opóźnienie
    }
}

/* Funkcje edycji dla poszczególnych zmiennych */
void actionSpeed() {
    editValue("Speed", &speed);
}

void actionAcceleration() {
    editValue("Acceleration", &acceleration);
}

void actionDeceleration() {
    editValue("Deceleration", &deceleration);
}

/* Funkcja ustawiania prędkości (przykład) */
void actionMotionSettings() {
    int speed = 0;
    int prevSpeed = -1;  // Zmienna do przechowywania poprzedniej prędkości
    DisplayMenu(true);

    sprintf((char*)disp.f_line, "Speed: %d", speed);
    sprintf((char*)disp.s_line, "Press # to exit");
    lcd_display(&disp);

    while (1) {
        char key = read_keyboard();
        if (key == '#') break; // Wyjście z ustawień

        if (key == '2') speed++;  // Zwiększ prędkość
        if (key == '8' && speed > 0) speed--;  // Zmniejsz prędkość

        if (speed != prevSpeed) {  // Tylko jeśli prędkość się zmieniła
            sprintf((char*)disp.f_line, "Speed: %d", speed);
            sprintf((char*)disp.s_line, "Press # to exit");
            lcd_display(&disp);
            prevSpeed = speed;  // Aktualizujemy poprzednią prędkość
        }

        HAL_Delay(200);  // Krótkie opóźnienie
    }
}

/* Funkcja wyświetlania parametrów (przykład) */
void actionViewParameters() {
    // Wyświetlamy wartości prędkości tylko raz
    sprintf((char*)disp.f_line, "Speed Left: %d", 10); // robot.MotorLeft.speed);
    sprintf((char*)disp.s_line, "Speed Right: %d", 15); // robot.MotorRight.speed);
    lcd_display(&disp);

    while (1) {
        char key = read_keyboard();
        if (key == '*') {
            break;  // Opuszczamy pętlę
        }
        HAL_Delay(50);  // Krótsze opóźnienie dla lepszej responsywności
    }

    // Resetujemy indeks, aby wymusić odświeżenie menu
    lastIndex = -1;

    // Powrót do menu głównego
    currentMenu = mainMenu;
    currentIndex = 0;

    // Wyświetlenie menu po powrocie
    DisplayMenu(true);
}

/* Funkcja do ustawiania kąta serwomechanizmu */
void setServoAngle(uint8_t angle) {
    // Zabezpieczenie, aby kąt nie wyszedł poza zakres
    if (angle > 180) angle = 180;
    if (angle < 0) angle = 0;

    // Obliczanie wartości PWM odpowiadającej kątowi
    // Zakładając, że mamy 50 Hz (okres 20 ms)
    uint32_t pulseWidth = (uint32_t)(1000 + (angle * 1000) / 180);  // 1 ms na 0° i 2 ms na 180°

    // Ustawienie wartości PWM
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pulseWidth);  // Zakładając, że używamy TIM3 CH3
}

/* Funkcja do płynnego ruchu serwa */
void smoothServoMove(uint8_t startAngle, uint8_t endAngle, uint16_t delayMs) {
    int step;
    if (startAngle < endAngle) {
        step = 1;  // Zwiększaj kąt
    } else {
        step = -1; // Zmniejszaj kąt
    }

    for (uint8_t angle = startAngle; angle != endAngle; angle += step) {
        setServoAngle(angle);  // Ustawienie kąta
        HAL_Delay(delayMs);    // Czekaj przez chwilę
    }

    // Ustawienie końcowego kąta
    setServoAngle(endAngle);
}

/* USER CODE END 1 */

/* USER CODE BEGIN SysInit 0 */
/* Tutaj możesz dodać kod inicjalizacji systemu (np. zegarów, peryferiów), który nie będzie nadpisywany. */
/* USER CODE END SysInit 0 */

/* USER CODE BEGIN Main 0 */
int main(void) {
    HAL_Init();
  //  SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART1_UART_Init();
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

    lcd_init(&disp);
    DisplayMenu(true); // Pierwsze wyświetlenie menu

    while (1) {
        char input = read_keyboard();
        if (input) {
            HandleMenuInput(input);
        }
        HAL_Delay(10);
    }
}
/* USER CODE END Main 0 */

/* USER CODE BEGIN 2 */
/* Tutaj możesz umieścić kod, który nie jest nadpisywany, np. funkcje pomocnicze itp. */

/* USER CODE END 2 */
