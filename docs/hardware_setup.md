# Hardware Setup

## Components
- STM32 Nucleo-F411RE 
- OLED display SSD1306 (I2C)
- BME280 sensor (I2C)
- MPU-6050 (I2C)
- ESP32 (UART)
- SD Card (SPI)
- TB6612FNG x 2 (GPIO)


## Wiring
|   Component   |   STM32 Pin   |
|---------------|---------------|
|   LED GPIO    |      PA5      |
|               |               |
|    UART TX    |      PA2      |
|    UART RX    |      PA3      |
|               |               |
| I2C OLED VCC  |      3V3      |
| I2C OLED GND  |      GND      |
| I2C OLED SCL  |    PB8/SCL    |
| I2C OLED SDA  |    PB9/SDA    |
|               |               |
|  BME280 SDA   |      PB7      |


## UART Settings
|   Baud Rate   |    115200     |
|   Data Bits   |    8 bits     |
|    Parity     |     None      |
|   Stop Bits   |       1       |
|    TX Mode    |      DMA      |
|    RX Mode    |DMA+Normal+Idle|

## Notes
- Pull-up resistors required for I2C
- Power supply stability is critical