/*  TidelibSeattle.h 
  A library for calculating the current tide height at 
   Seattle, Puget Sound, Washington 
  Luke Miller, 2015-03-27 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibSeattle_h 
#define  TidelibSeattle_h 
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