# 🌡️ BMP180 & LCD1602 Project  
**with STM32 Nucleo-F411RE**  

---

## 📌 프로젝트 개요

- BMP180, LCD1602, DHT11, DS1302 등의 센서 및 디스플레이 장치에 대한 **Device Driver 직접 구현**
- 각 부품의 **데이터시트 기반**으로 프로토콜 분석 → 코드 작성 → 임베디드 구현
- 드라이버 없이 **레지스터 직접 제어 및 타이밍 구현**에 집중

---

## 🔩 주요 구성요소

| 부품            | 설명 |
|-----------------|------|
| **STM32F411RE** | 메인 MCU |
| **BMP180**      | 온도, 기압 측정 (I2C) |
| **LCD1602**     | 문자 출력 디스플레이 (4비트 병렬) |
| **DHT11**       | 온습도 센서 (단일 GPIO 데이터 전송) |
| **DS1302**      | RTC 모듈 (시간 및 날짜 표시) |
| **DOT MATRIX**  | 시각적 출력 (Shift Register 연동) |

---

## 🛠 주요 구현 기능

### 📡 BMP180 센서 드라이버
- I2C 통신 기반
- 보정 계수 EEPROM에서 직접 읽기
- 원시 온도/기압 데이터 획득 및 보정 계산
- **무응답 시 타임아웃 처리 기능 포함**
- UART 및 LCD로 실시간 출력

### 💬 LCD1602 제어 (4-bit)
- 상위 4비트, 하위 4비트 분리 전송
- RS 핀으로 명령/데이터 구분
- Enable 핀 Falling Edge로 latch
- GPIO 제어 기반 직접 구현

### 🌡️ DHT11
- 정확한 타이밍 기반 단일 데이터 라인 통신
- 온도/습도 데이터 수신 및 출력

### ⏰ DS1302
- 실시간 시계 데이터 읽기/쓰기
- LCD, UART를 통한 시간 표시

---

## 💻 주요 코드 설명

### BMP180 - 센서 주소 및 초기화
```c
#define BMP180_ADDR (0x77 << 1)  // 8-bit 주소
HAL_I2C_Mem_Read(&hi2c3, BMP180_ADDR, START_ADDR, ADDR_SIZE, buffer, len, TIMEOUT);
```

### 보정 계수 읽기 및 결합
- 22byte EEPROM block → 11개 보정 계수 분리
- MSB + LSB 결합하여 16bit 정수형 저장

### 원시 온도/기압 측정
```c
// 온도 측정 시작
uint8_t cmd = 0x2E;
HAL_I2C_Mem_Write(&hi2c3, BMP180_ADDR, 0xF4, 1, &cmd, 1, 100);
HAL_Delay(5);
HAL_I2C_Mem_Read(&hi2c3, BMP180_ADDR, 0xF6, 1, data, 2, 100);
```

### LCD1602 - 4bit 데이터 전송
```c
void LCD_Send4Bit(uint8_t data) {
  HAL_GPIO_WritePin(GPIOx, D4, (data >> 0) & 0x01);
  HAL_GPIO_WritePin(GPIOx, D5, (data >> 1) & 0x01);
  HAL_GPIO_WritePin(GPIOx, D6, (data >> 2) & 0x01);
  HAL_GPIO_WritePin(GPIOx, D7, (data >> 3) & 0x01);
  LCD_Enable();
}
```

---

## 🎞️ 시연 영상

- 🔗 [동작 영상](https://youtube.com/shorts/3SWXgfUtDQU)

---

## 📊 오실로스코프 분석

- BMP180 통신 타이밍 분석
- LCD1602 4bit 데이터 전송 파형 확인

---

## 💡 느낀 점

> 📌 “드라이버 없이 직접 하드웨어와 통신하고, 동작을 확인하는 과정이 인상 깊었고, 소프트웨어와 하드웨어가 얼마나 밀접하게 연결되어 있는지를 체감함. 라이브러리에만 의존하지 않고 **데이터시트 기반의 직접 제어 능력**을 기르게 된 점이 큰 수확이었다.” – 박성호

---
