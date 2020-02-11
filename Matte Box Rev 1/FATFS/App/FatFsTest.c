///////////////////////////////////////////////////////////////////////////////
/** @file "FatFsTest.c"
*   @brief Test interface hooks for the filesystem

-----------------------------------------------------------

@b Details

A detailed description goes here (if needed)


@b Target
- STM32F411

@b Author(s)
- Colton Crandell

Copyright &copy; 2020: Light Widow LLC

-----------------------------------------------------------

<b>Revision History:</b>
- 1.0:  1/23/20  Original

- last revision ($Date$)

-----------------------------------------------------------

<b>Attention</b>


*/
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
//-----------------------------  Includes  ------------------------------------
//=============================================================================
#include "stdint.h"
#include "stdbool.h"

#include "fatfs.h"
#include "ff.h"

//=============================================================================
//----------------------------  Constants  ------------------------------------
//=============================================================================


//=============================================================================
//------------------------  Modular Variables  --------------------------------
//=============================================================================


//=============================================================================
//------------------------  Private Functions  --------------------------------
//=============================================================================


//=============================================================================
//------------------------  Public Functions  ---------------------------------
//=============================================================================
SD_error FatFsInitRunner(void)
{
	SD_error retVal = FatFsInit();
	return retVal;
}

SD_error FatFsFormatSdRunner(void)
{
	SD_error retVal = FatFsReMake();
	return retVal;
}

SD_error FatFsCreateRunner(void) {
	FIL myFile;
	FRESULT fr;

	fr = f_open(&myFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE);
	if (fr == FR_OK) {
		fr = f_close(&myFile);
	}
	if(fr != FR_OK) {
		return SD_ERR;
	}

	return SD_OK;
}

SD_error FatFsCwdRunner(void) {
	FRESULT fr;
	uint8_t str[256] = {0};

	fr = f_getcwd(str, 255);
	if (fr == FR_OK) {
		return SD_OK;
	} else {
		return SD_ERR;
	}
}

SD_error FatFsMakeDirRunner(void) {
	FRESULT fr;
	uint8_t str[256] = {"LIGHT_WIDOW"};

	fr = f_mkdir((const TCHAR*)str);
	if (FR_OK == fr) {
		return SD_OK;
	} else {
		return SD_ERR;
	}
}

SD_error FatFsDirRunner(void)
{
	FRESULT fr;
	FILINFO fno = {};
	DIR cwd = {};
	char dirPath[256] = {};

	fr = f_getcwd(dirPath, 255);
	if (fr == FR_OK) {
		fr = f_opendir(&cwd, dirPath);
		if (fr == FR_OK) {
			do {
				fr = f_readdir(&cwd, &fno);
				if (fr == FR_OK) {
					if (fno.fname[0] != '\0') {
						TCHAR tmpName = fno.fname;
					}
				} else {
					return SD_ERR;
				}
			} while(fno.fname[0] != '\0');
		} else {
			return SD_ERR;
		}
		f_closedir(&cwd);
	} else {
		return SD_ERR;
	}

	return SD_OK;
}

SD_error FatFsCdRunner(void) {
	FRESULT fr;
	char str[256] = {"LIGHT_WIDOW"};

	fr = f_chdir(str);
	if (fr != FR_OK) {
		return SD_ERR;
	} else {
		fr = f_getcwd(str, 255);
		if (FR_OK == fr) {
			return SD_OK;
		} else {
			return SD_ERR;
		}
	}
}

SD_error FatFsWriteRunner(void) {
	FRESULT fr;
	FIL myFile = {};
	TCHAR testStr[256] = {"This is a test string to be written to the file.\n"};

	// Create the Test File
	SD_error res = FatFsCreateRunner();

	if (res != SD_OK) {
		return res;
	} else {
		// Open the file
		fr = f_open(&myFile, "STM32.TXT", FA_OPEN_APPEND | FA_WRITE);
		if (fr != FR_OK) {
			// Failed to open the file
			return SD_ERR;
		} else {
			// Write a string to the file
			uint8_t ret = f_puts(testStr, &myFile);
			f_puts(testStr, &myFile);
			if (ret < 0) {
				// Failed to write to the file
				return SD_ERR;
			} else {
				// Close the file
				f_close(&myFile);
				return SD_OK;
			}
		}
	}
}


//SD_error FatFsUnlinkRunner(void)
//{
//	FRESULT res;
//	char str[256] = {};
//
//	if ((res = f_chdir("/")) != FR_OK)
//	{
//		return SD_ERR;
//	}
//
//	if ((retVal = FatFsRecursiveDelete(str)).code == success)
//	{
//		return SD_OK;
//	}
//
//	return SD_ERR;
//}

//uint8_t FatFsChDrvRunner(void)
//{
//	error_t retVal = {};
//	retVal.code = failure;
//	FRESULT fr;
//	char str[256];
//	DbgExtract(str, dbg_tkn_d, suppliedPackage);
//
//	dbgPrint("\r\n");
//
//	if ((fr = f_chdrive(str)) == FR_OK)
//	{
//		dbgPrint("->OK\r\n");
//		retVal.code = success;
//	}
//	dbgPrint("->Error\r\n");
//
//	return retVal;
//} // Function FatFsChDrvRunner
//

