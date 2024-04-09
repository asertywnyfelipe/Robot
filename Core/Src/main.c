/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "lcd_i2c.h"
#include <stdbool.h>
#define lcd_Columns 20
#define lcd_Rows 2
#define menuStep 10

/* BUTTONS 4x3
 *
 */

bool isMenuLock = false;
int menuSelected = -1;
bool isLcdRefresh = false;
int demoSpeed = 0;
struct lcd_disp disp;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

typedef struct {
	const char *name;
	int subMenu;
} MenuItem;

typedef struct {
	int index; //obecny index
	int indexResult; //index do wchodzenia do odpowiednich menu bez submenu
	int lcdDisplayStart; //liczba od ktorej mamu wyswietlac
	int lcdIndexStart; //maksymalna liczba menu albo submenu
	char lcdValue[lcd_Rows][lcd_Columns + 1];
} Menu;

const MenuItem menuItem[] = { { "Main menu", -1 },     			 // 0
		{ "Autonomous Mode", -1 },         // 1
		{ "User Control", -1 },         // 2
		{ "Speed Settings", -1 },         // 3
		{ "View Parameters", -1 },       		  // 4
		{ "Menu E", -1 },        		 // 5
		{ "S", -1 },     			 // 6
		{ "", -2 },               // 7
		{ "", -2 },               // 8
		{ "", -2 },               // 9
		{ "", -2 },               // 10

		{ "", -2 },               // 11
		{ "", -2 },               // 12
		{ "", -2 },               // 9

		};

typedef void (*ButtonCallback)(uint8_t);

typedef struct {
	GPIO_TypeDef *gpio_port;
	uint16_t gpio_pin;
	ButtonCallback callback;
} Button;

Button buttons[] = { { GPIOD, BUTTON_Confirm_Pin, NULL }, { GPIOD,
		BUTTON_Up_Pin, NULL }, { GPIOD, BUTTON_Down_Pin, NULL } };

Menu menu = { .index = 1, .indexResult = -1, .lcdDisplayStart = 0,
		.lcdIndexStart = 0 };

// Define a struct representing a motor
typedef struct {
	TIM_HandleTypeDef *htim; // Pointer to TIM_HandleTypeDef for the motor PWM

	uint32_t channel;        // TIM channel number for the motor PWM
	int speed;               // Motor speed (0-100)
	bool dir;           // Motor direction (1: forward, 0: reverse)
} Motor;

typedef struct {
	Motor MotorLeft;
	Motor MotorRight;
} Robot;

void init_motor(Motor *motor, TIM_HandleTypeDef *htim, uint32_t channel,
		int speed, bool direction) {
	HAL_TIM_PWM_Start(motor->htim, motor->channel);
	motor->htim = htim;
	motor->channel = channel;
	motor->speed = 0;
	motor->dir = 1;
}

void motor_set_speed(Motor *motor, int speed) {
	if (speed > 100) {
		speed = 100;
	} else if (speed < 0) {
		speed = 0;
	}
	motor->speed = speed;
}

void motor_set_direction(Motor *motor, GPIO_TypeDef *gpio_port,
		uint16_t gpio_pinFW, uint16_t gpio_pinBW, bool direction) {
	if (direction != 0 && direction != 1) {
		printf("direction not correct\n");
	} else {
		motor->dir = direction;
	}

	HAL_GPIO_WritePin(gpio_port, gpio_pinFW,
			direction ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(gpio_port, gpio_pinBW,
			!direction ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
char read_keyboard() {

	HAL_GPIO_WritePin(GPIOD, R1_Keyboard_Pin, GPIO_PIN_RESET); //Pull the R1 low
	HAL_GPIO_WritePin(GPIOD, R2_Keyboard_Pin, GPIO_PIN_SET); // Pull the R2 High
	HAL_GPIO_WritePin(GPIOD, R3_Keyboard_Pin, GPIO_PIN_SET); // Pull the R3 High
	HAL_GPIO_WritePin(GPIOD, R4_Keyboard_Pin, GPIO_PIN_SET); // Pull the R4 High

	if (!(HAL_GPIO_ReadPin(GPIOB, C1_Keyboard_Pin)))   // if the Col 1 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C1_Keyboard_Pin))); // wait till the button is pressed
		return '1';
	}

	if (!(HAL_GPIO_ReadPin(GPIOB, C2_Keyboard_Pin)))   // if the Col 2 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C2_Keyboard_Pin)));
			   // wait till the button is pressed
		return '2';
	}

	if (!(HAL_GPIO_ReadPin(GPIOB, C3_Keyboard_Pin)))   // if the Col 3 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C3_Keyboard_Pin)));
			   // wait till the button is pressed
		return '3';
	}
	HAL_GPIO_WritePin(GPIOD, R1_Keyboard_Pin, GPIO_PIN_SET);  //Pull the R1 low
	HAL_GPIO_WritePin(GPIOD, R2_Keyboard_Pin, GPIO_PIN_RESET); // Pull the R2 High
	HAL_GPIO_WritePin(GPIOD, R3_Keyboard_Pin, GPIO_PIN_SET); // Pull the R3 High
	HAL_GPIO_WritePin(GPIOD, R4_Keyboard_Pin, GPIO_PIN_SET); // Pull the R4 High

	if (!(HAL_GPIO_ReadPin(GPIOB, C1_Keyboard_Pin)))   // if the Col 1 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C1_Keyboard_Pin)))
			;   // wait till the button is pressed
		return '4';
	}

	if (!(HAL_GPIO_ReadPin(GPIOB, C2_Keyboard_Pin)))   // if the Col 2 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C2_Keyboard_Pin)))
			;   // wait till the button is pressed
		return '5';
	}

	if (!(HAL_GPIO_ReadPin(GPIOB, C3_Keyboard_Pin)))   // if the Col 3 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C3_Keyboard_Pin)))
			;   // wait till the button is pressed
		return '6';
	}

	HAL_GPIO_WritePin(GPIOD, R1_Keyboard_Pin, GPIO_PIN_SET);  //Pull the R1 low
	HAL_GPIO_WritePin(GPIOD, R2_Keyboard_Pin, GPIO_PIN_SET); // Pull the R2 High
	HAL_GPIO_WritePin(GPIOD, R3_Keyboard_Pin, GPIO_PIN_RESET); // Pull the R3 High
	HAL_GPIO_WritePin(GPIOD, R4_Keyboard_Pin, GPIO_PIN_SET); // Pull the R4 High

	if (!(HAL_GPIO_ReadPin(GPIOB, C1_Keyboard_Pin)))   // if the Col 1 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C1_Keyboard_Pin)))
			;   // wait till the button is pressed
		return '7';
	}

	if (!(HAL_GPIO_ReadPin(GPIOB, C2_Keyboard_Pin)))   // if the Col 2 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C2_Keyboard_Pin)))
			;   // wait till the button is pressed
		return '8';
	}

	if (!(HAL_GPIO_ReadPin(GPIOB, C3_Keyboard_Pin)))   // if the Col 3 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C3_Keyboard_Pin)))
			;   // wait till the button is pressed
		return '9';
	}
	HAL_GPIO_WritePin(GPIOD, R1_Keyboard_Pin, GPIO_PIN_SET);  //Pull the R1 low
	HAL_GPIO_WritePin(GPIOD, R2_Keyboard_Pin, GPIO_PIN_SET); // Pull the R2 High
	HAL_GPIO_WritePin(GPIOD, R3_Keyboard_Pin, GPIO_PIN_SET); // Pull the R3 High
	HAL_GPIO_WritePin(GPIOD, R4_Keyboard_Pin, GPIO_PIN_RESET); // Pull the R4 High

	if (!(HAL_GPIO_ReadPin(GPIOB, C1_Keyboard_Pin)))   // if the Col 1 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C1_Keyboard_Pin)))
			;   // wait till the button is pressed
		return '*';
	}

	if (!(HAL_GPIO_ReadPin(GPIOB, C2_Keyboard_Pin)))   // if the Col 2 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C2_Keyboard_Pin)))
			;   // wait till the button is pressed
		return '0';
	}

	if (!(HAL_GPIO_ReadPin(GPIOB, C3_Keyboard_Pin)))   // if the Col 3 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C3_Keyboard_Pin)))
			;   // wait till the button is pressed
		return '#';
	}

	return 'X';
}
void Display() {
	if (menu.lcdDisplayStart % menuStep == 0) //beginning of menu
			{
		menu.lcdDisplayStart = menu.index;
	}
	if (menuItem[(menu.index) + 1].subMenu == -2) //end of menu
			{
		menu.lcdDisplayStart = menu.index - 2;
	} else //middle of menu
	{
		menu.lcdDisplayStart = menu.index - 1;
	}
	menu.lcdDisplayStart = menu.index;
//       if (index == 0)
//           index = 1;

	/* Generowanie lini */



//		if (menu.index == menu.lcdDisplayStart)
//			strcat(_dataStr, ">"); // Adding ">" if the index matches
//		if (menuItem[menu.lcdDisplayStart + x].subMenu >= 0
//				&& menu.lcdIndexStart != menu.lcdDisplayStart + x) {
//			strcat(_dataStr, "+"); // Adding "+" if condition met
//		} else {
//			strcat(_dataStr, " "); // Adding space otherwise
//		}



		sprintf((char *)disp.f_line, ">%s", menuItem[menu.lcdDisplayStart].name);
		sprintf((char *)disp.s_line, menuItem[menu.lcdDisplayStart+1].name);
		lcd_display(&disp);
		HAL_Delay(500);



	}


void GetButtonInput() {

	char temp =read_keyboard();
	if (temp == '8' && menuItem[(menu.index) + 1].subMenu > -2) {
		menu.index++;
		if (menu.index > menu.lcdIndexStart + menuStep) {
			menu.index = menu.lcdIndexStart + menuStep;
		}
		Display();
	}
	if (temp == '2') {
		menu.index--;
		if (menu.index < menu.lcdIndexStart) {
			menu.index = menu.lcdIndexStart;
		}
		Display();
	}
	if (temp == '#') {
		MenuOk();

	}

	if (temp=='X')
	{
		//Display();

	}



}

int Tick() {
	GetButtonInput();
	int _indexResult = menu.indexResult;
	menu.indexResult = -1;
	return _indexResult;
}

void MenuOk() {
	if (menuItem[menu.index].subMenu > 0) //jezeli istnieje submenu
			{
		if (menuItem[menu.index].subMenu > menu.index) //jezeli submenu jest wieksze niz obecne menu (idziemy w gore)
				{
			menu.lcdIndexStart = menuItem[menu.index].subMenu;
			menu.index = menu.lcdIndexStart + 1;  //jestesmy na 1 nizej niz menu
			menu.lcdDisplayStart = menu.lcdIndexStart;
		} else //jezeli submenu jest mniejsze
		{

			menu.lcdIndexStart = menuItem[menu.index].subMenu / menuStep;
			menu.lcdIndexStart *= menuStep;
			menu.index = menuItem[menu.index].subMenu;
			menu.lcdDisplayStart = menu.index - 1;
			if (menu.lcdDisplayStart < 0)
				menu.lcdDisplayStart = 0;
		}
		Display();
	} else //nie ma submenu
	{
		menu.indexResult = menu.index;
	}
}



//void PrintOnLCD()
//{
//
//	 if (isMenuLock == true)
//	  {
//	    isLcdRefresh = true;
//	    return;
//	  }
//	  for (int x = 0; x < lcd_Rows; x++)
//	  {
//
//	    if (mainLcdValue[x] != menu.lcdValue[x] || isLcdRefresh == true)
//	    {
//	      mainLcdValue[x] = menu.lcdValue[x];
//	     // lcd.setCursor(0, x);
//	     // lcd.print(mainLcdValue[x]);
//	    }
//	  }
//	  isLcdRefresh = false;
//}

void MenuDecode()
{
  switch (menuSelected)
  {
  case 0:
  {
    break;
  }
  case 3:
  {
    if (!isMenuLock)
    {
      isMenuLock = true;
    }
    else
    {
    	int temp_speed=0;
    	sprintf((char *)disp.f_line, "SPEED:%d", temp_speed);
    	sprintf((char *)disp.s_line, "");
    	lcd_display(&disp);
    	while(read_keyboard() != '#')
    	{
    		if (read_keyboard() == '2')
    		{
    		temp_speed++;
    		sprintf((char *)disp.f_line, "SPEED:%d", temp_speed);
    		lcd_display(&disp);
    		}





    	}
    	isMenuLock = false;
    	menuSelected = -1;
    	       // motor_set_speed();
    	Display();
    	break;
    }
  }
  default:
  {

    break;
  }
  }
}

void button_up_callback(int current_mode, int current_selection) {
	if (current_mode == 0) {
		if (current_selection > 0)
			current_selection++;
	}
}

void button_down_callback(int current_mode, int current_selection) {
	if (current_mode == 0) {
		if (current_selection > 0)
			current_selection--;
	}
}

void motor_info(Motor *motor) {
	printf("Motor Speed: %d\n", motor->speed);
	printf("Motor Direction %d\n", motor->dir);
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM3_Init();
	MX_I2C1_Init();
	/* USER CODE BEGIN 2 */

	Robot *andrzej;

// init_motor(&andrzej->MotorLeft, &htim3, TIM_CHANNEL_3, 0, 1);
// init_motor(&andrzej->MotorRight, &htim3, TIM_CHANNEL_4, 0, 1);

	int current_mode = 0;
	int mode_change = 0;



	disp.addr = (0x27 << 1);
	disp.bl = true;
	lcd_init(&disp);

	Menu menu;

	menu.index = 0;
	menu.indexResult = -1;
	menu.lcdDisplayStart = 0;
	menu.lcdIndexStart = 0;
	char temp = 0;
	Display();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
//








		  if (isMenuLock == false)
		  {
		    menuSelected = Tick();
		  }
		  MenuDecode();





//
//	  switch (current_mode) {
//	        case 0: //mode 0, print menu options;
//
//
//	            printf("Hello, choose mode you wanna operate?\n");
//
//
//
//	            printf("current mode to choose is %d\n", mode_change);//print on the screen
//
//
//
//
//				;
//	            break;
//	        case 1://mode 1, autonomous driving
//
//	            break;
//	        case 2://mode 2, driving from controller
//	            printf("You selected Option 2.\n");
//	            break;
//	        case 3://mode 3 fight mode
//	            printf("You selected Option 3.\n");
//	            break;
//	        default:
//	            printf("Invalid choice.\n");
//	            break;
//}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
