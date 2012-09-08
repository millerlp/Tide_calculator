/*  TidelibPointReyes.h 
  A library for calculating the current tide height at 
   Drakes Bay, Point Reyes, California 
  Luke Miller, June 2012
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibPointReyes_h 
#define  TidelibPointReyes_h 
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