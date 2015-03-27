/*  TidelibPier41SanFrancisco.h 
  A library for calculating the current tide height at 
   North Point, Pier 41, San Francisco, San Francisco Bay, California 
  Luke Miller, 2015-03-27 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibPier41SanFrancisco_h 
#define  TidelibPier41SanFrancisco_h 
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