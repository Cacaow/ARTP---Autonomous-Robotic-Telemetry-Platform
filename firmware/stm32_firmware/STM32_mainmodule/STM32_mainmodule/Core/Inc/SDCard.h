#ifndef __SDCARD_H__
#define __SDCARD_H__

#include "stm32f4xx_hal.h"

#include <stddef.h>
#include <stdint.h>
#include <_ansi.h>

//Functions

int SDCard_init(void);
int SDCard_close(void);
int SDCard_write(void);
int SDCard_write_log(char *text);
int SDCard_read(void);
void SD_Card_Test(void);

#endif /* INC_SDCARD_H_ */
