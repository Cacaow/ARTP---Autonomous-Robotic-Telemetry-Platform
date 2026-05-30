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

# Issue: Debugging error when implementing I2C
- Cause: Missing header file
- Fix: Add fonts header file and rename conf_template.h to conf.h

# Issue: OLED screen not on when running code
- Cause: Loose wire
- Fix: Insert wire firmly into port

# Issue: Unable to receive values through UART RX when UART TX is running
- Cause: DMA stop is called before value is received
- Fix: Move DMA stop to the UART deinitialize function

# Issue: UART Tx only transmit first four characters
- Cause: Using sizeof on an uint8_t* returns the size of the address which is always four (does not the full length of the data) 
- Fix: Replace sizeof with strlen(const char*) (which calculates the number of characters in the data)

# Issue: OLED SDA line has no voltage after connecting to STM32 through a breadboard
- Cause: Voltage will not be automatically pulled-up when connecting through a wire longer than 20cm
- Fix: Implement two 4.7K pull-up resistors between the 3.3V power source and SCL and SDA respectively (commonly required by I2C)

# Issue: Numerous undeclared errors
- Cause: Missing inclusions
- Fix: Add inclusions

# Issue: Unidentified float printf error
- Cause: STM32CubeIDE u_printf_float is disabled by default
- Fix: C/C++ Build -> Settings -> MCU Settings, enable "Use float with printf from newlib-nano (-u_printf_float)

# Issue: BME280 Undefined Chip Id error
- Cause: SDO connectivity issue
- Fix: Ensure SDO wire is fully connected

# Issue: MPU6050 readings equal to zero
- Currently Investigating