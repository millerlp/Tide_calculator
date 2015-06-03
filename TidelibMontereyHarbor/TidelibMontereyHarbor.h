/*  TidelibMontereyHarbor.h 
  A library for calculating the current tide height at 
   Monterey, Monterey Harbor, California 
  Luke Miller, 2015-06-03 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibMontereyHarbor_h 
#define  TidelibMontereyHarbor_h 
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