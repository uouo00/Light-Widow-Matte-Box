/*
 * FatFsTest.h
 *
 *  Created on: 1/23/20
 *      Author: Colton Crandell
 */

#ifndef FATFS_FATFSTEST_H_
#define FATFS_FATFSTEST_H_

SD_error FatFsInitRunner(void);
SD_error FatFsFormatSdRunner(void);
SD_error FatFsCreateRunner(void);
SD_error FatFsCwdRunner(void);
SD_error FatFsMakeDirRunner(void);
SD_error FatFsDirRunner(void);
SD_error FatFsCdRunner(void);
SD_error FatFsWriteRunner(void);
//SD_error FatFsUnlinkRunner(void);
//uint8_t FatFsChDrvRunner(void);

#endif /* FATFS_FATFSTEST_H_ */
