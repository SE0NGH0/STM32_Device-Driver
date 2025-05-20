#include "main.h"

extern TIM_HandleTypeDef htim2;
extern uint8_t rx_data;


extern t_print o_prt;
extern UART_HandleTypeDef huart2;
extern volatile int TIM11_1ms_counter;
extern uint16_t SUM, RH, TEMP;

extern void led_main(void);
extern void button_led_toggle_test(void);
extern void ds1302_main(void);
extern void pc_command_processing(void);
extern void led_all_on(void);
extern void led_all_off(void);
extern void set_rtc(char *date_time);
extern void delay_us(int us);
extern void dht11_main(void);
extern void flash_main();
extern void flash_set_time(void);
extern void i2c_lcd_main(void);
extern void servo_motor_main(void);
extern void dotmatrix_main_test();
extern int dotmatrix_main_func(void);
