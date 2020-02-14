/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */

uint8_t workBuffer[_MAX_SS]; // __attribute__((section (".RAM_D1")));
static bool fatFsModInit = false;

/* USER CODE END Variables */    

void MX_FATFS_Init(void) 
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */     
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */  
}

/* USER CODE BEGIN Application */
     
SD_error FatFsInit(void) {
	// Initializing Fat Filesystem
	if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0) {
		FRESULT res;
		/* Attempt to Mount SD Drive */
		 if(f_mount(&SDFatFS, (TCHAR const*)SDPath, 1) != FR_OK) {
			 /* Failed to mount drive */
			 res = f_mkfs(SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer));
			 if (res != FR_OK) {
				 /* Error, failed to initialize filesystem */
				 return SD_ERR;
			 }
		 }
		 fatFsModInit = true;
	} else {
		// Failed to link SD Driver
		return SD_ERR;
	}
	return SD_OK;
}

SD_error FatFsDeInit(void) {
	fatFsModInit = false;
	return FATFS_UnLinkDriver(SDPath);
}


SD_error FatFsReMake(void) {
	FRESULT res;

	// Remaking Filesystem
	res = f_mkfs(SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer));
	if (res != FR_OK) {
		/* Error, failed to initialize filesystem */
		return SD_ERR;
	}

	// Success - Remounting
	if(f_mount(&SDFatFS, (TCHAR const*)SDPath, 1) != FR_OK) {
		/* Error, failed to mount drive */
		return SD_ERR;
	}

	// Success
	return SD_OK;
}

bool FatFsIsModuleInit(void) {
	return fatFsModInit;
}



/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
