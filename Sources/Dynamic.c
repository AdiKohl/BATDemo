/*
 * Dynamic.c
 *
 *  Created on: 29.06.2019
 *      Author: Adrian Kohler
 */


//#include "Platform.h" /* interface to the platform */


#if 1 || PL_CONFIG_HAS_SHELL
  #include "CLS1.h"


#include "UTIL1.h"
#include "FRTOS1.h"


static int32_t DYN_currSpeed = 0;

int32_t DYN_GetSpeed(){
	return DYN_currSpeed;

}



#if 1 //PL_CONFIG_HAS_SHELL
/*!
 * \brief Prints the system low power status
 * \param io I/O channel to use for printing status
 */
static void DYN_PrintStatus(const CLS1_StdIOType *io) {
  //TACHO_CalcSpeed(); /*! \todo only temporary until this is done periodically */
  CLS1_SendStatusStr((unsigned char*)"Dyn", (unsigned char*)"\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  speed", (unsigned char*)"", io->stdOut);
  CLS1_SendNum32s(DYN_GetSpeed(TRUE), io->stdOut);
  CLS1_SendStr((unsigned char*)" km/h\r\n", io->stdOut);

}

/*!
 * \brief Prints the help text to the console
 * \param io I/O channel to be used
 */
static void DYN_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"Dyn", (unsigned char*)"Group of Dynamics commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows Dynamics help or status\r\n", io->stdOut);
}

uint8_t DYN_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
  if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, (char*)"Dyn help")==0) {
    DYN_PrintHelp(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"Dyn status")==0) {
    DYN_PrintStatus(io);
    *handled = TRUE;
  }
  return ERR_OK;
}
#endif /* PL_HAS_SHELL */

void DYN_Deinit(void) {
}

void DYN_Init(void) {
  DYN_currSpeed = 0;

}

#endif /* PL_CONFIG_HAS_MOTOR_TACHO */

