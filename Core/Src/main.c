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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
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

typedef void (*ButtonCallback)(uint8_t);

typedef struct {
    GPIO_TypeDef *gpio_port;
    uint16_t gpio_pin;
    ButtonCallback callback;
} Button;

Button buttons[] = {
    {GPIOD, BUTTON_Confirm_Pin, NULL},
    {GPIOD, BUTTON_Up_Pin, NULL},
    {GPIOD, BUTTON_Down_Pin, NULL}
};



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

void init_motor(Motor *motor,TIM_HandleTypeDef *htim, uint32_t channel, int speed, bool direction)
{
	 HAL_TIM_PWM_Start(motor->htim, motor->channel);
	 motor->htim=htim;
	 motor->channel=channel;
	 motor->speed=0;
	 motor->dir=1;
}


void motor_set_speed(Motor *motor, int speed)
{
	if (speed>100)
	{
		speed=100;
	}
	else if (speed <0)
	{
		speed=0;
	}
	motor->speed=speed;
}


void motor_set_direction(Motor *motor, GPIO_TypeDef *gpio_port, uint16_t gpio_pinFW, uint16_t gpio_pinBW, bool direction)
{
	if (direction!=0 && direction !=1)
	{
		printf("direction not correct\n");
	}
	else
	{
		motor->dir=direction;
	}

	            HAL_GPIO_WritePin(gpio_port, gpio_pinFW,  direction ? GPIO_PIN_SET : GPIO_PIN_RESET);
	            HAL_GPIO_WritePin(gpio_port, gpio_pinBW,  !direction ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void button_up_callback(int current_mode, int current_selection)
{
    if (mode == 0)
    {
        if (current_selection > 0)
            current_selection++;
    }
}



void button_down_callback(int current_mode, int current_selection)
{
    if (mode == 0)
    {
        if (current_selection > 0)
            current_selection--;
    }
}


void motor_info(Motor *motor)
{
printf("Motor Speed: %d\n", motor->speed);
printf("Motor Direction %d\n", motor->dir);
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
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  Robot *andrzej;

 init_motor(&andrzej->MotorLeft, &htim3, TIM_CHANNEL_3, 0, 1);
 init_motor(&andrzej->MotorRight, &htim3, TIM_CHANNEL_4, 0, 1);


 int current_mode=0;
int mode_change =0;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	    switch (current_mode) {
	        case 0: //mode 0, print menu options;


	            printf("Hello, choose mode you wanna operate?\n");



	            printf("current mode to choose is %d\n", mode_change);//print on the screen




				;
	            break;
	        case 1://mode 1, autonomous driving

	            break;
	        case 2://mode 2, driving from controller
	            printf("You selected Option 2.\n");
	            break;
	        case 3://mode 3 fight mode
	            printf("You selected Option 3.\n");
	            break;
	        default:
	            printf("Invalid choice.\n");
	            break;
	    }


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
