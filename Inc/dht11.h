#include <string.h>        // memset() 사용
#include <stdio.h>
#include <stdlib.h>

/*
 * DHT11 센서 헤더 파일
 *
 * DHT11은 단일 와이어 인터페이스를 사용하는 온습도 센서입니다.
 * 데이터 라인은 기본적으로 풀업되어 있어야 하며, 내부 풀업 활성화 또는
 * 외부 10kΩ 풀업 저항 사용을 권장합니다.
 *
 * 데이터 전송은 다음과 같이 진행됩니다.
 * 1. MCU가 시작 신호(Start Signal)를 보냄:
 *    - 데이터 라인을 LOW로 최소 18ms 이상 유지 (여기서는 20ms 사용)
 *    - 이후 HIGH로 전환하고 입력 모드로 설정
 * 2. 센서 응답(Response):
 *    - 센서는 80us 정도 LOW로 응답한 후 80us 정도 HIGH로 응답합니다.
 * 3. 데이터 전송:
 *    - 센서는 40비트(5바이트)의 데이터를 전송합니다.
 *      각 비트는 50us LOW 펄스 후 HIGH 펄스 길이로 '0'(약 26~28us) 또는 '1'(약 70us)을 구분합니다.
 * 4. 데이터 형식:
 *    - Byte0: 습도의 정수 부분
 *    - Byte1: 습도의 소수 부분 (DHT11은 일반적으로 0)
 *    - Byte2: 온도의 정수 부분
 *    - Byte3: 온도의 소수 부분 (DHT11은 일반적으로 0)
 *    - Byte4: 체크섬 (Byte0~Byte3의 합과 동일해야 함)
 */
#define GPIO_DHT11_MODER *(unsigned int *)0x40020000
#define GPIO_DHT11_OTYPER *(unsigned int *)0x40020004
#define GPIO_DHT11_OPSEEDR *(unsigned int *)0x40020008
#define GPIO_DHT11_PUPDR *(unsigned int *)0x4002000C
#define GPIO_DHT11_IDR *(unsigned int *)0x40020010
#define GPIO_DHT11_ODR *(unsigned int *)0x40020014


extern uint8_t us_count;
enum state_define {
   OK,            // 통신 정상
   TIMEOUT,      // 응답 시간 초과
   VALUE_ERROR,   // 데이터 값 오류(체크섬 불일치 등)
   TRANS_ERROR      // 전송 에러 (추가 검증 실패 시)
};
extern enum state_define dht11_state;

void init_dht11(void);
void dht11_main(void);
