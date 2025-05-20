#include "led.h"
#include "extern.h"
void led_all_on(void)
{
#if 1
	//printf("int %d\n", sizeof(int)); // 4로 찍히는지 확인
	GPIOB->ODR = 0xff;
#else
	//	HAL_GPIO_WritePin(GPIOB, 0Xff, 1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|
							 GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 1);
#endif
}

void led_all_off(void)
{
#if 1
	//printf("int %d\n", sizeof(int)); // 4로 찍히는지 확인
	GPIOB->ODR = 0x00;
#else
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|
							 GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0);
#endif
}

void shift_left_ledon(void)
{
	for(int count = 0; count < 8; count++)
	{
		GPIOB->ODR = 0x01 << count;
		HAL_Delay(100);
	}
	led_all_off();
	HAL_Delay(100);

#if 0
	char GPIO_number[8] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
						   GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
	for(int count = 0; count < 8; count++)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_number[count], 1);
				HAL_Delay(100);
				HAL_GPIO_WritePin(GPIOB, GPIO_number[count], 0);
			}
			led_all_off();
			HAL_Delay(100);
#endif
}

void shift_right_ledon(void)
{
	for(int count = 0; count < 8; count++)
	{
		GPIOB->ODR = 0x80 >> count;
		HAL_Delay(100);
	}
	led_all_off();
	HAL_Delay(100);
#if 0
	char GPIO_number[8] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
								   GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
		for(int count = 7; count >= 0; count--)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_number[count], 1);
			HAL_Delay(100);
			HAL_GPIO_WritePin(GPIOB, GPIO_number[count], 0);
		}
		led_all_off();
		HAL_Delay(100);
#endif
}

void shift_left_keep_ledon(void)
{
	for(int count = 0; count < 8; count++)
	{
		GPIOB->ODR |= 0x01 << count;
		HAL_Delay(100);
	}
	led_all_off();
	HAL_Delay(100);
}

void shift_right_keep_ledon(void)
{
	for(int count = 0; count < 8; count++)
	{
		*( unsigned int *)GPIOB_ODR |= 0x80 >> count;
		HAL_Delay(100);
	}
	led_all_off();
	HAL_Delay(100);
#if 0
	char GPIO_number[8] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
								   GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
		for(int count = 7; count >= 0; count--)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_number[count], 1);
			HAL_Delay(100);

		}
		led_all_off();
		HAL_Delay(100);
#endif
}



void flower_on(void)
{
#if 1 // 구조체 pointer member 변수
	for(int i = 0; i < 4; i++)
			{
				GPIOB->ODR |= 0x10 << i | 0x08 >> i;
				HAL_Delay(200);
			}
			led_all_off();
			HAL_Delay(200);
#endif
#if 0
	char GPIO_number[8] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
						   GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
	for(int count = 3; count >= 0; count--)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_number[count]|GPIO_number[7-count], 1);
				HAL_Delay(100);

			}
			led_all_off();
			HAL_Delay(100);
#endif
}

void flower_off(void)
{
	led_all_on();
	HAL_Delay(100);
	for(int i = 0; i < 4; i++)
	{
		GPIOB->ODR &= ~(0x80 >> i | 0x01 << i);
		HAL_Delay(100);
	}
	led_all_off();
	HAL_Delay(100);

#if 0
	led_all_on();
	HAL_Delay(100);
	for(int i = 0; i < 4; i++)
	{
		* (unsigned int *)GPIOB_ODR &= ~(0x80 >> i | 0x01 << i);
		HAL_Delay(100);
	}
	led_all_off();
	HAL_Delay(100);
#endif
}

extern SPI_HandleTypeDef hspi2;

void led_main(void)
{
	uint8_t led_buff[8] = {0xFF, 0x0F, 0xF0, 0x00,0xFF, 0x0F, 0xF0, 0x00};

	while (1)
	{
#if 1
		  HAL_SPI_Transmit(&hspi2,led_buff, 1, 1);
		  GPIOB->ODR &= ~GPIO_PIN_13;   // latch핀을 pull-down ODR(Output Data Register)
		  GPIOB->ODR |= GPIO_PIN_13;    // latch핀을 pull-up ODR(Output Data Register)
		  HAL_Delay(500);
		  HAL_SPI_Transmit(&hspi2, &led_buff[3], 1, 1);
		  GPIOB->ODR &= ~ GPIO_PIN_13;
		  GPIOB->ODR |= GPIO_PIN_13;
		  HAL_Delay(500);
#else
		  for (int i=0; i < 4; i++)
		  {
			  HAL_SPI_Transmit(&hspi2, &led_buff[i], 1, 1);
			  GPIOB->ODR &= ~ GPIO_PIN_13;   // latch핀을 pull-down
			  GPIOB->ODR |= GPIO_PIN_13;   //  // latch핀을 pull-up
			  HAL_Delay(1000);
		  }
#endif

//		(*GPIOB).ODR |= GPIO_PIN_0; // LED ON
//		HAL_Delay(500);
//		GPIOB->ODR &= ~GPIO_PIN_0;// LED OFF GPIOB 구조체 특성으로 GPIO_Typedeff에 정의된 ODR에 접근
//								  // 즉 GPIOB_ODR 주소값인 0x40020414에 접근하여
//		GPIOB->ODR ^= GPIO_PIN_1; // LED1 toggle

//		led_all_on();
//		led_all_off();
//		shift_left_ledon();
//		shift_right_ledon();
//		shift_left_keep_ledon();
//		shift_right_keep_ledon();
//		flower_on();
//		flower_off();
//		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
//		for (int i = 0; i < 50; i++)
//			delay_us(1000);


	}
}
