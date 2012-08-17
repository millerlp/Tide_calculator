/*  TideMontereyHarborlib.h 
  A library for calculating the current tide height at 
   Monterey, Monterey Harbor, California 
  Luke Miller, June 2012
*/ 
 
#ifndef  TideMontereyHarborlib_h 
#define  TideMontereyHarborlib_h 
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