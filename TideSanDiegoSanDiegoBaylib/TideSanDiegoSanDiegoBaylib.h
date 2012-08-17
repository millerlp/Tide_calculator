/*  TideSanDiegoSanDiegoBaylib.h 
  A library for calculating the current tide height at 
   San Diego, San Diego Bay, California 
  Luke Miller, June 2012
*/ 
 
#ifndef  TideSanDiegoSanDiegoBaylib_h 
#define  TideSanDiegoSanDiegoBaylib_h 
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