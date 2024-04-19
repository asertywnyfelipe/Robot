/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, R4_Keyboard_Pin|R3_Keyboard_Pin|R2_Keyboard_Pin|R1_Keyboard_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, MOT1_Forward_Pin|MOT1_Backward_Pin|MOT2_Forward_Pin|MOT2_Backward_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = C3_Keyboard_Pin|C2_Keyboard_Pin|C1_Keyboard_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin */
  GPIO_InitStruct.Pin = R4_Keyboard_Pin|R3_Keyboard_Pin|R2_Keyboard_Pin|R1_Keyboard_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

//  /*Configure GPIO pins : PAPin PAPin PAPin PAPin */
//  GPIO_InitStruct.Pin = MOT1_Forward_Pin|MOT1_Backward_Pin|MOT2_Forward_Pin|MOT2_Backward_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
char read_keyboard() {

	HAL_GPIO_WritePin(GPIOD, R1_Keyboard_Pin, GPIO_PIN_RESET); //Pull the R1 low
	HAL_GPIO_WritePin(GPIOD, R2_Keyboard_Pin, GPIO_PIN_SET); // Pull the R2 High
	HAL_GPIO_WritePin(GPIOD, R3_Keyboard_Pin, GPIO_PIN_SET); // Pull the R3 High
	HAL_GPIO_WritePin(GPIOD, R4_Keyboard_Pin, GPIO_PIN_SET); // Pull the R4 High

	if (!(HAL_GPIO_ReadPin(GPIOB, C1_Keyboard_Pin)))   // if the Col 1 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C1_Keyboard_Pin)))
			; // wait till the button is pressed
		return '1';
	}

	if (!(HAL_GPIO_ReadPin(GPIOB, C2_Keyboard_Pin)))   // if the Col 2 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C2_Keyboard_Pin)))
			;
		// wait till the button is pressed
		return '2';
	}

	if (!(HAL_GPIO_ReadPin(GPIOB, C3_Keyboard_Pin)))   // if the Col 3 is low
	{
		while (!(HAL_GPIO_ReadPin(GPIOB, C3_Keyboard_Pin)))
			;
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
/* USER CODE END 2 */
