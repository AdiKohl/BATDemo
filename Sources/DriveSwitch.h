/*
 * DriveSwitch.h
 *
 *  Created on: 02.07.2019
 *      Author: mail
 */

#ifndef SOURCES_DRIVESWITCH_H_
#define SOURCES_DRIVESWITCH_H_

#include "FRTOS1.h"

#define DS_CONFIG_HAS_SHELL (1)
#define DS_CONGIF_HAS_DYN (1)

/*!
 * \brief Returns the current state.
 * \return DS_State
 */
int32_t DS_GetState(void);

/*!
 * \brief Returns the Spannungsstufe.
 * \return DS_SS
 */
int32_t DS_GetSS(void);




#if DS_CONFIG_HAS_SHELL
#include "CLS1.h"
/*!
 * \brief Parses a command
 * \param cmd Command string to be parsed
 * \param handled Sets this variable to TRUE if command was handled
 * \param io I/O stream to be used for input/output
 * \return Error code, ERR_OK if everything was fine
 */
uint8_t DS_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
#endif

/*! \brief De-initialization of the module */
void DS_Deinit(void);

/*! \brief Initialization of the module */
void DS_Init(void);


#endif /* SOURCES_DRIVESWITCH_H_ */
