/*  TidelibNeahBay.h 
  A library for calculating the current tide height at 
   Neah Bay, Strait Of Juan De Fuca, Washington 
  Luke Miller, 2015-03-27 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibNeahBay_h 
#define  TidelibNeahBay_h 
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