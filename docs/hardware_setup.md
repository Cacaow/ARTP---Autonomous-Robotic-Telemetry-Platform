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
|  BME280 SDA   |      PB7      |
|               |               |

## UART Settings
|   Baud Rate   |    115200     |
|   Data Bits   |    8 bits     |
|    Parity     |     None      |
|   Stop Bits   |       1       |

## Notes
- Pull-up resistors required for I2C
- Power supply stability is critical