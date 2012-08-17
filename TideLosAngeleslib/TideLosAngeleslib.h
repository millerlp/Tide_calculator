/*  TideLosAngeleslib.h 
  A library for calculating the current tide height at 
   Los Angeles (outer harbor), California 
  Luke Miller, June 2012
*/ 
 
#ifndef  TideLosAngeleslib_h 
#define  TideLosAngeleslib_h 
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