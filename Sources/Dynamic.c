/*
 * Dynamic.c
 *
 *  Created on: 29.06.2019
 *      Author: Adrian Kohler
 */



/* Includes */
#include "Dynamic.h"

#if DYN_CONFIG_HAS_SHELL
  #include "CLS1.h"
#endif

#if DYN_CONFIG_HAS_DS
	#include "DriveSwitch.h"
#endif

#include "UTIL1.h"
#include "FRTOS1.h"
#include <stdlib.h>

/* Defines for constants (change if needed) */
#define MASS_LOK		(100) 	//Masse der Lok in Tonnen
#define MASS_WAG		(20) 	//Masse eines Wagens in Tonnen
#define ANZ_WAG			(3)		// Anzahl der Wagen im Zug

/* Include the Fahr- and Bremstabelle */
#include "Tabellen.txt"


/* Variables needed for communication with J-Scope */
static int32_t DYN_currSpeed = 0;
static int32_t DYN_MassZug = 0;
static int32_t DYN_FAntrieb = 0;
static int32_t DYN_BTotal = 0;
static int32_t DYN_kiloAcc = 0;
static int32_t DYN_IF = 0;
static int32_t DYN_IFnext = 0;
static int32_t DYN_VStufe = 0;

/* Variables used for the calculation */
static float DYN_fSpeed = 0;
static float DYN_fSpeednew = 0;
static float DYN_fAcc = 0;

/* Inputvariables */
static int32_t DYN_SS = 0;
static int32_t DYN_MB = 0;

/*--------------------------------------------------*/


int32_t DYN_GetSpeed(){
	return DYN_currSpeed;

}

int32_t DYN_GetIF(){
	return DYN_IF;
}

int32_t DYN_GetIFnext(){
	return DYN_IFnext;
}

void DYN_SetSS(int32_t value){
	DYN_SS = value;
}


int32_t DYN_CalcCurr(int32_t kraft){
	if(kraft < 1472){
		return (int32_t)(200*(kraft/1472)); // interpolation: strom = ya + (yb-ya)*(x-xa)/(xb-xa)
	}else if (kraft < 2943){
		return (int32_t)(200+100*(kraft-1472)/(1472));
	}else if (kraft < 7358){
		return (int32_t)(300+100*(kraft-2943)/(4415));
	}else if (kraft < 11772){
		return (int32_t)(400+100*(kraft-7358)/(4415));
	}else if (kraft < 17658){
		return (int32_t)(500+100*(kraft-11772)/(5886));
	}else if (kraft < 26478){
		return (int32_t)(600+100*(kraft-17658)/(8829));
	}else if (kraft < 39240){
		return (int32_t)(700+150*(kraft-26478)/(12753));
	}else if (kraft < 100062){
		return (int32_t)(850+650*(kraft-39240)/(60822));
	}else {
		return (int32_t)(1500+900*(kraft-100062)/(88290));
	}

}

/*--------------------------------------------------*/

/*!
 * \brief Calculates the speed based on the Input from the DriveSwitch file
 * \this is the function used by the task
 */

static void DYN_CalcSpeed(void *pvParameters){

	(void)pvParameters; /* not used */

	/* initialise timekeeping */
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();




	/* Function variables not visible from the outside of this function*/
	int32_t fhang = 0; 	// Da keine Streckenelemente vorhanden sind, kann fhang nicht berechnet werden.
	int32_t Fk = 0; 	// Da keine Streckenelemente vorhanden sind, kann Fk nicht berechnet werden.
	int32_t fantrieb;
	int32_t fantriebnext;
	int32_t fbrems;
	int32_t fmechbrems;
	int32_t fwiderst;
	int32_t fbremstotal;
	int32_t fbeschl;
	int32_t fres;

	/* Vraibles used for the interpolation of the values from the Fahr- and Bremstabellen */
	int32_t VStufeX;
	int32_t VStufeY;
	int32_t geschwgeru;
	float x; //Interpolationsvariable



	for(;;){

#if DYN_CONFIG_HAS_DS
		DYN_SS = DS_GetSS();
#endif

		/* Calculation of the 2 spots to interpolate between */
		VStufeX = (int)(DYN_fSpeed) / 5;
		VStufeY = 1+(int)(DYN_fSpeed) / 5;
		DYN_VStufe = VStufeX;

		x = DYN_fSpeed-(float)(DYN_VStufe*5);


		/* Readout of the tables wiht interpolation in both cases (Fahren / Bremsen) */
		if(DYN_SS > 0) {
			fantrieb = Fahrtabelle[DYN_SS][VStufeX]+(int)((float)((Fahrtabelle[DYN_SS][VStufeY]-Fahrtabelle[DYN_SS][VStufeX])/5)*x);
			if(DYN_SS < 28){ /* also calculate for the next SS */
				fantriebnext = Fahrtabelle[DYN_SS+1][VStufeX]+(int)((float)((Fahrtabelle[DYN_SS+1][VStufeY]-Fahrtabelle[DYN_SS+1][VStufeX])/5)*x);
			}else{
				fantriebnext = fantrieb;
			}
			DYN_IF = DYN_CalcCurr(fantrieb);
			DYN_IFnext = DYN_CalcCurr(fantriebnext);
			fbrems = 0;
		} else if(DYN_SS < 0) {
			fbrems = Bremstabelle[-DYN_SS][VStufeX]+(int)((float)((Bremstabelle[-DYN_SS][VStufeY]-Bremstabelle[-DYN_SS][VStufeX])/5)*x);
			DYN_IF = DYN_CalcCurr(fbrems);
			if(DYN_fSpeed == 0) DYN_IF = 0;
			fantrieb = 0;
		} else {
			fantrieb = 0;
			fbrems = 0;
			DYN_IF = 0;
		}


#if 0 	/* These parts need hardware implementation of on-track-sensors. These do not exist yet, so calculation is neglected */
		/* If you want to add these, make sure to test them, since those functions are still in a concept-like state */

		// Berechnung der Hangabtriebskraft und Kr�mmungswiderstand
	if (bisherigesStreckenelement != Streckenelement) { //Pr�fen, ob neues Streckenelement erreicht wurde

		fhang = DYN_MassZug * 9.81
				* Steigung[bisherigesStreckenelement][Streckenelement]; //in [N].
		//Da die Masse sehr hoch ist, kann es sich lohnen Nachkommastellen nicht zu berechnen/weiterzugeben (int ausreichend)
		// 1 promille Steigung ergibt eine Hangabtriebskraft von 821N (bei 80t Zuggewicht)
		// 30 promille Steigung ergibt eine Hangabtriebskraft von 23555N (bei 80t Zuggewicht)

		int radius = Kurven[Streckenelement]; //auslesen des Kurvenradius
			if(radius > 300){
					Fk = DYN_MassZug * 9.81 * 650 /(radius-55);
				} else {
					Fk = DYN_MassZug * 9.81 * 500 /(radius-30);
				}

		bisherigesStreckenelement = Streckenelement;//Das neue Streckenelement zum aktuellen Streckenelement machen
	}

#endif

		/* Calculation of all resistive forces. See documentation for formulas */
		// Mechanical brakes (pneumaticly actuated)
		 fmechbrems = DYN_MB * 776; //Needs to be thoght over again. Currently there is no interface to read in the break lever! 198000 / 255


		// Driving resistances. mass = [t], velocity = [km/h], acceleration = [m/s2], forces = [N]

		// Acceleration resistance. The formula is actually 1000*mass*acc*1.1 but this created problems with the time-discrete calculations, so it was toned down
		float Fb = 1 * DYN_MassZug * DYN_fAcc * 1.1;

		// Roll resistance (velocity undependant)
		int32_t Fr = 15 * DYN_MassZug;

		// Push resistance (velocity dependant)
		float Fs = 0.025 * DYN_MassZug * DYN_fSpeed;

		// Air resistance (velocity^2 dependant)
		float Fl = (0.62 + 0.1 * (ANZ_WAG - 2)) * 6.25 * DYN_fSpeed * DYN_fSpeed;

		// total resistance
		fwiderst = Fb + Fr + Fs + Fl + Fk;

		// Sum of accelerating forces
		fbeschl = fantrieb + fhang;

		// Sum of breaking forces and correct direction of those (always against velocity or (if v=0) against accelerating force
		if (DYN_fSpeed > 0) {
			fbremstotal = fwiderst + fmechbrems + fbrems;

		} else if (DYN_fSpeed < 0) {
			fbremstotal = -(fwiderst + fmechbrems + fbrems);
		} else {
			if (fbeschl >= 0) {
				fbremstotal = fwiderst + fmechbrems + fbrems;
			} else {
				fbremstotal = -(fwiderst + fmechbrems + fbrems);
			}
		}

		// Sum of accelerating and breaking forces. breaking forces may not accelerate the train from v=0

		if(DYN_fSpeed == 0){
			if (abs(fbeschl) - abs(fbremstotal) < 0) {
				fres = 0;
			}else{
				fres = fbeschl - fbremstotal;
			}
		} else {
			fres = fbeschl - fbremstotal;
		}

		// calculate acceleration (F=m*a)
		DYN_fAcc = (float)fres / (DYN_MassZug*1000);


		// integrate acceleration and prevent pass through zero (problematic in time-discrete systems)
		DYN_fSpeednew = DYN_fSpeed + DYN_fAcc * 0.01;
		if((DYN_fSpeed > 0 && DYN_fSpeednew < 0) || (DYN_fSpeed < 0 && DYN_fSpeednew > 0)){
			DYN_fSpeed = 0;
		}else {
			DYN_fSpeed = DYN_fSpeednew;
		}



		// confine speed so that readout of tables does not try to access values outside of the tables
		if(DYN_fSpeed > 130) DYN_fSpeed = 130;
		if(DYN_fSpeed < -130) DYN_fSpeed = -130;





		//Write variables for J-Scope (cast to Integer)
		DYN_currSpeed = DYN_fSpeed;
		DYN_FAntrieb = fantrieb;
		DYN_BTotal = fbremstotal;
		DYN_kiloAcc = (int)(DYN_fAcc*1000);



		/* Put thread to sleep */
		FRTOS1_vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
	}
}





#if DYN_CONFIG_HAS_SHELL
/*!
 * \brief Prints the system low power status
 * \param io I/O channel to use for printing status
 */
static void DYN_PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[16];

  CLS1_SendStatusStr((unsigned char*)"Dyn", (unsigned char*)"\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  speed", (unsigned char*)"", io->stdOut);
  CLS1_SendNum32s(DYN_GetSpeed(), io->stdOut);
  CLS1_SendStr((unsigned char*)" km/h\r\n", io->stdOut);
  UTIL1_Num32sToStr(buf, sizeof(buf), DS_GetSS());
  UTIL1_strcat(buf, sizeof(buf), "\r\n");
  CLS1_SendStatusStr("  SS", buf, io->stdOut);

}

/*!
 * \brief Prints the help text to the console
 * \param io I/O channel to be used
 */
static void DYN_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"Dyn", (unsigned char*)"Group of Dynamics commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows Dynamics help or status\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  SS [-18 .. 28]", (unsigned char*)"Set the SS\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  MB [0 .. 255]", (unsigned char*)"Set the SS\r\n", io->stdOut);
}

uint8_t DYN_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
	int32_t val;
	const unsigned char *p;

	if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, (char*)"Dyn help")==0) {
    DYN_PrintHelp(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"Dyn status")==0) {
    DYN_PrintStatus(io);
    *handled = TRUE;
  } else if (UTIL1_strncmp(cmd, "Dyn SS ", sizeof("Dyn SS ")-1)==0) {
	    p = cmd+sizeof("Dyn SS ")-1;
	    if (UTIL1_xatoi(&p, &val)==ERR_OK) {
	    	DYN_SS = val;
	    	*handled = TRUE;
	    }  else {
	        return ERR_FAILED; /* wrong format of command? */
	    }
  } else if (UTIL1_strncmp(cmd, "Dyn MB ", sizeof("Dyn MB ")-1)==0) {
	    p = cmd+sizeof("Dyn MB ")-1;
	    if (UTIL1_xatoi(&p, &val)==ERR_OK) {
	    	DYN_MB = abs(val);
	    	*handled = TRUE;
	    }  else {
	        return ERR_FAILED; /* wrong format of command? */
	    }
}


  return ERR_OK;
}
#endif /* PL_HAS_SHELL */

void DYN_Deinit(void) {
}

void DYN_Init(void) {
  DYN_currSpeed = 0;
  DYN_MassZug = MASS_LOK + MASS_WAG*ANZ_WAG;

  // Calculate Speed Task
  if (xTaskCreate(DYN_CalcSpeed, "CalcSpeed", 400/sizeof(StackType_t), NULL, tskIDLE_PRIORITY+3, NULL) != pdPASS) {
      for(;;){} /* error */
    }



}


