# ARTP - Autonomous Robotic Telemetry Platform

## Overview
Building a real-time embedded system with STM32 + ESP32 + Python dashboard.

### Key Features:
- STM32 real-time sensore processing (I2C/SPI)
- ESP32 WiFi telemetry streaming
- Python dashboard (real-time plotting)
- Modular firmware architecture

### System Architecture
(docs/architecture.png)

### Demo
- Robot motion: [insert video]
- Dashboard: [insert video]

### Tech stack
- STM32 (HAL)
- ESP32 (UART + WiFi)
- Python (matplotlib/tkinter)

### Highlights (To do)
- Design UART telemetry protocol
- Implement sensor fusion module
- Build real-time dashboard

### How to run (To do)

## Progress
- Day 1: Project initialized

- Day 2: STM32 setup + LED blink

- Day 3: Set up UART TX

- Day 4: Set up UART RX and error handling
 
- Day 5: UART RX in DMA + Idle + Normal mode 

- Day 6: I2C OLED set up

- Day 7: Moved OLED and LED test code into unique test module files

- Day 7: Moved UART Transmit DMA + Normal mode and UART Receive DMA + Idle + Normal into unique test module files

- Day 8: Allowed variable input for UART Tx values

- Day 9: Developed buffer reset method for UART Rx messages  

- Day 10: Moved connection between STM32 and OLED onto a breadboard and implemented error handling for OLED

- Day 11: Implemented hardware scrolling on OLED text

- Day 12: Implemented BME280

- Day 13: Displayed all BME280 data into OLED

- Day 14: Implement MPU6050 basic functions

- Day 15: Implemented roll and pitch calculations and Kalman filter algorithm for MPU6050

- Day 16: Replaced pull-up resistors

- Day 17: Implement MPU6050 Calibration and error handling 

- Day 18: Debugged MPU6050 values and implemented motion status for MPU6050

- Day 19: Added SPI interface for Micro SD Card

- Day 20: Added ESP32 for ground station

- Day 21 & 22: Added STM32 -> ESP32 UART communication

- Day 23: Updated ESP32 Webpage and communication with STM32

- Day 24: Fix ESP32 and STM32 PING-PONG response issue