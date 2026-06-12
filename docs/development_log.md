# Development Log

## 2026-04-27
- Project initialized

## 2026-04-29
- Imported existing STM32 project into Github repo
- Verified LED blinking on STM32 working after import
- Debugged printoff feature

## 2026-04-30
- Completed UART Transmit with Polling, IT, and DMA Normal and Circular mode

## 2026-05-01
- Completed UART Receive with Polling and IT
- Implemented error handling for UART Transmit and UART receive
- Attempted to implemented local data logging

## 2026-05-03
- Completed UART Receive with DMA + Normal/Circular mode + IDLE
- Implemented timer

## 2026-05-05
- Set up and tested I2C for OLED with SSD1306 OLED
- Reference library github: https://github.com/adafruit/adafruit_ssd1306

## 2026-05-07
- Seperated OLED and LED code from main.c

## 2026-05-10
- Seperated UART-Tx and UART-Idle-Rx code from main.c
- Cleaned UART-Tx and UART-Rx code, keeping UART-Tx DMA Normal and UART-Idle-Rx Normal

## 2026-05-12
- Allowed variable input for UART Tx values

## 2026-05-14
- Developed buffer reset method for UART Rx messages  

## 2026-05-17
- Moved connection between STM32 and OLED onto a breadboard to allow multiple I2C device connections to STM32
- Implemented error handling for OLED

## 2026-05-18
- Implemented hardware scrolling on OLED text

## 2026-05-21
- Implemented BME280 using I2C
- Reference code github: https://github.com/Afebia/BME280-STM32-V2

## 2026-05-23
- Displayed all BME280 data onto OLED
- Revamped OLED display layout and error logging 
- Added toggle flag for OLED and BME to allow for software enabling and disabling

## 2026-05-26
- Implemented MPU6050 using I2C
- Reference code: https://controllerstech.com/how-to-interface-mpu6050-gy-521-with-stm32/

## 2026-05-29
- Implemented roll and pitch calculations and Kalman filter algorithm for MPU6050
- Reference code: https://github.com/leech001/MPU6050/blob/ 
- Implemented pagination on OLED to avoid screen overflow
- Added I2C scanner code to detect connected I2C devices

## 2026-06-02
- Replaced pull-up resistors (from 4.7K to 10K) to accomodate for pre-existing pull-up resistors in sensors
- WHO_AM_I register for MPU6050 model (SY-104) readback is 72 by default
- Incorrect gyroscope readings on the Z-axis

## 2026-06-09
- Implemented MPU6050 Calibration
- Enhanced error handling for MPU6050 functions
- Adjusted OLED screen content

## 2026-06-11
- Debugged MPU6050 values
- Implemented motion status for MPU6050
- Disabled Kalman