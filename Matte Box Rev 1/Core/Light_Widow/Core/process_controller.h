/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PROCESS_CONTROLLER_H_
#define PROCESS_CONTROLLER_H_


/********************************************************************************
 * INCLUDES
 *******************************************************************************/

/********************************************************************************
 * DEFINES
 *******************************************************************************/
#define FILTER_SECTION_SIZE		3			/* Number of filter slots installed		*/
#define FILTER_NAME_LEN			10			/* String Length for Filter Names		*/

/********************************************************************************
 * TYPES
 *******************************************************************************/

/********************************************************************************
 * PROTOTYPES
 *******************************************************************************/
// Main setup for the program.
void setupIOs(void);

// Main operation state
void processIOs(void);


#endif // PROCESS_CONTROLLER_H_
