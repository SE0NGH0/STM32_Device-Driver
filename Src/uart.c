
#include <string.h>
#include <stdio.h>

#include "uart.h"
#include "extern.h"


t_print o_prt;

void pc_command_processing(void);
/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * move from Drivers/STM32F4xx_HAL_driver/Src/stm32f4xx_hal_uart.c to here
  * 예) comportmaster로 부터 1char를 수신하면 HAL_UART_RxCpltCallback으로 진입
  * 	9600bps인 경우 RxCpltCallback를 수행 후 1ms이내에는 빠져 나가야 한다.
  * 	115200bps 						   86us
   */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	volatile static int i = 0;	// buffer의 index


	if (huart == &huart2)
	{
		if(rx_data == '\n')
		{
			rx_buff[rear++][i] = 0; // 문장의 끝은 항상 0
			rear %= COMMAND_NUMBER; // 원형큐
			i = 0; // 새로오는건 버퍼에 새로우 채워야함 (다른 메시지니치!)

			// 큐 full 여부 확인
		}
		else
		{
			// 그래야 버퍼에 넣기 (push)
			rx_buff[rear][i++] = rx_data;
			// command length 확인
		}
		HAL_UART_Receive_IT(&huart2, &rx_data, 1);	// 주의 : 반드시 집어 넣어야 다음 INT가 발생
	}
}

void show_command(void)
{

	char *cmd[] =
	{
			"\nsetrtc",
			"print_rtc",
			"printoff_rtc",
			"help"
	};

	for (int i = 0; i < 4; i++)
	{
		printf("%s\n", cmd[i]);
	}
}

void pc_command_processing(void)
{

   if (front != rear) // rx_buff에 data가 존재
   {
      printf("%s\n", rx_buff[front]);   // &rx_buff[front][0]
      if (strncmp((const char *)rx_buff[front], (const char *)"led_all_on", strlen("led_all_on")) == 0)
	   {
//    	  printf("find : led_all_on\n");
	   }
      else if (strncmp((const char *)rx_buff[front], (const char *)"led_all_off", strlen("led_all_off")) == 0)
	   {
//    	  printf("find : led_all_on\n");
	   }
      else if (strncmp((const char *)rx_buff[front], (const char *)"setrtc", strlen("setrtc")) == 0)
	   {
		  set_rtc(&rx_buff[front][6]);
	   }
      else if (strncmp((const char *)rx_buff[front], (const char *)"help", strlen("help")) == 0)
	   {
    	  show_command();
	   }
      else if (strncmp((const char *)rx_buff[front], (const char *)"print_rtc", strlen("print_rtc")) == 0)
	   {
		  o_prt.p_rtc = 1;
		  printf("print_rtc: %d\n", o_prt.p_rtc);
	   }
      else if (strncmp((const char *)rx_buff[front], (const char *)"printoff_rtc", strlen("printoff_rtc")) == 0)
	   {
		  o_prt.p_rtc = 0;
		  printf("print_rtc: %d\n", o_prt.p_rtc);
	   }




      front++;
      front %= COMMAND_NUMBER;
      // !!!! QUEUE full check하는 logic이 들어가야 한다. !!!!
   }
}
