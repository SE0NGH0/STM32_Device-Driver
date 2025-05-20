/*
 * bmp180_internals.h
 *
 *  Created on: Apr 4, 2025
 *      Author: microsoft
 */

#ifndef INC_BMP180_INTERNALS_H_
#define INC_BMP180_INTERNALS_H_

#include  "bmp180.h"
#include <stdint.h>

#define BMP180_7_BIT_ADDRESS 0b1110111
#define BMP180_ADDRESS (BMP180_7_BIT_ADDRESS << 1);

#define CALIB 0xAA
#define ID 0xD0
#define SOFT 0xE0
#define CTRL_MEAS 0xF4
#define OUT_MSB 0xF6
#define OUT_LSB 0xF7
#define OUT_XLSB 0xF8

#define convert8bitto16bit(x, y) (((x) << 8 | (y)))
#define powerof2(x) (1  << (x))

static void bmp180_read(bmp180_t *bmp180, uint8_t reg, uint8_t *buffer, uint8_t size);
static void bmp180_write(bmp180_t *bmp180, uint8_t reg, uint8_t *buffer, uint8_t size);
static int bmp180_is_ready(bmp180_t *bmp180);
static int16_t _bmp180_read_ut(bmp180_t *bmp180);
static int32_t _bmp180_read_up(bmp180_t *bmp180);

#endif /* INC_BMP180_INTERNALS_H_ */
