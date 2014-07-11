/*  TidelibCharlestonCooperRiverEntranceSouthCarolina.h 
  A library for calculating the current tide height at 
   Charleston, Cooper River Entrance, South Carolina 
  Luke Miller, June 2012
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibCharlestonCooperRiverEntranceSouthCarolina_h 
#define  TidelibCharlestonCooperRiverEntranceSouthCarolina_h 
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include "RTClib.h"

class TideCalc {
 public:
	 TideCalc();
    float currentTide(DateTime now);
    char* returnStationID(void);
};
#endif