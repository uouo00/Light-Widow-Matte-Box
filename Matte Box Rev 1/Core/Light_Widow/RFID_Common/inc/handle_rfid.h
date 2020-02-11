/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HANDLE_RFID_H_
#define HANDLE_RFID_H_

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */

#include <rfal_analogConfig.h>
#include <rfal_isoDep.h>
#include <rfal_nfca.h>
#include <rfal_nfcb.h>
#include <rfal_nfcDep.h>
#include <rfal_nfcf.h>
#include <rfal_nfcv.h>
#include "st25r3916/st25r3916_com.h"

#include <stdint.h>
#include <stdbool.h>
#include "st_errno.h"
#include "utils.h"
#include "platform.h"

/********************************************************************************
 * DEFINES
 ******************************************************************************
 */
#define RFAL_POLLER_DEVICES      3    							/* Number of devices supported */
#define RFAL_POLLER_RF_BUF_LEN   255   							/* RF buffer length            */
#define RFAL_POLLER_FOUND_NONE   0x00  							/* No device found Flag        */
#define RFAL_POLLER_FOUND_A      0x01  							/* NFC-A device found Flag     */
#define RFAL_POLLER_FOUND_B      0x02  							/* NFC-B device found Flag     */
#define RFAL_POLLER_FOUND_F      0x04  							/* NFC-F device found Flag     */
#define RFAL_POLLER_FOUND_V      0x08  							/* NFC-V device Flag           */

/********************************************************************************
 * TYPES
 ******************************************************************************
 */
typedef enum{    RFAL_POLLER_STATE_INIT                			=  0,    	/* Initialize state            */
				 RFAL_POLLER_STATE_TECHDETECT          			=  1,    	/* Technology Detection state  */
				 RFAL_POLLER_STATE_COLAVOIDANCE        			=  2,    	/* Collision Avoidance state   */
				 RFAL_POLLER_STATE_ACTIVATION  					=  3,  		/* Activation state            */
				 RFAL_POLLER_STATE_DATAEXCHANGE_START  			=  4,  		/* Data Exchange Start state   */
				 RFAL_POLLER_STATE_DATAEXCHANGE_CHECK  			=  5,  		/* Data Exchange Check state   */
				 RFAL_POLLER_STATE_DEACTIVATION        			=  9     	/* Deactivation state          */
}RfalPollerState;

typedef enum{    RFAL_POLLER_TYPE_NFCA  =  0,                   /* NFC-A device type           */
			 	 RFAL_POLLER_TYPE_NFCB  =  1,                   /* NFC-B device type           */
				 RFAL_POLLER_TYPE_NFCF  =  2,                   /* NFC-F device type           */
				 RFAL_POLLER_TYPE_NFCV  =  3                    /* NFC-V device type           */
}RfalPollerDevType;

typedef enum{    RFAL_POLLER_INTERFACE_RF     = 0,              /* RF Frame interface          */
				 RFAL_POLLER_INTERFACE_ISODEP = 1,              /* ISO-DEP interface           */
				 RFAL_POLLER_INTERFACE_NFCDEP = 2               /* NFC-DEP interface           */
}RfalPollerRfInterface;

typedef struct{    RfalPollerDevType type;               /* Device's type                */
				   union{
     				   rfalNfcaListenDevice nfca;               /* NFC-A Listen Device instance */
				   	   rfalNfcbListenDevice nfcb;               /* NFC-B Listen Device instance */
				   	   rfalNfcfListenDevice nfcf;               /* NFC-F Listen Device instance */
				   	   rfalNfcvListenDevice nfcv;               /* NFC-V Listen Device instance */
				   }dev;                                        /* Device's instance            */

				   RfalPollerRfInterface rfInterface;    /* Device's interface           */
				   union{
					   rfalIsoDepDevice isoDep;              	/* ISO-DEP instance             */
				   	   rfalNfcDepDevice nfcDep;                 /* NFC-DEP instance             */
				   }proto;                                      /* Device's protocol            */
}RfalPollerDevice;

union {
	uint8_t                 rfRxBuf[RFAL_POLLER_RF_BUF_LEN];    /* RF Rx buffer                                    */
	rfalIsoDepBufFormat     isoDepRxBuf;                        /* ISO-DEP Rx buffer format (with header/prologue) */
	rfalNfcDepBufFormat     nfcDepRxBuf;                        /* NFC-DEP Rx buffer format (with header/prologue) */
} gRxBuf;

typedef struct {												/* Global UID Storage                              */
	uint8_t tagCount;
	uint8_t TagUID[RFAL_POLLER_DEVICES][RFAL_NFCV_UID_LEN];
} detectedTags_t;

/********************************************************************************
 * PROTOTYPES
 ******************************************************************************
 */
void rfidInit(void);
void powerOffRFID( void );
void checkForTags(detectedTags_t *dTags);

#endif // HANDLE_RFID_H_
