/*  \file handle_rfid.c
 *  \ handle_rfid.c
 *  \author Colton Crandell
 *  \brief  NFC Poller/Reader device (PCD)
 *
 *
 */

#include "handle_rfid.h"
#include <string.h>

/* ******************************************************************************
 * LOCAL VARIABLES
 * ******************************************************************************
 */

static uint8_t    		gDevCnt;								 /* Number of Devices Present    					*/
static RfalPollerDevice gDevList[RFAL_POLLER_DEVICES];           /* Device List                  					*/
static RfalPollerState  gState;                                  /* Main state                                      */
static uint8_t          gTechsFound;                             /* Technologies found bitmask                      */
RfalPollerDevice        *gActiveDev;                             /* Active device pointer                           */

/*******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 ******************************************************************************
 */
static bool rfalPollerTechDetetection( void );
static bool rfalPollerCollResolution( void );
static bool copyUIDs(dTags_t *dTags);

/*******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************
 */

void rfidInit(void) {
	rfalAnalogConfigInitialize();                                                     /* Initialize RFAL's Analog Configs */
	rfalInitialize();                                                                 /* Initialize RFAL */
	gState = RFAL_POLLER_STATE_INIT;
}

void powerOffRFID( void ) {
	// Turn off tx_en
	st25r3916ClrRegisterBits( 0x02, (1 << 3));
	// Turn off oscillator and regulator
	st25r3916ClrRegisterBits( 0x02, (1 << 7));
}

void checkForTags(dTags_t *dTags) {
	ReturnCode err;

	do {
		rfalWorker();                                                                 /* Execute RFAL process */
		switch( gState )
		{
			/*******************************************************************************/
			case RFAL_POLLER_STATE_INIT:
				gTechsFound = RFAL_POLLER_FOUND_NONE;
				gActiveDev  = NULL;
				gDevCnt     = 0;
				dTags->tagCount = 0;
				gState = RFAL_POLLER_STATE_TECHDETECT;
				break;

			/*******************************************************************************/
			case RFAL_POLLER_STATE_TECHDETECT:
				if( !rfalPollerTechDetetection() )                             	/* Poll for nearby devices	*/
				{
					gState = RFAL_POLLER_STATE_DEACTIVATION;                  	/* If no device was found, restart loop */
					break;
				}
				gState = RFAL_POLLER_STATE_COLAVOIDANCE;                      	/* One or more devices found, go to Collision Avoidance */
				break;

			/*******************************************************************************/
			case RFAL_POLLER_STATE_COLAVOIDANCE:
				if( !rfalPollerCollResolution() )                              	/* Resolve any eventual collision 	*/
				{
					gState = RFAL_POLLER_STATE_DEACTIVATION;                  	/* If Collision Resolution was unable to retrieve any device, restart loop */
					break;
				}

				dTags->tagCount = gDevCnt;

				copyUIDs(dTags);												/* The Collision Resolution has identified devices. Copy UIDs to structure */

				gState = RFAL_POLLER_STATE_DEACTIVATION;                      	/* Device(s) have been Loaded into UID Structure, power off field */
				break;
/////////
            /*******************************************************************************/
			case RFAL_POLLER_STATE_ACTIVATION:
//				if( !exampleRfalPollerActivation(0) ){                    		/* Any device previous identified can be Activated, on this example will select the first on the list */
//					gState = RFAL_POLLER_STATE_DEACTIVATION;            		/* If Activation failed, restart loop */
//					break;
//				}
				gState = RFAL_POLLER_STATE_DATAEXCHANGE_START;          		/* Device has been properly activated, go to Data Exchange */
				break;

			/*******************************************************************************/
			case RFAL_POLLER_STATE_DATAEXCHANGE_START:
			case RFAL_POLLER_STATE_DATAEXCHANGE_CHECK:
//				err = exampleRfalPollerDataExchange();                          /* Perform Data Exchange, in this example a simple transfer will executed in order to do device's presence check */
				switch(err){
					case ERR_NONE:                                              /* Data exchange successful  */
						platformDelay(300);                                     /* Wait a bit */
						gState = RFAL_POLLER_STATE_DATAEXCHANGE_START;        	/* Trigger new exchange with device */
						break;

					case ERR_BUSY:                                              /* Data exchange ongoing  */
						gState = RFAL_POLLER_STATE_DATAEXCHANGE_CHECK;        	/* Once triggered/started the Data Exchange only do check until is completed */
						break;

					default:                                                    /* Data exchange not successful, card removed or other transmission error */
						gState = RFAL_POLLER_STATE_DEACTIVATION;              	/* Restart loop */
						break;
				}
				break;
//////////
			/*******************************************************************************/
			case RFAL_POLLER_STATE_DEACTIVATION:
				rfalFieldOff();                                               	/* Turn the Field Off powering down any device nearby */
				gState = RFAL_POLLER_STATE_INIT;                              	/* Restart the loop */
				break;

			/*******************************************************************************/
			default:
				return;
		}
	} while (gState != RFAL_POLLER_STATE_INIT);								  	/* Only run for one full activation cycle */
}



static bool rfalPollerTechDetetection( void )
{
	ReturnCode           err;
	rfalNfcvInventoryRes invRes;
	gTechsFound = RFAL_POLLER_FOUND_NONE;

	/*******************************************************************************/
	/* NFC-V Technology Detection                                                  */
	/*******************************************************************************/
	rfalNfcvPollerInitialize();                                                       /* Initialize RFAL for NFC-V */
	rfalFieldOnAndStartGT();                                                          /* As field is already On only starts GT timer */
	err = rfalNfcvPollerCheckPresence( &invRes );                                     /* Poll for NFC-V devices */
	if( err == ERR_NONE )
	{
		gTechsFound |= RFAL_POLLER_FOUND_V;
	}
	return (gTechsFound != RFAL_POLLER_FOUND_NONE);
}

static bool rfalPollerCollResolution( void )
{
	uint8_t    i;
	uint8_t devCnt;
	ReturnCode err;

	/*******************************************************************************/
	/* NFC-V Collision Resolution                                                  */
	/*******************************************************************************/
	if( gTechsFound & RFAL_POLLER_FOUND_V )                                   /* If a NFC-F device was found/detected, perform Collision Resolution */
	{
		rfalNfcvListenDevice nfcvDevList[RFAL_POLLER_DEVICES];
		rfalNfcvPollerInitialize();
		rfalFieldOnAndStartGT();                                                      /* Ensure GT again as other technologies have also been polled */
		err = rfalNfcvPollerCollisionResolution( RFAL_COMPLIANCE_MODE_NFC, (RFAL_POLLER_DEVICES - gDevCnt), nfcvDevList, &devCnt );
		if( (err == ERR_NONE) && (devCnt != 0) )
		{
			for( i=0; i<devCnt; i++ )                                                /* Copy devices found form local Nfcf list into global device list */
			{
				gDevList[gDevCnt].type     = RFAL_POLLER_TYPE_NFCV;
				gDevList[gDevCnt].dev.nfcv = nfcvDevList[i];
				gDevCnt++;
			}
		}
	}

	return (gDevCnt > 0);
}


static bool copyUIDs(dTags_t *dTags) {
	for (uint8_t devCnt = 0; devCnt < gDevCnt; devCnt++) {
		memcpy(dTags->TagUID[devCnt], gDevList[devCnt].dev.nfcv.InvRes.UID, RFAL_NFCV_UID_LEN);
	}
}



