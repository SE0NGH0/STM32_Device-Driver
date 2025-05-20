/*
 * uart.h
 *
 *  Created on: Mar 31, 2025
 *      Author: user
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "main.h"

#define COMMAND_NUMBER 20
#define COMMAND_LENGTH 40

volatile uint8_t rx_buff[COMMAND_NUMBER][COMMAND_LENGTH];
volatile int rear;	// input index : HAL_UART_RxCpltCallback에서 집어 넣어 주는 index
volatile int front; // output index :
#endif /* INC_UART_H_ */
