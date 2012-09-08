/*  TidelibSanFranciscoNorthPoint.h 
  A library for calculating the current tide height at 
   North Point, Pier 41, San Francisco, San Francisco Bay, California 
  Luke Miller, June 2012
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibSanFranciscoNorthPoint_h 
#define  TidelibSanFranciscoNorthPoint_h 
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