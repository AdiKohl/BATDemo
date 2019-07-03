/*
 * DriveSwitch.c
 *
 *  Created on: 02.07.2019
 *      Author: mail
 */

#include "DriveSwitch.h"

#if DS_CONFIG_HAS_SHELL
  #include "CLS1.h"
#endif

#if DS_CONGIF_HAS_DYN
	#include "Dynamic.h"
#endif

#include "UTIL1.h"
#include "FRTOS1.h"

#include <stdlib.h>
#include <time.h>


/* Variabeln */
static int32_t DS_SS = 0;





int32_t DS_GetState(){
	return 1;
}

int32_t DS_GetSS(){
	return DS_SS;

}



static void DS_StateMachine(void *pvParameters){
	(void)pvParameters; /* not used */

	for(;;){




	vTaskDelay(50);
	}
}




#if DS_CONFIG_HAS_SHELL
/*!
 * \brief Prints the system low power status
 * \param io I/O channel to use for printing status
 */
static void DS_PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[16];

  CLS1_SendStatusStr((unsigned char*)"DS", (unsigned char*)"\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  state", (unsigned char*)"", io->stdOut);
  CLS1_SendNum32s(DS_GetState(), io->stdOut);
  CLS1_SendStr((unsigned char*)" \r\n", io->stdOut);

}

/*!
 * \brief Prints the help text to the console
 * \param io I/O channel to be used
 */
static void DS_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"DS", (unsigned char*)"Group of DriveSwitch commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows Dynamics help or status\r\n", io->stdOut);
}

uint8_t DS_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
	int32_t val;
	const unsigned char *p;

	if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, (char*)"DS help")==0) {
    DS_PrintHelp(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"DS status")==0) {
    DS_PrintStatus(io);
    *handled = TRUE;
  } else if (UTIL1_strncmp(cmd, "DS SS ", sizeof("DS SS ")-1)==0) {
	    p = cmd+sizeof("DS SS ")-1;
	    if (UTIL1_xatoi(&p, &val)==ERR_OK) {
	    	DS_SS = val;
	    	//DYN_SetSS(val); /* We don't set the SS in DYN from DS, but DYN uses DS_GetSS() every cycle */
	    	*handled = TRUE;
	    }  else {
	        return ERR_FAILED; /* wrong format of command? */
	    }
}

  return ERR_OK;
}

#endif /* PL_HAS_SHELL */


void DS_Deinit(void) {
}

void DS_Init(void) {


  // Calculate Speed Task
  if (xTaskCreate(DS_StateMachine, "StateMachine", 400/sizeof(StackType_t), NULL, tskIDLE_PRIORITY+3, NULL) != pdPASS) {
      for(;;){} /* error */
    }
}

