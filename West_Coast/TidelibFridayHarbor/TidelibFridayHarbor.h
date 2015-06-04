/*  TidelibFridayHarbor.h 
  A library for calculating the current tide height at 
   Friday Harbor, San Juan Island, San Juan Channel, Washington 
  Luke Miller, 2015-06-03 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibFridayHarbor_h 
#define  TidelibFridayHarbor_h 
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