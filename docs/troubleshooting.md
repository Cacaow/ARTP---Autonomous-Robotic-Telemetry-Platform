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
- Cause: Pre-existing pull-up resistors in sensors in parallel to added pull-up resistor on the SCL and SDA I2C channels
- Fix: Replace 4.7K pull-up resistors with 10K pull up resistors

# Issue: I2C unrecognized
- Cause: STM32 hung up during debugging
- Fix: Cycle power

# Issue: Incorrect MPU6050 calculated values
- Cause: Incorrect type defined for calculated values
- Fix: Use int instead of uint for all values

# Issue: Unable to use SPI1 config
- Cause: PA5 pin is already occupied by GPIO LED
- Fix: Use SPI2 and use different pin numbers

# Issue: No voltage in SD-card CS pin
- Cause: pin12 physical wiring is incorrect 
- Fix: Correct physical wiring (adjusted SD-card CS pin from pin12 to pin6)

# Issue: fmount error on start-up
- Cause: Incorrect SD-card physical wiring on STM32
- Fix: Correct physical wiring on STM32

# Issue: Main code duplicated when generating code from STM32CubeMX
- Cause: Incorrect placement of END WHILE statement in main code
- Fix: Adjusted position of END WHILE statement 

# Issue: Timer1 and Timer2 deadlock in while loop
- Cause: Timer1 and Timer2 values never incremented
- Fix: Increment Timer1 and Timer2

# Issue: RTC time death error
- Cause: RTC not enabled in ioc
- Fix: Enable RTC in ioc

# Issue: printf deadlock with SD-card LOG function
- Cause: Infinite loop when printf inside LOG function
- Fix: Remove printf in LOG function and complete check at start-up instead

# Issue: GetTime returns the same date
- Cause: Date and time re-initialize on start-up
- Fix: TBD using ESP32 or other methods

# Issue: Garbage values on SD-Card LOG file
- Cause: _write function not compatible with SD-Card Log function
- Fix: Create custom printf function (includes standard printf and SD-Card logging)

# Issue: No log messages written onto SD-Card LOG file
- Cause: File handle not opened before calling log
- Fix: Initialize SD-Card and open file as first step of main program

# Issue: ESP32 Webpage update latency longer than 1 second
- Cause: TBD
- Fix: TBD

# Issue: Unable to find webserver
- Cause: Web-browser refresh rate every 2 seconds
- Fix: TBD

# Issue: ESP32 unable to parse received data from STM32
- Cause: Buffer for data is too small
- Fix: Increase buffer size
