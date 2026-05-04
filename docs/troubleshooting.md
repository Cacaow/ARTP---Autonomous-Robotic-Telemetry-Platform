# TroubleShooting

## Issue: Debug message did not appear in ITM Data Console
- Cause: Core Clock did not matche the HCLK value in STM32CubeMX 
- Fix: Adjusted values to match

## Issue: 

## Issue: Could not create or access error logging file
- Cause: STM32 runs on the STM32 board, not on local PC drive
- Fix: Add microSD card in the future

## Issue: UART Receive Circular mode data not correct
- Cause: Data buffer copy potential bug
- Fix: Future fix

## Issue: UART Receive Error Code - 2 (HAL Busy)
- Cause: RxEventCallBack is called twice simultaneously
- Fix: After starting DMA, disable the half transfer interrupt by implementing this line: 
       __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);