# Hardware Setup

## Components
- STM32 Nucleo-F411RE
- BME280 sensor (I2C)
- OLED display
- ESP32
- DHT22

## Wiring
|   Component   |   STM32 Pin   |
|---------------|---------------|
|   LED GPIO    |      PA5      |
|    UART TX    |      PA2      |
|    UART RX    |      PA3      |
|  BME280 SDA   |      PB7      |
|               |               |

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