/*
 * motor.c
 *
 *  Created on: Apr 10, 2024
 *      Author: hudy
 */

#include "main.h"
#include <stdbool.h>
#include <stdint.h>
#include "tim.h"
#include "motor.h"


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

	HAL_GPIO_WritePin(gpio_port, gpio_pinFW, direction ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(gpio_port, gpio_pinBW, !direction ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
