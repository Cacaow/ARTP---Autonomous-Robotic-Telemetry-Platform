#include <SDCard.h>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "fatfs.h"
#include "stm32f4xx_hal.h"
#include "util.h"

int enable_SDCard = 1;
FIL Fil_log;
int log_open = 0;

FIL Fil_csv;
int csv_open = 0;

int SDCard_init(void)
{
	if (enable_SDCard == 0) {
		return 0;
	}

	printf("Start FATFS System\n");
	FATFS FatFs;        // FAT file system object (required by FatFs)
	FRESULT FR_Status;  // Stores return status of FatFs functions
	FATFS *FS_Ptr;      // Pointer to file system information
	DWORD FreeClusters; // Stores number of free clusters in SD card
	uint32_t TotalSize, FreeSpace; // SD card size information
	char temp_buffer[512];

	//------------------[ STEP 1: Mount the SD Card ]--------------------
	// This step makes the SD card ready to use with FatFs
	FR_Status = f_mount(&FatFs, "", 1);
	if (FR_Status != FR_OK)
	{
		// If mounting fails, print error and exit function
		sprintf(temp_buffer, "[ERROR] SD Card mount failed! Code = %d\r\n", FR_Status);
		printf(temp_buffer);
		return -1;
	}

	// SD card mounted successfully
	printf("[INFO] SD Card mounted successfully\r\n\n");

	//------------------[ STEP 2: Get SD Card Size & Free Space ]--------------------
	// Get free cluster count and file system information
	f_getfree("", &FreeClusters, &FS_Ptr);

	// Calculate total size and free space in Bytes
	// csize = cluster size, each sector = 512 bytes
	TotalSize = (FS_Ptr->n_fatent - 2) * FS_Ptr->csize * 512;
	FreeSpace = FreeClusters * FS_Ptr->csize * 512;

	// Print SD card memory details on serial terminal
	sprintf(temp_buffer, "[INFO] SD Card Total Size : %lu Bytes\r\n", TotalSize);
	printf(temp_buffer);
	sprintf(temp_buffer, "[INFO] SD Card Free Space : %lu Bytes\r\n\n", FreeSpace);
	printf(temp_buffer);

	printf("SDCard Initialization process is done!\n");

	return 0;
}

int SDCard_close(void)
{
	if (enable_SDCard == 0) {
		return 0;
	}
	FRESULT FR_Status;  // Stores return status of FatFs functions
	char temp_buffer[512];

 //------------------[ STEP 7: Unmount the SD Card ]--------------------
	// Properly unmount the SD card to avoid data corruption
	FR_Status = f_mount(NULL, "", 0);
	if (FR_Status != FR_OK) {
		sprintf(temp_buffer, "[ERROR] SD Card unmount failed! Code = %d\r\n", FR_Status);
		printf(temp_buffer);
	} else {
		printf("[INFO] SD Card unmounted safely\r\n");
	}
	return 0;
}

int SDCard_write(void)
{
	if (enable_SDCard == 0) {
			return 0;
	}

	FIL Fil;            // File object (used to open/read/write files)
	FRESULT FR_Status;  // Stores return status of FatFs functions
	UINT WWC;      // RWC = Read byte count, WWC = Written byte count
	char RW_Buffer[200]; // Buffer used for both read and write operations

	//------------------[ STEP 3: Create File and Write Data ]--------------------
	// Create a new file or overwrite if it already exists
	FR_Status = f_open(&Fil,
					   "STM_FILE.txt",
					   FA_WRITE | FA_OPEN_APPEND);

	if (FR_Status != FR_OK)
	{
		// File creation/open failed
		printf("[ERROR] File create/open failed! Code = %d\r\n", FR_Status);
		return -1;
	}

	// File created successfully
	printf("[SD] File opened: STM_FILE.txt\r\n");

	// Prepare data to be written into the SD card file
	strcpy(RW_Buffer, "[SD] Hello from STM32! Data written using f_write()\r\n");

	// Write data to the file
	FR_Status = f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
	if (FR_Status != FR_OK || WWC == 0)
	{
		// Writing failed
		printf("[ERROR] SD file write failed\r\n");
		f_close(&Fil);
		return -1;
	}

	// Print number of bytes written
	printf("[INFO] Written string to SD card: %s\r\n", RW_Buffer);
	printf("[INFO] Written %d bytes to SD card\r\n", WWC);

	// Close the file after writing
	f_close(&Fil);

	return 0;
}

int SDCard_write_log(char *text)
{
	if (enable_SDCard == 0) {
			return 0;
	}

	FIL Fil;            // File object (used to open/read/write files)
	FRESULT FR_Status;  // Stores return status of FatFs functions
	UINT WWC;      // RWC = Read byte count, WWC = Written byte count
	char RW_Buffer[200]; // Buffer used for both read and write operations
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	extern RTC_HandleTypeDef hrtc;

	// 1. Get the current time first (Locks the shadow registers)
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

	// 2. Get the current date next (Unlocks the shadow registers)
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	uint8_t hours   = sTime.Hours;
	uint8_t minutes = sTime.Minutes;
	uint8_t seconds = sTime.Seconds;

	uint8_t day     = sDate.Date;
	uint8_t month   = sDate.Month;
	uint16_t year   = 2000 + sDate.Year; // RTC stores year as a 2-digit offset from 2000

	char Filename[256];
	Filename[0] = 0;
	sprintf(Filename, "ARTP_LOG_%04d-%02d-%02d.txt", year, month, day);

	//------------------[ STEP 3: Create File and Write Data ]--------------------
	// Create a new file or overwrite if it already exists
	FR_Status = f_open(&Fil,
					   Filename,
					   FA_WRITE | FA_OPEN_APPEND);

	if (FR_Status != FR_OK)
	{
		// File creation/open failed
		printf("[ERROR] File create/open failed! Code = %d\r\n", FR_Status);
		return -1;
	}

	// File created successfully
	printf("[SD] File opened: STM_FILE.txt\r\n");

	// Prepare data to be written into the SD card file
	char *result = strstr(text, "ERROR");
	if (result == NULL) {
		sprintf(RW_Buffer, "%04d-%02d-%02d %02d:%02d:%02d [DEBUG] %s", year, month, day, hours, minutes, seconds, text);
	} else {
		sprintf(RW_Buffer, "%04d-%02d-%02d %02d:%02d:%02d [ERROR] %s", year, month, day, hours, minutes, seconds, text);
	}
	//strcpy(RW_Buffer, text);

	// Write data to the file
	FR_Status = f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
	if (FR_Status != FR_OK || WWC == 0)
	{
		// Writing failed
		printf("[ERROR] SD file write failed\r\n");
		f_close(&Fil);
		return -1;
	}

	// Print number of bytes written
	printf("[INFO] Written string to SD card: %s\r\n", RW_Buffer);
	printf("[INFO] Written %d bytes to SD card\r\n", WWC);

	// Close the file after writing
	f_close(&Fil);

	return 0;
}

int SDCard_open_log()
{
	if (enable_SDCard == 0) {
			return 0;
	}

	FRESULT FR_Status;  // Stores return status of FatFs functions
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	extern RTC_HandleTypeDef hrtc;
	char temp_buffer[512];

	// 1. Get the current time first (Locks the shadow registers)
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

	// 2. Get the current date next (Unlocks the shadow registers)
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	uint8_t hours   = sTime.Hours;
	uint8_t minutes = sTime.Minutes;
	uint8_t seconds = sTime.Seconds;

	uint8_t day     = sDate.Date;
	uint8_t month   = sDate.Month;
	uint16_t year   = 2000 + sDate.Year; // RTC stores year as a 2-digit offset from 2000

	char Filename[256];
	Filename[0] = 0;
	sprintf(Filename, "ARTP_LOG_%04d-%02d-%02d.txt", year, month, day);

	//------------------[ STEP 3: Create File and Write Data ]--------------------
	// Create a new file or overwrite if it already exists
	FR_Status = f_open(&Fil_log,
					   Filename,
					   FA_WRITE | FA_OPEN_APPEND);

	if (FR_Status != FR_OK)
	{
		// File creation/open failed
		sprintf(temp_buffer, "[ERROR] File create/open failed! Code = %d\r\n", FR_Status);
		printf(temp_buffer);
		return -1;
	}

	// File created successfully
	printf("[SD] File opened: STM_FILE.txt\r\n");
	log_open = 1;

	return 0;
}

//CAN NOT USE my_printf WITHIN THIS FUNCTION
int SDCard_append_log(char *text)
{
	if (enable_SDCard == 0) {
			return 0;
	}
	if (!log_open) {
		printf("FILE INVALID\n");
		return -1;
	}

	FRESULT FR_Status;  // Stores return status of FatFs functions
	UINT WWC;      // RWC = Read byte count, WWC = Written byte count
	char RW_Buffer[200]; // Buffer used for both read and write operations
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	extern RTC_HandleTypeDef hrtc;

	// 1. Get the current time first (Locks the shadow registers)
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

	// 2. Get the current date next (Unlocks the shadow registers)
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	uint8_t hours   = sTime.Hours;
	uint8_t minutes = sTime.Minutes;
	uint8_t seconds = sTime.Seconds;

	uint8_t day     = sDate.Date;
	uint8_t month   = sDate.Month;
	uint16_t year   = 2000 + sDate.Year; // RTC stores year as a 2-digit offset from 2000

	// Prepare data to be written into the SD card file
	char *result = strstr(text, "ERROR");
	if (result == NULL) {
		sprintf(RW_Buffer, "%04d-%02d-%02d %02d:%02d:%02d [DEBUG] %s", year, month, day, hours, minutes, seconds, text);
	} else {
		sprintf(RW_Buffer, "%04d-%02d-%02d %02d:%02d:%02d [ERROR] %s", year, month, day, hours, minutes, seconds, text);
	}

	// Write data to the file
	FR_Status = f_write(&Fil_log, RW_Buffer, strlen(RW_Buffer), &WWC);
	if (FR_Status != FR_OK || WWC == 0)
	{
		// Writing failed
		printf("[ERROR] SD file write failed\r\n");
		f_close(&Fil_log);
		log_open = 0;
		return -1;
	}

	// Print number of bytes written
	printf("[INFO] Written string to SD card: %s\r\n", RW_Buffer);
	printf("[INFO] Written %d bytes to SD card\r\n", WWC);

	return 0;
}

int SDCard_close_log()
{
	if (enable_SDCard == 0) {
			return 0;
	}
	if (!log_open) {
		printf("FILE INVALID\n");
		return -1;
	}

	// Close the file after writing
	f_close(&Fil_log);
	log_open = 0;

	return 0;
}


int SDCard_write_csv(char *text)
{
	if (enable_SDCard == 0) {
			return 0;
	}

	FIL Fil;            // File object (used to open/read/write files)
	FRESULT FR_Status;  // Stores return status of FatFs functions
	UINT WWC;      // RWC = Read byte count, WWC = Written byte count
	char RW_Buffer[200]; // Buffer used for both read and write operations
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	extern RTC_HandleTypeDef hrtc;
	char temp_buffer[512];

	// 1. Get the current time first (Locks the shadow registers)
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

	// 2. Get the current date next (Unlocks the shadow registers)
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	uint8_t hours   = sTime.Hours;
	uint8_t minutes = sTime.Minutes;
	uint8_t seconds = sTime.Seconds;

	uint8_t day     = sDate.Date;
	uint8_t month   = sDate.Month;
	uint16_t year   = 2000 + sDate.Year; // RTC stores year as a 2-digit offset from 2000

	char Filename[256];
	Filename[0] = 0;
	sprintf(Filename, "ARTP_SENSOR_DATA_%04d-%02d-%02d.csv", year, month, day);

	int writeHeader = 0;
	FILINFO fno;
	FRESULT res = f_stat(Filename, &fno);

	if (res == FR_OK) {
		my_printf("File exists.\n");
	} else {
		my_printf("File does not exist.\n");
		writeHeader = 1;
	}

	//------------------[ STEP 3: Create File and Write Data ]--------------------
	// Create a new file or overwrite if it already exists
	FR_Status = f_open(&Fil,
					   Filename,
					   FA_WRITE | FA_OPEN_APPEND);

	if (FR_Status != FR_OK)
	{
		// File creation/open failed
		sprintf(temp_buffer, "[ERROR] File create/open failed! Code = %d\r\n", FR_Status);
		my_printf(temp_buffer);
		return -1;
	}

	// File created successfully
	my_printf("[SD] CSV File opened: STM_FILE.csv\r\n");

	if (writeHeader) {
		sprintf(RW_Buffer, "DATE, TIME, Temperature, Pressure, Humidity, Alititude, Ax, Ay, Az, Gx, Gy, Gz, roll, pitch, KalmanX, KalmanY, state\n");
		FR_Status = f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
		if (FR_Status != FR_OK || WWC == 0)
		{
			// Writing failed
			my_printf("[ERROR] SD file write failed\r\n");
			f_close(&Fil);
			return -1;
		}
	}

	// Prepare data to be written into the SD card file
	sprintf(RW_Buffer, "%04d-%02d-%02d, %02d:%02d:%02d, %s\n", year, month, day, hours, minutes, seconds, text);

	//strcpy(RW_Buffer, text);

	// Write data to the file
	FR_Status = f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
	if (FR_Status != FR_OK || WWC == 0)
	{
		// Writing failed
		my_printf("[ERROR] SD file write failed\r\n");
		f_close(&Fil);
		return -1;
	}
	// Print number of bytes written
	sprintf(temp_buffer, "[INFO] Written string to SD card: %s\r\n", RW_Buffer);
	my_printf(temp_buffer);
	sprintf(temp_buffer, "[INFO] Written %d bytes to SD card\r\n", WWC);
	my_printf(temp_buffer);

	// Close the file after writing
	f_close(&Fil);

	return 0;
}

int SDCard_open_csv()
{
	if (enable_SDCard == 0) {
			return 0;
	}

	FRESULT FR_Status;  // Stores return status of FatFs functions
	UINT WWC;      // RWC = Read byte count, WWC = Written byte count
	char RW_Buffer[200]; // Buffer used for both read and write operations
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	extern RTC_HandleTypeDef hrtc;
	char temp_buffer[512];

	// 1. Get the current time first (Locks the shadow registers)
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

	// 2. Get the current date next (Unlocks the shadow registers)
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	uint8_t hours   = sTime.Hours;
	uint8_t minutes = sTime.Minutes;
	uint8_t seconds = sTime.Seconds;

	uint8_t day     = sDate.Date;
	uint8_t month   = sDate.Month;
	uint16_t year   = 2000 + sDate.Year; // RTC stores year as a 2-digit offset from 2000

	char Filename[256];
	Filename[0] = 0;
	sprintf(Filename, "ARTP_SENSOR_DATA_%04d-%02d-%02d.csv", year, month, day);

	int writeHeader = 0;
	FILINFO fno;
	FRESULT res = f_stat(Filename, &fno);

	if (res == FR_OK) {
		my_printf("File exists.\n");
	} else {
		my_printf("File does not exist.\n");
		writeHeader = 1;
	}

	//------------------[ STEP 3: Create File and Write Data ]--------------------
	// Create a new file or overwrite if it already exists
	FR_Status = f_open(&Fil_csv,
					   Filename,
					   FA_WRITE | FA_OPEN_APPEND);

	if (FR_Status != FR_OK)
	{
		// File creation/open failed
		sprintf(temp_buffer, "[ERROR] File create/open failed! Code = %d\r\n", FR_Status);
		my_printf(temp_buffer);
		return -1;
	}

	// File created successfully
	my_printf("[SD] CSV File opened: STM_FILE.csv\r\n");
	csv_open = 1;

	if (writeHeader) {
		sprintf(RW_Buffer, "DATE, TIME, Temperature, Pressure, Humidity, Alititude, Ax, Ay, Az, Gx, Gy, Gz, roll, pitch, KalmanX, KalmanY, state\n");
		FR_Status = f_write(&Fil_csv, RW_Buffer, strlen(RW_Buffer), &WWC);
		if (FR_Status != FR_OK || WWC == 0)
		{
			// Writing failed
			my_printf("[ERROR] SD file write failed\r\n");
			f_close(&Fil_csv);
			csv_open = 0;
			return -1;
		}
	}

	return 0;
}

int SDCard_append_csv(char *text)
{
	if (enable_SDCard == 0) {
			return 0;
	}
	if (!csv_open) {
		my_printf("INVALID CSV FILE\n");
		return -1;
	}

	FRESULT FR_Status;  // Stores return status of FatFs functions
	UINT WWC;      // RWC = Read byte count, WWC = Written byte count
	char RW_Buffer[200]; // Buffer used for both read and write operations
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	extern RTC_HandleTypeDef hrtc;
	char temp_buffer[512];

	// 1. Get the current time first (Locks the shadow registers)
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

	// 2. Get the current date next (Unlocks the shadow registers)
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	uint8_t hours   = sTime.Hours;
	uint8_t minutes = sTime.Minutes;
	uint8_t seconds = sTime.Seconds;

	uint8_t day     = sDate.Date;
	uint8_t month   = sDate.Month;
	uint16_t year   = 2000 + sDate.Year; // RTC stores year as a 2-digit offset from 2000

	// Prepare data to be written into the SD card file
	sprintf(RW_Buffer, "%04d-%02d-%02d, %02d:%02d:%02d, %s\n", year, month, day, hours, minutes, seconds, text);

	//strcpy(RW_Buffer, text);

	// Write data to the file
	FR_Status = f_write(&Fil_csv, RW_Buffer, strlen(RW_Buffer), &WWC);
	if (FR_Status != FR_OK || WWC == 0)
	{
		// Writing failed
		my_printf("[ERROR] SD file write failed\r\n");
		f_close(&Fil_csv);
		csv_open = 0;
		return -1;
	}
	// Print number of bytes written
	sprintf(temp_buffer, "[INFO] Written string to SD card: %s\r\n", RW_Buffer);
	my_printf(temp_buffer);
	sprintf(temp_buffer, "[INFO] Written %d bytes to SD card\r\n", WWC);
	my_printf(temp_buffer);

	return 0;
}

int SDCard_close_csv()
{
	if (enable_SDCard == 0) {
			return 0;
	}
	if (!csv_open) {
		my_printf("INVALID CSV FILE\n");
		return -1;
	}

	// Close the file after writing
	f_close(&Fil_csv);
	csv_open = 0;

	return 0;
}


int SDCard_read(void)
{
	if (enable_SDCard == 0) {
			return 0;
	}

	FIL Fil;            // File object (used to open/read/write files)
	FRESULT FR_Status;  // Stores return status of FatFs functions
	UINT RWC, WWC;      // RWC = Read byte count, WWC = Written byte count
	char RW_Buffer[200]; // Buffer used for both read and write operations
	char temp_buffer[512];

	//------------------[ STEP 4: Read File Content ]--------------------
	// Open the same file in read mode
	FR_Status = f_open(&Fil, "STM_FILE.txt", FA_READ);
	if (FR_Status != FR_OK)
	{
		my_printf("[ERROR] File open for read failed\r\n");
		return -1;
	}

	// Clear buffer before reading data
	memset(RW_Buffer, 0, sizeof(RW_Buffer));

	// Move file pointer to the beginning of the file
	f_lseek(&Fil, 0);

	// Read data from SD card file into buffer
	FR_Status = f_read(&Fil, RW_Buffer, sizeof(RW_Buffer) - 1, &RWC);
	if (FR_Status == FR_OK)
	{
		// Add null character to make it a valid string
		RW_Buffer[RWC] = '\0';

		// Print file content on serial terminal
		sprintf(temp_buffer, "[SD] File content read from SD card:\r\n%s\r\n", RW_Buffer);
		my_printf(temp_buffer);
	}

	// Close file after reading
	f_close(&Fil);
	return 0;
}

void SD_Card_Test(void)
{
	my_printf("Start FATFS System\n");
    FATFS FatFs;        // FAT file system object (required by FatFs)
    FIL Fil;            // File object (used to open/read/write files)
    FRESULT FR_Status;  // Stores return status of FatFs functions
    FATFS *FS_Ptr;      // Pointer to file system information
    UINT RWC, WWC;      // RWC = Read byte count, WWC = Written byte count
    DWORD FreeClusters; // Stores number of free clusters in SD card
    uint32_t TotalSize, FreeSpace; // SD card size information
    char RW_Buffer[200]; // Buffer used for both read and write operations

    //------------------[ STEP 1: Mount the SD Card ]--------------------
	// This step makes the SD card ready to use with FatFs
	FR_Status = f_mount(&FatFs, "", 1);
	if (FR_Status != FR_OK)
	{
		// If mounting fails, print error and exit function
		printf("[ERROR] SD Card mount failed! Code = %d\r\n", FR_Status);
		return;
	}

	// SD card mounted successfully
	printf("[INFO] SD Card mounted successfully\r\n\n");

	//------------------[ STEP 2: Get SD Card Size & Free Space ]--------------------
	// Get free cluster count and file system information
	f_getfree("", &FreeClusters, &FS_Ptr);

	// Calculate total size and free space in Bytes
	// csize = cluster size, each sector = 512 bytes
	TotalSize = (FS_Ptr->n_fatent - 2) * FS_Ptr->csize * 512;
	FreeSpace = FreeClusters * FS_Ptr->csize * 512;

	// Print SD card memory details on serial terminal
	printf("[INFO] SD Card Total Size : %lu Bytes\r\n", TotalSize);
	printf("[INFO] SD Card Free Space : %lu Bytes\r\n\n", FreeSpace);

    //------------------[ STEP 3: Create File and Write Data ]--------------------
    // Create a new file or overwrite if it already exists
    FR_Status = f_open(&Fil,
                       "STM_FILE.txt",
                       FA_WRITE | FA_READ | FA_CREATE_ALWAYS);

    if (FR_Status != FR_OK)
    {
        // File creation/open failed
        printf("[ERROR] File create/open failed! Code = %d\r\n", FR_Status);
        return;
    }

    // File created successfully
    printf("[SD] File created: STM_FILE.txt\r\n");

    // Prepare data to be written into the SD card file
    strcpy(RW_Buffer, "[SD] Hello from STM32! Data written using f_write()\r\n");

    // Write data to the file
    FR_Status = f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
    if (FR_Status != FR_OK || WWC == 0)
    {
        // Writing failed
        printf("[ERROR] SD file write failed\r\n");
        f_close(&Fil);
        return;
    }

    // Print number of bytes written
    printf("[INFO] Written string to SD card: %s\r\n", RW_Buffer);
    printf("[INFO] Written %d bytes to SD card\r\n", WWC);

    // Close the file after writing
    f_close(&Fil);

    //------------------[ STEP 4: Read File Content ]--------------------
    // Open the same file in read mode
    FR_Status = f_open(&Fil, "STM_FILE.txt", FA_READ);
    if (FR_Status != FR_OK)
    {
        printf("[ERROR] File open for read failed\r\n");
        return;
    }

    // Clear buffer before reading data
    memset(RW_Buffer, 0, sizeof(RW_Buffer));

    // Move file pointer to the beginning of the file
    f_lseek(&Fil, 0);

    // Read data from SD card file into buffer
    FR_Status = f_read(&Fil, RW_Buffer, sizeof(RW_Buffer) - 1, &RWC);
    if (FR_Status == FR_OK)
    {
        // Add null character to make it a valid string
        RW_Buffer[RWC] = '\0';

        // Print file content on serial terminal
        printf("[SD] File content read from SD card:\r\n%s\r\n", RW_Buffer);
    }

    // Close file after reading
    f_close(&Fil);

    //------------------[ STEP 5: Append New Data to File ]--------------------
    // Open existing file in write mode (for updating)
    FR_Status = f_open(&Fil,
                       "STM_FILE.txt",
                       FA_OPEN_EXISTING | FA_WRITE);
    if (FR_Status != FR_OK)
    {
        printf("[ERROR] File open for update failed\r\n");
        return;
    }

    // Move file pointer to the end of the file
    // This ensures new data is appended, not overwritten
    f_lseek(&Fil, f_size(&Fil));

    // New data to be appended at the end of file
    strcpy(RW_Buffer,
           "[SD] This line was appended later\r\n");

    // Write appended data
    f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
    printf("[SD] Appended new data to file\r\n");

    // Close file after updating
    f_close(&Fil);

    //------------------[ STEP 6: Read File After Update ]--------------------
    // Open file again to verify updated content
    FR_Status = f_open(&Fil, "STM_FILE.txt", FA_READ);
    if (FR_Status == FR_OK)
    {
        memset(RW_Buffer, 0, sizeof(RW_Buffer));

        // Read entire file content
        f_read(&Fil, RW_Buffer, sizeof(RW_Buffer) - 1, &RWC);
        RW_Buffer[RWC] = '\0';

        // Print updated file content
        printf("[SD] File content after update:\r\n%s\r\n", RW_Buffer);

        f_close(&Fil);
    }

    //------------------[ STEP 7: Unmount the SD Card ]--------------------
    // Properly unmount the SD card to avoid data corruption
    FR_Status = f_mount(NULL, "", 0);
    if (FR_Status != FR_OK)
        printf("[ERROR] SD Card unmount failed! Code = %d\r\n", FR_Status);
    else
        printf("[INFO] SD Card unmounted safely\r\n");
}
