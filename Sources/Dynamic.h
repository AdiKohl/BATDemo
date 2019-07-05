/*
 * Dynamic.h
 *
 *  Created on: 29.06.2019
 *      Author: mail
 */

#ifndef SOURCES_DYNAMIC_H_
#define SOURCES_DYNAMIC_H_


#include "FRTOS1.h"


#define DYN_CONFIG_HAS_SHELL (1)
#define DYN_CONFIG_HAS_DS (1)




/*!
 * \brief Returns the calculated speed.
 * \return speed value in [km/h]
 */
int32_t DYN_GetSpeed(void);

/*!
 * \brief Returns the calculated current.
 * \return current in [A]
 */
int32_t DYN_GetIF(void);

/*!
 * \brief Returns the calculated current.
 * \return current in [A]
 */
int32_t DYN_GetIFnext(void);


/*!
 * \brief Sets the DYN_SS variable (used with the Shell).
 */
void DYN_SetSS(int32_t value);

/*!
 * \brief Calculates the current flowing through the motors based on the generated force
 */
int32_t DYN_CalcCurr(int32_t kraft);



#if DYN_CONFIG_HAS_SHELL
#include "CLS1.h"
/*!
 * \brief Parses a command
 * \param cmd Command string to be parsed
 * \param handled Sets this variable to TRUE if command was handled
 * \param io I/O stream to be used for input/output
 * \return Error code, ERR_OK if everything was fine
 */
uint8_t DYN_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
#endif

/*! \brief De-initialization of the module */
void DYN_Deinit(void);

/*! \brief Initialization of the module */
void DYN_Init(void);



#endif /* SOURCES_DYNAMIC_H_ */
