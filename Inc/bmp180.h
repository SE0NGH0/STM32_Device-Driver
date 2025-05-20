/*
 * bmp180.h
 *
 *  Created on: Apr 4, 2025
 *      Author: microsoft
 */

#ifndef INC_BMP180_H_
#define INC_BMP180_H_

#define I2C_LIB "stm32f4xx_hal.h"

#include I2C_LIB
#include <stdint.h>

// 각 모드에 따라 센서가 데이터를 읽어오는 방식과 대기 시간이 달라짐
enum bmp180_oversampling_settings{
	ultra_low_power, // 가장 낮은 전력 소비 모드
	standard, // 표준 모드
	high_resolution, // 높은 정밀도 모드
	ultra_high_resolution // 가장 높은 정밀도 모드
};

typedef struct bmp180_t {
	I2C_HandleTypeDef *hi2c3; // I2C3 사용

	float temperature;
	int32_t pressure;
	float altitude;
	int32_t  sea_pressure;

	enum bmp180_oversampling_settings oversampling_setting;
	uint8_t oss;

	int16_t AC1;
	int16_t AC2;
	int16_t AC3;
	uint16_t AC4;
	uint16_t AC5;
	uint16_t AC6;
	int16_t B1;
	int16_t B2;
	int32_t B3;
	uint32_t B4;
	int32_t B5;
	int32_t B6;
	uint32_t B7;
	int16_t MB;
	int16_t MC;
	int16_t MD;

} bmp180_t;

uint8_t bmp180_init(I2C_HandleTypeDef *hi2c3, bmp180_t *bmp180);
void bmp180_get_all(bmp180_t *bmp180);
void bmp180_get_temperature(bmp180_t *bmp180);
void bmp180_get_pressure(bmp180_t *bmp180);
void bmp180_get_altitude(bmp180_t *bmp180);
void bmp180_get_sea_pressure(bmp180_t *bmp180, int32_t sea_pressure);

#endif /* INC_BMP180_H_ */
