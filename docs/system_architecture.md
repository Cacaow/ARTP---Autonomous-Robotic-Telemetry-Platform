# System Architecture

## Overview
The system consists of three main components:

1. STM32 (Real-time control)
2. ESP32 (Communication bridge)
3. Ground station (Visualization)

## Data Flow

Sensor -> STM32 -> UART -> ESP32 -> WiFi -> PC Dashboard

## Design Decisions

- UART chosen for simplicity and reliability
- ESP32 used instead of direct WiFi on STM32
- Modular firmware seperation

## Future Improvements

- Implement nested array for UART Tx to avoid missing transmissions due to short period
- Add ROS interface
- Replace UART with SPI for higher speed
- Re-enable Kalman filter

