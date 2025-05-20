/*
 * lcd1602.h
 *
 *  Created on: 2025. 4. 6.
 *      Author: microsoft
 */

#ifndef INC_LCD1602_H_
#define INC_LCD1602_H_

// --------------------[ LCD I2C 관련 설정 ]--------------------

// PCF8574의 기본 주소 (0x27) <<1 은 HAL_I2C 함수가 8비트 주소를 요구하기 때문
#define I2C_LCD_ADDRESS (0x27<<1)

// 백라이트 ON 설정 (PCF8574의 P3 핀에 연결됨, 보통 LCD 밝기 조절용)
#define BACKLIGHT_ON 0x08

// --------------------[ LCD 명령어 정의 ]--------------------

// LCD 화면 켜기 (화면 ON, 커서 OFF, 커서 깜빡임 OFF)
#define DISPLAY_ON 0x0C

// LCD 화면 끄기
#define DISPLAY_OFF 0x08

// LCD 화면 지우기 (클리어)
// 실행 후 반드시 2ms 이상의 딜레이 필요
#define CLEAR_DISPLAY 0x01

// 커서를 홈 위치 (0,0)으로 복귀
#define RETURN_HOME 0x02

// --------------------[ LCD 함수 선언 ]--------------------

// 명령어 전송 함수
void lcd_command(uint8_t command);
void LCD_SendCommand(uint8_t cmd);
// 데이터(문자) 전송 함수
void lcd_data(uint8_t data);
void LCD_SendData(uint8_t data);
// LCD 초기화 함수
void i2c_lcd_init(void);

// 문자열 출력 함수
void lcd_string(uint8_t *str);

// 커서 이동 함수 (row : 행, column : 열)
void move_cursor(uint8_t row, uint8_t column);

#endif /* INC_LCD1602_H_ */
