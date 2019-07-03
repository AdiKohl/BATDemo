/*
 * DriveSwitch.c
 *
 *  Created on: 02.07.2019
 *      Author: mail
 */

/* Standard includes. */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

/* Kernel includes. */
#include "UTIL1.h"
#include "FRTOS1.h"
#include "task.h"
#include "semphr.h"

#include "DriveSwitch.h"

#if DS_CONFIG_HAS_SHELL
  #include "CLS1.h"
#include "Shell.h"
#endif

#if DS_CONGIF_HAS_DYN
	#include "Dynamic.h"
#endif

/* The rate at which show state is sent from ShowState to Controller.
The times are converted from milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define ShowState_FREQUENCY_MS	pdMS_TO_TICKS( 50 )

/* The rate at which button input is checked by ReadButton.
The times are converted from milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define ReadInput_FREQUENCY_MS	pdMS_TO_TICKS( 10 )

/* The number of items the queue can hold at once. */
#define QUEUE_LENGTH	( 2 )

/* The values sent to the queueController. */
#define BUTTON_NONE			( -1L )
#define BUTTON_LKS			( 1UL )
#define BUTTON_SET			( 2UL )
#define BUTTON_CANCEL		( 3UL )
#define LANE_DETECT_0		( 4UL )
#define LANE_DETECT_1		( 5UL )

/*-----------------------------------------------------------*/

/* the Tasks used for the state machine */
static void tskReadInput   (void *pvParameters);
static void tskShowState    (void *pvParameters);
static void DS_Controller   (void *pvParameters);

/*-----------------------------------------------------------*/

/* The queue used by ReadButton, ShowState, and Controller tasks. */
static QueueHandle_t queueController = NULL;

/* interrupt number and handler*/
static unsigned long showStateInterruptNumber = 3;
static unsigned long showStateInterruptHandler(void)
{
    uint32_t ulQueuedValue;
    ulQueuedValue = "interrupt";
    xQueueSend(queueController, &ulQueuedValue, 0U);
    return pdTRUE;
}

/*-----------------------------------------------------------*/

/* Variabeln */
static int32_t DS_SS = 0;
static int32_t DS_STNR = 0;
static unsigned char* DS_STNM;





int32_t DS_GetState(){
	return DS_STNR;
}

unsigned char* DS_GetStateName(){
	return DS_STNM;
}

int32_t DS_GetSS(){
	return DS_SS;

}

/*-----------------------------------------------------------*/

static void tskReadInput(void *pvParameters)
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = ReadInput_FREQUENCY_MS;
    //uint32_t ulReceivedValue;
    //uint32_t ulQueuedValue;

    /* Prevent the compiler warning about the unused parameter. */
    (void)pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for (;; ) {
#if 0
        /* Wait until a key has been pressed. */
        if (_kbhit() != 0)
        {
            /* Remove the key from the input buffer. */
            ulReceivedValue = _getch();
            /* parse input. */
            ulQueuedValue = "none";
            switch (ulReceivedValue)
            {
            case (uint32_t) 'l': ulQueuedValue = BUTTON_LKS; break;
            case (uint32_t) 's': ulQueuedValue = BUTTON_SET; break;
            case (uint32_t) 'c': ulQueuedValue = BUTTON_CANCEL; break;
            case (uint32_t) '0': ulQueuedValue = LANE_DETECT_0; break;
            case (uint32_t) '1': ulQueuedValue = LANE_DETECT_1; break;
            }
            /* queue input. */
            xQueueSend(queueController, &ulQueuedValue, 0U);
        }
#endif
        vTaskDelayUntil(&xNextWakeTime,xBlockTime);

    }
}

/*-----------------------------------------------------------*/

static void tskShowState(void *pvParameters)
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = ShowState_FREQUENCY_MS;

    /* Prevent the compiler warning about the unused parameter. */
    (void)pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for (;; )
    {
        /* generate interrupt. */
        //vPortGenerateSimulatedInterrupt(showStateInterruptNumber);
    	uint32_t ulQueuedValue;
        ulQueuedValue = "interrupt";
        xQueueSend(queueController, &ulQueuedValue, 0U);

        /* Place this task in the blocked state until it is time to run again.
        While in the Blocked state this task will not consume any CPU time. */
        vTaskDelayUntil(&xNextWakeTime, xBlockTime);
    }
}

/*-----------------------------------------------------------*/
/* These are the used states. Edit as needed */
struct main_state;
typedef void main_state_fn(struct main_state *);
struct main_state
{
    main_state_fn * next;
    uint32_t button;
    uint32_t statenr;
    unsigned char* statename;
    bool led_lks;
    bool led_lane;
    bool led_steering;
    bool servo;
};

main_state_fn	start,
				DS_ST_FB0,
                DS_ST_FN,
                DS_ST_FD,
                DS_ST_FM,
                DS_ST_FP,
				DS_ST_FPP,
				DS_ST_BN,
				DS_ST_BD,
				DS_ST_BP;

void start(struct main_state * state)
{
	state->statenr = 0;
    state->next = DS_ST_FB0;
}

void DS_ST_FB0(struct main_state * state)
{
	state->statenr = 1;
	state->statename = "FB0";
    if (state->button == BUTTON_LKS){
        state->led_lks = true;
        state->next = DS_ST_FN;
    }
    else{
        state->next = DS_ST_FB0;
    }
}

void DS_ST_FN(struct main_state * state)
{
	state->statenr = 2;
	state->statename = "FN";
    if (state->button == BUTTON_LKS){
        state->led_lks = false;
        state->next = DS_ST_FB0;
    }
    else if (state->button == LANE_DETECT_1){
        state->led_lane = true;
        state->next = DS_ST_FD;
    }
    else{
        state->next = DS_ST_FN;
    }
}

void DS_ST_FD(struct main_state * state)
{
	state->statenr = 3;
	state->statename = "FD";
    if (state->button == BUTTON_LKS){
        state->led_lks = false;
        state->led_lane = false;
        state->next = DS_ST_FB0;
    }
    else if (state->button == LANE_DETECT_0){
        state->led_lane = false;
        state->next = DS_ST_FM;
    }
    else if (state->button == BUTTON_SET){
        state->led_steering = true;
        state->servo = true;
        state->next = DS_ST_FP;
    }
    else{
        state->next = DS_ST_FD;
    }
}

void DS_ST_FM(struct main_state * state)
{
	state->statenr = 4;
	state->statename = "FM";
    if (state->button == BUTTON_LKS){
        state->led_lks = false;
        state->next = DS_ST_FB0;
    }
    else if (state->button == LANE_DETECT_1){
        state->led_lane = true;
        state->next = DS_ST_FD;
    }
    else{
        state->next = DS_ST_FM;
    }
}

void DS_ST_FP(struct main_state * state)
{
	state->statenr = 5;
	state->statename = "FP";
    if (state->button == BUTTON_LKS){
        state->led_lks = false;
        state->led_lane = false;
        state->led_steering = false;
        state->servo = false;
        state->next = DS_ST_FB0;
    }
    else if (state->button == BUTTON_CANCEL){
        state->led_steering = false;
        state->servo = false;
        state->next = DS_ST_FD;
    }
    else if (state->button == LANE_DETECT_0){
        state->led_lane = false;
        state->led_steering = false;
        state->servo = false;
        state->next = DS_ST_FM;
    }
    else{
        state->next = DS_ST_FP;
    }
}

/*-----------------------------------------------------------*/


static void DS_Controller(void *pvParameters){


	(void)pvParameters;


	 uint32_t ulReceivedValue;

	    /* init FSM */
	    struct main_state main_state = {start, "", 0, false, false, false, false};


	for(;;) {
        /* Wait until something arrives in the queue - this task will block indefinitely.
        It will not use any CPU time while it is in the Blocked state. */
        xQueueReceive(queueController, &ulReceivedValue, portMAX_DELAY);

        /* update FSM */
        main_state.button = ulReceivedValue;
        main_state.next (&main_state);
        DS_STNR = main_state.statenr;
        DS_STNM = main_state.statename;



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
  CLS1_SendStr(DS_GetStateName(), io->stdOut);
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
} else if (UTIL1_strncmp(cmd, "DS ST ", sizeof("DS ST ")-1)==0) {
    p = cmd+sizeof("DS ST ")-1;
    if (UTIL1_xatoi(&p, &val)==ERR_OK) {
    	xQueueSend(queueController, &val, 0U);
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
		/* Create the queues. */
	    queueController  = xQueueCreate(QUEUE_LENGTH, sizeof(uint32_t));
	    if (queueController != NULL){
	    	/* Create the tasks */
	        if (xTaskCreate(tskReadInput, "ReadButton", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL ) != pdPASS){
	        	for(;;){} /* error */
	        }

	        if(xTaskCreate(tskShowState, "ShowState", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL ) != pdPASS) {
	        	for(;;){} /* error */
	        }

	    	// main state machine task
	    	if (xTaskCreate(DS_Controller, "Controller", configMINIMAL_STACK_SIZE /*400/sizeof(StackType_t)*/, NULL, tskIDLE_PRIORITY+3, NULL) != pdPASS) {
	    		for(;;){} /* error */
	    	}
	     }
}

