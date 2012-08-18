/*  TideFridayHarborlib.h 
  A library for calculating the current tide height at 
   Friday Harbor, San Juan Island, San Juan Channel, Washington 
  Luke Miller, June 2012
*/ 
 
#ifndef  TideFridayHarborlib_h 
#define  TideFridayHarborlib_h 
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