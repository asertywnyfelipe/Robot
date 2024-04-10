/*
 * motor.h
 *
 *  Created on: Apr 10, 2024
 *      Author: hudy
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_



typedef struct {
	TIM_HandleTypeDef *htim; // Pointer to TIM_HandleTypeDef for the motor PWM
	uint32_t channel;        // TIM channel number for the motor PWM
	int speed;               // Motor speed (0-100)
	bool dir;           // Motor direction (1: forward, 0: reverse)
} Motor;


void init_motor(Motor *motor, TIM_HandleTypeDef *htim, uint32_t channel,
		int speed, bool direction);

void motor_set_speed(Motor *motor, int speed);

void motor_set_direction(Motor *motor, GPIO_TypeDef *gpio_port,
		uint16_t gpio_pinFW, uint16_t gpio_pinBW, bool direction);

#endif /* INC_MOTOR_H_ */
