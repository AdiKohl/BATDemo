/*
 * Dynamic.h
 *
 *  Created on: 29.06.2019
 *      Author: mail
 */

#ifndef SOURCES_DYNAMIC_H_
#define SOURCES_DYNAMIC_H_




#if 1 //PL_CONFIG_HAS_MOTOR_TACHO
/*!
 * \brief Returns the previously calculated speed of the motor.
 * \param isLeft TRUE for left speed, FALSE for right speed.
 * \return Actual speed value
 */
int32_t DYN_GetSpeed(void);

/*!
 * \brief Calculates the speed based on the position information from the encoder.
 */
void DYN_CalcSpeed(void);


#if 1 //PL_CONFIG_HAS_SHELL
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

#endif /* PL_CONFIG_HAS_MOTOR_TACHO */

#endif /* SOURCES_DYNAMIC_H_ */
