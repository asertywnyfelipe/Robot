#include "main.h"
#include "lcd_i2c.h"
#include "usart.h"
#include <stdbool.h>
#include <stdio.h>

// Struktura menu
typedef struct MenuItem {
    const char* name;
    struct MenuItem* subMenu;
    void (*action)(void);
} MenuItem;

// Prototypy funkcji
void actionSpeedSettings(void);
void actionViewParameters(void);
void DisplayMenu(bool forceUpdate);
void HandleMenuInput(char input);

// Definicja menu

MenuItem mainMenu[] = {
    { "Autonomous Mode", NULL, NULL },
    { "User Control", NULL, NULL },
    { "Speed Settings", NULL, actionSpeedSettings },
    { "View Parameters", NULL, actionViewParameters },
    { NULL, NULL, NULL } // Koniec menu
};

MenuItem speedSettingsMenu[] = {
    { "Set Min Speed", NULL, actionSetMinSpeed },
    { "Set Max Speed", NULL, actionSetMaxSpeed },
    { NULL, NULL, NULL }  // Koniec podmenu
};

// Aktualne menu
MenuItem* currentMenu = mainMenu;
int currentIndex = 0;
int lastIndex = -1; // Zapamiętana ostatnia pozycja

// Struktura LCD
struct lcd_disp disp = {
    .addr = (0x27 << 1),
    .bl = true
};

// Funkcja wyświetlania menu tylko przy zmianie
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

// Obsługa klawiszy
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
    if (input == '*') { // Powrót do menu głównego
        currentMenu = mainMenu; // Resetujemy menu do głównego
        currentIndex = 0;       // Resetujemy pozycję kursora
        updated = true;
    }
    if (updated) {
    	 DisplayMenu(false);
    }
}

// Funkcja ustawiania prędkości (przykład)
void actionSpeedSettings() {
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

// Funkcja wyświetlania parametrów (przykład)
void actionViewParameters() {
    // Wyświetlamy wartości prędkości tylko raz
    sprintf((char*)disp.f_line, "Speed Left: %d", 10);// robot.MotorLeft.speed);
    sprintf((char*)disp.s_line, "Speed Right: %d", 15);//robot.MotorRight.speed);
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

void send_via_bluetooth(char* message) {
    HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);
}

// Funkcja główna
int main(void) {
    HAL_Init();
    //SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART1_UART_Init();


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
