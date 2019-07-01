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


#define MASS_LOK		(100) 	//Masse der Lok in Tonnen
#define MASS_WAG		(20) 	//Masse eines Wagens in Tonnen
#define ANZ_WAG			(3)		// Anzahl der Wagen im Zug

int Fahrtabelle[29][27] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			,9810,6867,4905,3924,3139,2511,2009,1607,1286,1029,823,658,527,421,337,270,216,173,138,110,88,71,57,45,36,29,23
			,22073,17168,13244,10301,7848,6278,5023,4018,3215,2572,2057,1646,1317,1053,843,674,539,431,345,276,221,177,141,113,90,72,58
			,35316,28449,22563,17658,13734,10791,8633,6906,5525,4420,3536,2829,2263,1810,1448,1159,927,742,593,475,380,304,243,194,156,124,100
			,50522,39240,30902,24525,19130,15696,12263,9320,7456,5964,4772,3817,3054,2443,1954,1564,1251,1001,801,640,512,410,328,262,210,168,134
			,66218,51993,40712,31883,25016,20111,16187,12753,9320,7456,5964,4772,3817,3054,2443,1954,1564,1251,1001,801,640,512,410,328,262,210,168
			,84366,66708,51993,41202,31883,24035,18639,14715,12753,10791,9172,7796,6627,5633,4788,4070,3459,2940,2499,2124,1806,1535,1305,1109,943,801,681
			,105948,83385,65727,51012,39240,31392,25016,20601,17658,14715,12263,10791,9172,7796,6627,5633,4788,4070,3459,2940,2499,2124,1806,1535,1305,1109,943
			,147150,107910,83385,65727,51993,41202,33354,27468,23544,19620,16677,14715,12753,11282,10301,9270,8343,7509,6758,6082,5474,4927,4434,3991,3592,3232,2909
			,188352,144698,108891,85347,66708,53465,42674,35316,29430,24525,21092,18149,15696,13734,12753,11772,11282,10153,9138,8224,7402,6662,5995,5396,4856,4371,3934
			,188352,176580,137340,108891,85347,66708,53955,45126,37278,31392,26978,23544,20111,17168,15500,13832,12753,11576,10301,9476,8718,8021,7379,6789,6246,5746,5286
			,188352,188352,168732,132926,104967,84366,66708,54446,45126,38259,32864,28449,25016,22073,19130,17462,15696,14126,12557,11282,10202,9590,9015,8474,7966,7488,7038
			,188352,188352,188352,166770,131454,105948,83876,66708,55917,46107,39240,33845,29921,26487,23544,20601,18639,16873,15206,13734,12753,11772,11282,10791,10301,10006,9006
			,188352,188352,188352,188352,153036,124587,101534,81423,66708,55917,48069,42183,37278,32864,28940,25997,23544,21092,18639,16873,15402,14225,13244,12753,12263,11772,10595
			,188352,188352,188352,188352,182466,149603,122625,100062,81914,67199,56898,49541,43164,37769,33354,29430,26487,24035,21582,19620,18345,16971,15892,15206,14323,13734,12361
			,188352,188352,188352,188352,188352,176580,142245,117720,96138,80442,68670,58370,50031,43164,38259,34335,30411,27468,25016,22857,21092,19424,18149,17168,16481,15990,14391
			,188352,188352,188352,188352,188352,188352,162846,137340,114287,95157,80933,69651,59841,51993,45617,41006,37278,33354,30411,27468,25016,23544,21582,20307,19130,18639,16775
			,188352,188352,188352,188352,188352,188352,186390,153527,128511,107910,91724,80442,68670,59841,52974,47088,42674,38259,35316,32373,29430,26978,24525,23054,21582,20601,18541
			,188352,188352,188352,188352,188352,188352,188352,176580,151270,125568,105948,90743,79461,68670,60822,53955,48069,43655,40025,36591,33354,30902,28449,26487,24525,22563,20307
			,188352,188352,188352,188352,188352,188352,188352,188352,166770,141264,120663,103986,89762,79461,70142,63275,56898,51503,47088,43164,39240,35807,32373,30019,27468,25506,22955
			,188352,188352,188352,188352,188352,188352,188352,188352,188352,158922,136359,117720,103005,89762,80442,71613,63765,57879,52974,48069,43655,40221,36591,33845,30902,27860,25074
			,188352,188352,188352,188352,188352,188352,188352,188352,188352,179523,154998,134888,117720,102024,89271,79461,70632,63765,57879,52974,48069,43655,40221,37278,34335,31392,28253
			,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,170694,149112,130473,113796,100553,88781,79952,71613,64746,58860,53955,49050,45617,41693,38259,35316,31784
			,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,185409,163337,142736,125568,110853,98100,87800,78971,71613,65237,59841,54936,50423,46107,42183,39240,35316
			,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,179523,157941,139302,124097,109872,98100,87309,78971,71613,65237,59841,54936,50031,45617,42183,37965
			,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,175599,156960,140283,124097,110363,97119,86328,78480,71613,64746,59841,55427,51012,47088,42379
			,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,171675,152546,135869,121644,108401,96138,86819,78971,71809,65727,60822,56408,52974,47677
			,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,186390,165299,147150,131454,117720,104967,94176,86328,78480,71613,65727,61313,56898,51208
			,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,188352,180995,163337,147150,131454,117720,105948,95157,86328,78480,71613,65727,60822,54740
	};

int Bremstabelle[19][27] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			,1570,1643,1717,1778,1839,1839,1839,1778,1717,1655,1594,1533,1472,1373,1275,1177,1079,981,907,809,736,711,687,662,638,613,589
			,3139,3286,3434,3556,3679,3679,3679,3556,3434,3311,3188,3066,2943,2747,2551,2354,2158,1962,1815,1619,1472,1422,1373,1324,1275,1226,1177
			,4709,4930,5150,5334,5518,5518,5518,5334,5150,4966,4782,4598,4415,4120,3826,3532,3237,2943,2722,2428,2207,2134,2060,1987,1913,1839,1766
			,6278,6573,6867,7112,7358,7358,7358,7112,6867,6622,6377,6131,5886,5494,5101,4709,4316,3924,3630,3237,2943,2845,2747,2649,2551,2453,2354
			,9810,10134,10457,10791,10791,10693,10595,10301,9810,9320,8829,8437,8044,7456,6867,6377,5984,5592,5199,4807,4415,4120,3826,3532,3237,2943,2649
			,13734,14028,14323,14519,14519,14421,14077,13734,13244,12753,12066,11331,10595,9614,9025,8339,7750,7161,6671,6377,6131,5886,5641,5396,5150,4905,4660
			,17658,17658,17658,17658,17658,17658,17462,17069,16677,16187,15500,14617,13734,12949,12164,11380,10595,9810,9221,8731,8044,7358,6965,6475,6082,5886,5690
			,22073,22857,23054,22955,22857,22759,22367,21876,21288,20601,19620,18443,17266,16187,15206,14225,13244,12263,11527,10791,10301,9810,9320,8829,8339,7848,7603
			,26978,27664,28106,28449,28449,28086,27468,26732,25997,24917,23838,22759,21582,20601,19620,18639,17658,16677,15696,14813,13930,13244,12557,11968,11380,10791,10301
			,33550,34826,35316,35610,35316,34826,34139,33158,32049,30902,29430,27959,26487,25016,23740,22563,21337,20111,19130,18149,17413,16677,15990,15206,14421,13734,13244
			,40221,41202,41987,42183,41889,41300,40712,39731,38553,37278,35807,34163,32520,30877,29234,27860,26487,25114,23838,22563,21501,20438,19376,18313,17251,16187,15304
			,48069,50031,50522,50816,50227,49541,48069,46598,45126,43655,42183,40221,38259,36297,34335,32667,31000,29234,27664,25997,24329,22955,21582,20601,19620,18835,18149
			,55917,57389,58566,58860,58370,57389,55917,54446,52484,50522,48560,46303,44047,41791,39534,37474,35610,33943,32373,31000,29724,28449,27223,25997,24770,23544,22563
			,65237,66708,67689,67885,67493,66512,65237,63275,61313,58860,56506,54151,51797,49442,47088,45028,42968,40908,38848,37278,35807,34237,32667,31098,29528,27959,26978
			,73575,75537,77009,77303,77009,76518,75439,73575,71613,69651,67002,64256,61313,58370,55427,52974,50522,48069,46107,44145,42183,40221,38504,36788,35071,33354,31883
			,83385,85347,86819,87309,87015,86328,85151,83385,81423,78971,76028,73085,70215,67346,64476,61607,58762,55917,53465,51012,48658,46303,44292,42281,40270,38259,36493
			,93195,96138,97610,97806,97119,96138,94667,92901,90743,88290,85347,82404,79461,76273,73085,69847,66806,63765,60822,57879,55525,53219,51012,49050,47088,45126,43655
			,103005,105359,106929,107223,106439,105752,104477,102809,100847,98100,95648,92705,89565,86009,82453,78897,75341,72300,69259,66218,63441,60665,57879,55593,53308,51012,49050
	};

/* Variabeln */
static int32_t DYN_currSpeed;
static int32_t DYN_MassZug;

static float geschwindigkeit;
static float geschwindigkeitneu;
static float beschleunigung;

static uint32_t DYN_SS;

// static int bisherigesStreckenelement = 0;





int32_t DYN_GetSpeed(){
	return DYN_currSpeed;

}

void DYN_CalcSpeed(){
	int32_t result;

	// Funktionseigene Variabeln
	int fhang = 0; //Da keine Streckenelemete vorhanden sind, kann fhang nicht berechnet werden.
	int Fk = 0; // Da keine Streckenelente vorhanden sind, kann Fk nicht berechnet werden.
	int fantrieb;
	int fbrems;
	int fmechbrems;
	int fwiderst;
	int fbremstotal;
	int fbeschl;
	int fres;

	int vreal;


	for(;;){

		int VStufe = ((int)geschwindigkeit / 130) * 27;

		if (DYN_SS > 0) {
			fantrieb = Fahrtabelle[DYN_SS][VStufe];
			//IF = stromberechnen(fantrieb);
			fbrems = 0;
		} else if (DYN_SS < 0) {
			fbrems = Bremstabelle[DYN_SS][VStufe];
			//IF = stromberechnen(fbrems);
			fantrieb = 0;
		} else {
			fantrieb = 0;
			fbrems = 0;
			//IF = 0;
		}




	result = fantrieb;
	DYN_currSpeed = result;



	 vTaskDelay(10);
	}
}





#if 1 //PL_CONFIG_HAS_SHELL
/*!
 * \brief Prints the system low power status
 * \param io I/O channel to use for printing status
 */
static void DYN_PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[16];

  CLS1_SendStatusStr((unsigned char*)"Dyn", (unsigned char*)"\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  speed", (unsigned char*)"", io->stdOut);
  CLS1_SendNum32s(DYN_GetSpeed(TRUE), io->stdOut);
  CLS1_SendStr((unsigned char*)" km/h\r\n", io->stdOut);
  UTIL1_Num32sToStr(buf, sizeof(buf), DYN_SS);
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
  CLS1_SendHelpStr((unsigned char*)"  SS [val]", (unsigned char*)"Set the SS\r\n", io->stdOut);
}

uint8_t DYN_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
	uint32_t val;
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
	    } else {
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

#endif /* PL_CONFIG_HAS_MOTOR_TACHO */

