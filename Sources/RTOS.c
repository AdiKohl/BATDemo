/**
 * \file
 * \brief Real Time Operating System (RTOS) main program.
 * \author Erich Styger, erich.styger@hslu.ch
 */

//#include "Platform.h"
//#if PL_CONFIG_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"
#include "Bit1.h"
//#include "Application.h"
//#include "LED.h"
//#include "Buzzer.h"
//#include "KeyDebounce.h"

#if 0
#include "LEDExtPin1.h"
#include "LEDExtPin2.h"
int cnt = 9;
#endif

#if PL_CONFIG_HAS_LINE_FOLLOW
#include "LineFollow.h"
static xTaskHandle LFTaskHandle;
#endif

#if PL_CONFIG_HAS_PID
#include "Pid.h"
#endif
#if PL_CONFIG_HAS_CONFIG_NVM
  #include "NVM_Config.h"
#endif


//App Event Handler for RTOS
#if 0
static void RTOS_APP_EventHandler(EVNT_Handle event) {
	/*! \todo handle events */
	switch (event) {
	case EVNT_STARTUP: {
		int i;
		for (i = 0; i < 5; i++) {
			LED1_Neg();
			WAIT1_Waitms(50);
		}
		Bit1_Off();
#if 0
		LEDExtPin1_SetVal();
#endif
#if PL_CONFIG_HAS_BUZZER
		BUZ_PlayTune(BUZ_TUNE_BUTTON);
#endif
	}
	case EVNT_LED_HEARTBEAT: {
		Bit1_Neg();
#if 0
		LEDExtPin1_NegVal();
		LEDExtPin2_NegVal();
		cnt++;
		if(cnt >= 7){
		BUZ_PlayTune(BUZ_TUNE_JINGLEBELLS);
		cnt = 0;
		}
#endif
	}
	break;
#if PL_CONFIG_NOF_KEYS>=1
	case EVNT_SW1_PRESSED:
		LED1_Neg();
		BUZ_PlayTune(BUZ_TUNE_BUTTON);

#if 0
		(void)xTaskNotify(LFTaskHandle, (1<<0)/*LF_START_FOLLOWING*/, eSetBits);//LF_StopFollowing();
#endif

		//BtnMsg(1, "pressed");
	break;
	case EVNT_SW1_LPRESSED:
		LED1_Neg();
		BUZ_PlayTune(BUZ_TUNE_BUTTON_LONG);

#if 0
		//WAIT1_Waitms(1000);
		//(void)xTaskNotify(LFTaskHandle, LF_START_FOLLOWING, eSetBits);//LF_StartFollowing();
#endif


		//BtnMsg(1, "long pressed");
	break;
#endif
	default:
	break;
} /* switch */
}

#endif

/*Task Functions*/
static void MainTaskRTOS(void* brate){
	for(;;){
		 //KEYDBNC_Process();
		 //EVNT_HandleEvent(RTOS_APP_EventHandler, TRUE);
		 vTaskDelay(10);
	}
}

static void BlinkLed(void* brate){
	for(;;){
		Bit1_NegVal();
		FRTOS1_vTaskDelay(100);
	}
}


void RTOS_Init(void) {
  /*! \todo Create tasks here */
	BaseType_t TaskResult;

	//Blink Task LED
	TaskResult = xTaskCreate(MainTaskRTOS,
			"MainTask",
			configMINIMAL_STACK_SIZE,
			(void*) NULL,
			tskIDLE_PRIORITY+1,
			NULL
	);

if(TaskResult != pdPASS){
	for(;;){}
}

	//Blink Task LED
		TaskResult = xTaskCreate(BlinkLed,
				"BlinkLedBaby",
				configMINIMAL_STACK_SIZE,
				(void*) NULL,
				tskIDLE_PRIORITY+1,
				NULL
		);



if(TaskResult != pdPASS){
	for(;;){}
}


/*Starten des Schedulers*/
	vTaskStartScheduler();
}

void RTOS_Deinit(void) {
  /* nothing needed for now */
}

//#endif /* PL_CONFIG_HAS_RTOS */
