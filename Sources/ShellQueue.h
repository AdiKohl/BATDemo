/**
 * \file
 * \brief This is the interface to the shell queue module
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module is used pass messages from other tasks to the shell task.
 */

#ifndef SHELL_QUEUE_C_
#define SHELL_QUEUE_C_

//#include "Platform.h"
#if 1 //PL_CONFIG_HAS_SHELL_QUEUE

/*!
 * \brief Sends a string to the queue. Note that if the queue is full, it will block.
 * \param str Pointer to the string.
 */
void SQUEUE_SendString(const unsigned char *str);

/*!
 * \brief Returns the number of elements (characters) in the queue.
 * \return Number of characters in the queue.
 */
unsigned short SQUEUE_NofElements(void);

#if PL_CONFIG_SQUEUE_SINGLE_CHAR
/*!
 * \brief Receives an a message character from the queue, and returns immediately if the queue is empty.
 * \return Message character, or '\0' if there was no message.
 */
unsigned char SQUEUE_ReceiveChar(void);
#else
unsigned char *SQUEUE_ReceiveMessage(void);
#endif

/*! \brief Initializes the queue module */
void SQUEUE_Init(void);

/*! \brief Deinitializes the queue module */
void SQUEUE_Deinit(void);
#endif /* PL_CONFIG_HAS_SHELL_QUEUE */

#endif /* SHELL_QUEUE_C_ */
