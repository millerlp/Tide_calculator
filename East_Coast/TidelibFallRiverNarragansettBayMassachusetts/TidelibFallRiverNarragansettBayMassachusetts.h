/*  TidelibFallRiverNarragansettBayMassachusetts.h 
  A library for calculating the current tide height at 
   Fall River, Narragansett Bay, Massachusetts 
  Luke Miller, 2015-06-03 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibFallRiverNarragansettBayMassachusetts_h 
#define  TidelibFallRiverNarragansettBayMassachusetts_h 
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