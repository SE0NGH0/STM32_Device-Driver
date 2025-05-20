/*
 * bmp180.c
 *
 *  Created on: Apr 4, 2025
 *      Author: microsoft
 */

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  // abs() 함수를 사용하기 위해 추가
#include "i2c_lcd.h"

// 타임아웃 1000ms 설정
#define BMP180_TIMEOUT_MS 1000

// BMP180 I2C 주소 (8비트 주소 체계: 좌측 쉬프트)
// BMP180 센서의 기본 I2C 주소인 0x77을 왼쪽으로 1비트 시프트하여 8비트 형식으로 변환
// HAL 라이브러리에서는 8비트 주소를 사용하므로
#define BMP180_ADDRESS (0x77 << 1)

// hi2c3는 BMP180 센서와 통신하기 위해 사용되는 I2C 인터페이스의 핸들러
extern I2C_HandleTypeDef hi2c3;

// BMP180 레지스터 및 명령어
// 보정 계수(캘리브레이션 데이터)를 저장한 EEPROM의 시작 주소 0xAA
#define BMP180_REG_CALIB_START 0xAA
// 센서 제어 레지스터 주소(명령어 쓰기를 통해 온도/압력 측정 시작)
#define BMP180_REG_CONTROL     0xF4
// 측정 결과(데이터)의 최상위 바이트를 읽는 주소
#define BMP180_REG_OUT_MSB     0xF6
// 온도 측정을 위한  명령어(0x2E를 제어 레지스터에 쓰면 온도 측정 시작)
#define BMP180_CMD_READ_TEMP   0x2E
// 기압 측정을 위한 기본 명령어(oversampling 설정에 따라 변경)
#define BMP180_CMD_READ_PRESS  0x34

// 보정 계수 변수
// BMP180 센서는 개별 칩마다 고유의 보정 계수를 EEPROM에 저장
// 온도와 기압을 정확하게 계산하기 위해 이 계수들을 읽어와서 사용
// AC1, AC2, AC3, B1, B2, MB, MC, MD는 부호가 있는 16비트 정수형
int16_t AC1, AC2, AC3, B1, B2, MB, MC, MD;
// AC4, AC5, AC6는 부호가 없는 16비트 정수형
uint16_t AC4, AC5, AC6;

// 보정 계수 읽기 함수
// I2C 인터페이스를 통해 BMP180의 보정 데이터를 읽어오는 함수
void BMP180_ReadCalibrationCoefficients(I2C_HandleTypeDef *hi2c)
{
	// calib_data 배열(22바이트 크기)은 보정 데이터 전체 저장
    uint8_t calib_data[22];
    if (HAL_I2C_Mem_Read(hi2c, BMP180_ADDRESS, BMP180_REG_CALIB_START,
    		I2C_MEMADD_SIZE_8BIT, calib_data, 22, BMP180_TIMEOUT_MS) != HAL_OK)
    {
        // 타임아웃 또는 통신 오류 시 처리: 여기서는 간단히 리턴
        return;
    }
    // 보정 데이터는 8비트 단위로 읽어오므로 상우 ㅣ바이트와 하위 바이트를 결합하여 16비트값으로 만듦
    // (calib_data[0] << 8 | calib_data[1])는 첫 번째 16비트 값(AC1)을 만듦
    // 나머지 보정 계수들도 같은 방식으로 계산
    // 부호 있는 값은 (int16_t)로, 부호 없는 값은 (uint16_t)로 캐스팅
    AC1 = (int16_t)(calib_data[0] << 8 | calib_data[1]);
    AC2 = (int16_t)(calib_data[2] << 8 | calib_data[3]);
    AC3 = (int16_t)(calib_data[4] << 8 | calib_data[5]);
    AC4 = (uint16_t)(calib_data[6] << 8 | calib_data[7]);
    AC5 = (uint16_t)(calib_data[8] << 8 | calib_data[9]);
    AC6 = (uint16_t)(calib_data[10] << 8 | calib_data[11]);
    B1  = (int16_t)(calib_data[12] << 8 | calib_data[13]);
    B2  = (int16_t)(calib_data[14] << 8 | calib_data[15]);
    MB  = (int16_t)(calib_data[16] << 8 | calib_data[17]);
    MC  = (int16_t)(calib_data[18] << 8 | calib_data[19]);
    MD  = (int16_t)(calib_data[20] << 8 | calib_data[21]);
}

// 원시 온도 값 읽기
// 온도 측정을 위해 센서에 명령을 보내고 원시 온도 데이터를 읽어오는 함수
int16_t BMP180_ReadRawTemperature(I2C_HandleTypeDef *hi2c)
{
	// cmd 변수에 온도 측정 명령어(0x2E)를 저장
    uint8_t cmd = BMP180_CMD_READ_TEMP;
    if (HAL_I2C_Mem_Write(hi2c, BMP180_ADDRESS, BMP180_REG_CONTROL,
    		I2C_MEMADD_SIZE_8BIT, &cmd, 1, BMP180_TIMEOUT_MS) != HAL_OK)
    {
        // 통신 타임아웃 또는 오류 발생 시 -1 반환
    	return -1;
    }
    // 센서가 온도 측정을 완료할 때까지 대기 시간
    HAL_Delay(5);
    // 0xF6 주소에서 2바이트를 읽어와 원시 온도 데이터(raw 배열) 저장
    uint8_t raw[2];
    if (HAL_I2C_Mem_Read(hi2c, BMP180_ADDRESS, BMP180_REG_OUT_MSB,
    		I2C_MEMADD_SIZE_8BIT, raw, 2, BMP180_TIMEOUT_MS) != HAL_OK)
    {
        return -1;
    }
    // 상위 바이트와 하위 바이트를 결합하여 16비트 정수형 온도 값을 반환
    return (int16_t)((raw[0] << 8) | raw[1]);
}

// 원시 기압 값 읽기 (oss: oversampling setting, 0~3)
// 기압 측정을 위해 원기 기압 데이터를 읽어오는 함수
int32_t BMP180_ReadRawPressure(I2C_HandleTypeDef *hi2c, uint8_t oss)
{
	// oss는 oversampling setting으로 0~3 사이의 값을 사용
	// 기압 명령어에 oversampling 지트를 추가하기 위해 (oss << 6)을 더함
    uint8_t cmd = BMP180_CMD_READ_PRESS + (oss << 6);
    if (HAL_I2C_Mem_Write(hi2c, BMP180_ADDRESS, BMP180_REG_CONTROL,
    		I2C_MEMADD_SIZE_8BIT, &cmd, 1, BMP180_TIMEOUT_MS) != HAL_OK)
    {
        return -1;
    }
    // oversampling 설정에 따라 측정 완료까지 대기해야 하는 시간이 달라짐
    // oss가 0이면 5ms, 1이면 8ms, 2이면 14ms, 3이면 26ms 대기
    // 잘못된 값이 들어올 경우 기본적으로 5ms 대기
    switch(oss)
    {
        case 0: HAL_Delay(5);  break;
        case 1: HAL_Delay(8);  break;
        case 2: HAL_Delay(14); break;
        case 3: HAL_Delay(26); break;
        default: HAL_Delay(5); break;
    }
    //0xF6 주소에서 3바이트를 읽어옴, 이 데이터는 기압 측정의 결과를 포함
    uint8_t raw[3];
    if (HAL_I2C_Mem_Read(hi2c, BMP180_ADDRESS, BMP180_REG_OUT_MSB, I2C_MEMADD_SIZE_8BIT, raw, 3, BMP180_TIMEOUT_MS) != HAL_OK)
    {
    	return -1;
    }
    // 읽어온 3바이트 데이터를 결합하여 24비트 정수형 값을 만든 후 oversampling 설정에 따라 오른쪽으로 시프트하여 최종 원시 기압 값(up)을 계산
    // 계산괸 up 값을 반환
    int32_t up = (((int32_t)raw[0] << 16) | ((int32_t)raw[1] << 8) | raw[2]) >> (8 - oss);
    return up;
}

// 온도 계산 함수 (BMP180 데이터시트에 따른 계산식)
// 반환값은 0.1°C 단위의 온도 값
// 원시 온도 값(UT)을 이용하여 보정된 온도를 계산하는 함수
int32_t BMP180_ComputeTemperature(int16_t UT)
{
	// 중간 계산에 사용할 변수 X1, X2, B5 선언
    int32_t X1, X2, B5;
    // 데이터 시트에 따른 공식의 일부로 UT에서 AC6을 빼고 AC5를 곱한 후
    // 2^15로 나누기 위해 비트 시프트 연산 (>> 15)을 수행
    X1 = ((UT - AC6) * AC5) >> 15;
    // MC를 2^11(2048)배한 후 X1과 MD의 합으로 나눔
    // 이 계산은 온도 보정을 위한 중간 단계
    X2 = (MC << 11) / (X1 + MD);
    // X1과 X2를 더해 B5값을 얻음, B5는 최종 온도 계산에 중요한 중간 변수
    B5 = X1 + X2;
    // B5에 8을 더한 후 2^4(16)로 나눠 최종 보정 온도 구함
    // 반환 값을 0.1도 단위의 온도
    return (B5 + 8) >> 4;
}

// 기압 계산 함수 (BMP180 데이터시트에 따른 계산식)
// 단, 내부에서 온도 보정 값(B5)을 다시 계산합니다.
// 온도 보정을 포함하여 원시 기압(UP)과 온도(UT)를 이용해 보정된 기압 계산
int32_t BMP180_ComputePressure(int32_t UT, int32_t UP, uint8_t oss)
{
	// 여러 중간 변수(X1, X2, B5, B6, X3, p)와 보정용 변수(B4, B7)를 선언
    int32_t X1, X2, B5, B6, X3, p;
    uint32_t B4, B7;

    // 온도 보정 계산
    // 기압 계산 시에도 온도 보정 값(B5)이 필요하므로 앞서 온도 계산과 동일한 공식으로 B5를 재계산
    X1 = ((UT - AC6) * AC5) >> 15;
    X2 = (MC << 11) / (X1 + MD);
    B5 = X1 + X2;

    // 기압 보정 계산
    // B5에서 4000을 빼서 B6을 구함, 기압 보정 공식의 일부
    B6 = B5 - 4000;
    // B6의 제곱을 오른쪽으로 12비트 시프트한 후 B2와 곱하고 다시 2^11(시프트 11)로 나눔
    X1 = (B2 * ((B6 * B6) >> 12)) >> 11;
    // AC2와 B6을 곱한 후 2^11로 나눈 값을 X2에 저장하고 X1과 X2를 더해 X3을 구함
    X2 = (AC2 * B6) >> 11;
    X3 = X1 + X2;
    // AC1에 4를 곱하고 X3를 더한 후 oversampling setting(oss)에 따라
    // 왼쪽 시프트를 수행하고 2를 더한 후 4로 나누어 B3를 계산
    int32_t B3 = (((AC1 * 4 + X3) << oss) + 2) / 4;
    // AC3와 B6를 곱한 값을 오른쪽으로  13비트 시프트하여 X1
    X1 = (AC3 * B6) >> 13;
    // B6의 제곱을 오른쪽으로 12비트 시프트한 후 B1과 곱하고 2^16으로 나눈 값을 X2
    X2 = (B1 * ((B6 * B6) >> 12)) >> 16;
    // X1과 X2를 더하고 2를 더한 후 2^2(4)로 나누어 최종 X3
    X3 = ((X1 + X2) + 2) >> 2;
    // X3에 32768을 더한 후 AC4와 곱하고 오른쪽으로 15비트 시프트하여 B4값을 계산
    // B4는 보정 과정의 분모 역할
    B4 = (AC4 * (uint32_t)(X3 + 32768)) >> 15;
    // 원시 기압 값 UP에서 B3를 뺀 후 50000을 oversampling에 맞게 오른쪽으로 시프트한 값과 곱하여 B7을 계산
    B7 = ((uint32_t)UP - B3) * (50000 >> oss);

    // B7의 값에 따라 두 가지 경우로 나누어 p값을 계산
    // 조건문은 B7이 0x80000000보다 작은지 확인하여 p를 B4로 나누고 2를 곱하는 방식으로 계산
    if(B7 < 0x80000000)
        p = (B7 * 2) / B4;
    else
        p = (B7 / B4) * 2;

    // p를 오른쪽으로  8비트 시프트한 후 제곱하여 X1에 저장하고 다시 3038을 곱한 후 2^16으로 나눔
    X1 = (p >> 8) * (p >> 8);
    X1 = (X1 * 3038) >> 16;
    // X2는 p에 -7357을 곱한 후 2^16으로 나눔
    X2 = (-7357 * p) >> 16;
    // X1, X2 그리고 3791을 더한 후 2^4(16)로 나누어 p에 더함
    // 이 계산을 통해 최종 보정된 기압 값 도출
    p = p + (((X1 + X2 + 3791) >> 4));

    // 계산된 기압 p값을 반환, 단위는 파스칼(Pa)
    return p; // 단위: Pa
}

// BMP180 센서로부터 데이터를 읽어 LCD와 UART로 출력하는 메인 함수
void bmp_main(void)
{
	// lcd_line1, lcd_line2는 16자를 저장할 문자열 배열, LCD의 각 행에 출력할 내용 저장
	char lcd_line1[17];
	char lcd_line2[17];

	// I2C LCD 초기화
	i2c_lcd_init(); // LCD 초기화 함수

    // BMP180 보정 계수 읽기
	// BMP180 센서의 보정 데이터를 읽어와 전역 변수들(AC1~MD)에 저장
	// hi2c3 핸들러를 사용하여 I2C통신으로 센서의 EEPROM 영역에서 데이터를 읽어옴
    BMP180_ReadCalibrationCoefficients(&hi2c3);

    // 센서 측정 및 출력 작업을 계속 반복하기 위해 무한 루프 시작
    while(1)
    {
        // 원시 온도 및 기압 읽기
    	// BMP180_ReadRawTemperature함수를 호출하여 원시 온도 값(UT)를 읽어옴
        int16_t UT = BMP180_ReadRawTemperature(&hi2c3);
        // BMP180_ReadRawPressure함수를 호출하여 원시 기압 값(UP)을 읽음, oversampling setting을 0으로 사용
        int32_t UP = BMP180_ReadRawPressure(&hi2c3, 0);

        // 통신 오류 시 -1 반환된 경우 에러 처리
        if (UT == -1 || UP == -1)
        {
        	move_cursor(0, 0);
        	lcd_string("                "); // 16칸 공백
        	move_cursor(1, 0);
        	lcd_string("                "); // 16칸 공백
            sprintf(lcd_line1, "Sensor Error!");
            sprintf(lcd_line2, "Timeout");
            move_cursor(0, 0);
            lcd_string(lcd_line1);
            move_cursor(1, 0);
            lcd_string(lcd_line2);
            printf("BMP180 Timeout or Communication Error\r\n");
            HAL_Delay(1000);
            continue;
        }

        // 온도 계산 (0.1°C 단위)
        // 읽어온 UT값을 이용하여 보정된 온도 계산
        // 반환 값은 0.1도 단위
        int32_t temperature = BMP180_ComputeTemperature(UT);
        // 보정식을 이용한 실제 기압 계산 (Pa 단위 -> hPa로 변환)
        // UT와 UP값을 사용해 보정된 기압 계산
        int32_t pressure = BMP180_ComputePressure(UT, UP, 0) / 100;

        // 첫 번째 줄에 온도값, 두 번째 줄에 기압값을 문자열로 포맷
        sprintf(lcd_line1, "Temp: %2ld.%ld C", temperature / 10, abs(temperature) % 10);
        sprintf(lcd_line2, "Press: %4ld hPa", pressure);

        // LCD 출력
        move_cursor(0, 0);
        lcd_string(lcd_line1);
        move_cursor(1, 0);
        lcd_string(lcd_line2);
        HAL_Delay(1000);

        // UART를 통해 결과 출력 (온도는 소수 첫째자리까지 출력)
        char msg[100];
        sprintf(msg, "Temp: %ld.%ld C, Pressure: %ld hPa\r\n", temperature / 10, abs(temperature) % 10, pressure);
        printf("%s", msg);
        HAL_Delay(1000); // 1초 주기로 측정
    }
}
