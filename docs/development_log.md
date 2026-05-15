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

## 2026-05-07
- Seperated OLED and LED code from main.c

## 2026-05-10
- Seperated UART-Tx and UART-Idle-Rx code from main.c
- Cleaned UART-Tx and UART-Rx code, keeping UART-Tx DMA Normal and UART-Idle-Rx Normal

## 2026-05-12
- Allowed variable input for UART Tx values

## 2026-05-14
- Developed buffer reset method for UART Rx messages  