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
int SDCard_open_log();
int SDCard_append_log(char *text);
int SDCard_close_log();
int SDCard_write_csv(char *text);
int SDCard_open_csv();
int SDCard_append_csv(char *text);
int SDCard_close_csv();
int SDCard_read(void);
void SD_Card_Test(void);

#endif /* INC_SDCARD_H_ */
