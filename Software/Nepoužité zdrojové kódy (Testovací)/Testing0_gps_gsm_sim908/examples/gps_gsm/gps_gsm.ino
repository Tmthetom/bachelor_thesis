/********************* start of gps_gsm.ino *************************/

/*
 *	created:	2013-08-02
 *	by:		lisper (leyapin@gmail.com)
 *	Version:	0.1
 *	test gps gsm on Leonardo &XBEE R3
 *
 */

#ifndef DEBUG
#define DEBUG
#endif

#include "gps_gsm_sim908.h"

//for test, changed it to yours phone numble 
char *phone_num = "18501680000";


//
void setup () {
	gps_init ();	//init GPS pin

	#ifdef DEBUG
	Serial.begin (9600);	//serial0 connect computer
	while (!Serial);
	#endif

	Serial1.begin (9600);	//serial1 connect GPS
	while (!Serial1);

	#ifdef DEBUG
	Serial.println ("start GPS! ");
	#endif

	start_gps ();	//open GPS
}

//
void loop () {
	int stat = gps_get_gga ();	// read data from GPS, return 0 is ok

	if (stat == 0 || stat == 1) {
		if (gps_gga_is_fix ()) {	//true if fix
			//send_message (phone_num, gps_gga_utc_s ());
			gsm_begin_msg (phone_num);
			gsm_send_msgln (gps_gga_utc_s ());
			gsm_send_msgln (gps_gga_EW_s ());
			gsm_send_msgln (gps_gga_NS_s ());
			gsm_send_msgln (gps_gga_lat_s ());
			gsm_send_msgln (gps_gga_long_s ());
			gsm_stop_send ();
			while (1);
		}
	}

	//
	switch (stat) {
	case 0:
		#ifdef DEBUG
		Serial.println ("data checksum is ok");
		#endif
		break;
	case 1:
		#ifdef DEBUG
		Serial.println ("error! data checksum is fail");
		#endif
		break;
	case 2:
		#ifdef DEBUG
		Serial.println ("GPGGA ID is error!");
		#endif
		break;
	case 3:
		#ifdef DEBUG
		Serial.println ("data is error!");
		#endif
		break;
	}

	#ifdef DEBUG
	Serial.println ("$GPGGA data:");
	gps_gga_print ();	//for test
	#endif
}


/********************* end of gps_gsm.ino *************************/
