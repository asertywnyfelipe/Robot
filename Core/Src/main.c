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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "lcd_i2c.h"
#include "motor.h"
#include "menu.h"
#include <stdbool.h>
#include <stdio.h>
#define lcd_Columns 20
#define lcd_Rows 2
#define menuStep 10

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

uint8_t txBuffer[20] = "TEST 1\r\n";

typedef struct {
	Motor MotorLeft;
	Motor MotorRight;
} RobotComponents;

const MenuItem menuItem[] = { { "Main menu", -1 },     			 // 0
		{ "Autonomous Mode", -1 },         // 1
		{ "User Control", -1 },         // 2
		{ "Speed Settings", -1 },         // 3
		{ "View Parameters", -1 },       		  // 4
		{ "Menu E", -1 },        		 // 5
		{ "Bluetooth Test", -1 },     			 // 6
		{ "", -2 },               // 7
		{ "", -2 },               // 8
		{ "", -2 },               // 9
		{ "", -2 },               // 10

		{ "", -2 },               // 11
		{ "", -2 },               // 12
		{ "", -2 },               // 9

		};

Menu menu = { .index = 1, .indexResult = -1, .lcdDisplayStart = 0,
		.lcdIndexStart = 0 };

RobotComponents robot;

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

	sprintf((char*) disp.f_line, ">%s", menuItem[menu.lcdDisplayStart].name);
	sprintf((char*) disp.s_line, menuItem[menu.lcdDisplayStart + 1].name);
	lcd_display(&disp);
	HAL_Delay(500);
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

void GetButtonInput() {
	char temp = read_keyboard();
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
}

int Tick() {
	GetButtonInput();
	int _indexResult = menu.indexResult;
	menu.indexResult = -1;
	return _indexResult;
}

void DisplayParameters() {
	while (read_keyboard() != '#') {
		sprintf((char*) disp.f_line, "SPEED LEFT:%d", robot.MotorLeft.speed);
		sprintf((char*) disp.s_line, "SPEED RIGHT:%d", robot.MotorRight.speed);
		lcd_display(&disp);
		HAL_Delay(1000);

		sprintf((char*) disp.f_line, "SPEED DIR:%d", robot.MotorLeft.dir);
		sprintf((char*) disp.s_line, "SPEED DIR:%d", robot.MotorRight.dir);
		lcd_display(&disp);
		HAL_Delay(1000);
	}
}

void MenuDecode() {
	switch (menuSelected) {
	case 0: {
		break;
	}
	case 3: {
		if (!isMenuLock) {
			isMenuLock = true;
			break;
		}

		else {
			int temp_speed = 0;
			sprintf((char*) disp.f_line, "SPEED:%d", temp_speed);
			sprintf((char*) disp.s_line, "  ");
			lcd_display(&disp);
			while (read_keyboard() != '#') {
				if (read_keyboard() == '2') {
					temp_speed++;
					sprintf((char*) disp.f_line, "SPEED:%d", temp_speed);
					lcd_display(&disp);
				}

				if (read_keyboard() == '8' && temp_speed > 0) {
					temp_speed--;
					sprintf((char*) disp.f_line, "SPEED:%d", temp_speed);
					lcd_display(&disp);
				}
			}
			isMenuLock = false;
			menuSelected = -1;
			motor_set_speed(&(robot.MotorLeft), temp_speed);
			motor_set_speed(&(robot.MotorRight), temp_speed);
			Display();
			break;
		}
	}
	case 4: {
		if (!isMenuLock) {
			isMenuLock = true;
			break;
		} else {
			DisplayParameters();
			isMenuLock = false;
			menuSelected = -1;
			Display();
			break;
		}
		}
		case 6:
		{
			if (!isMenuLock) {
				isMenuLock = true;
				break;
			} else {
				while (read_keyboard() != '#') {



					HAL_UART_Transmit(&huart1, txBuffer, sizeof(txBuffer),
							HAL_MAX_DELAY);



					HAL_Delay(100);
				}
				isMenuLock = false;
				menuSelected = -1;
				break;
			}
		}
		default:
		{

			break;
		}
	}
	}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */

		init_motor(&(robot.MotorLeft), &htim3, TIM_CHANNEL_3, 0, 1);
		init_motor(&(robot.MotorRight), &htim3, TIM_CHANNEL_4, 0, 1);

		disp.addr = (0x27 << 1);
		disp.bl = true;
		lcd_init(&disp);

		menu.index = 0;
		menu.indexResult = -1;
		menu.lcdDisplayStart = 0;
		menu.lcdIndexStart = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
		while (1) {
//



//			HAL_UART_Transmit(&huart2, txBuffer, sizeof(txBuffer), HAL_MAX_DELAY);
//								HAL_Delay(1000);




			if (isMenuLock == false) {

				menuSelected = Tick();

			}
			MenuDecode();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
		/* User can add his own implementation to report the HAL error return state */
		__disable_irq();
		while (1)
			;
		{

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
