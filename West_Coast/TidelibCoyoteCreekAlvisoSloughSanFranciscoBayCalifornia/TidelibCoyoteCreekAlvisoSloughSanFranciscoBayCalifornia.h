/*  TidelibCoyoteCreekAlvisoSloughSanFranciscoBayCalifornia.h 
  A library for calculating the current tide height at 
   Coyote Creek, Alviso Slough, San Francisco Bay, California 
  Luke Miller, 2015-06-03 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibCoyoteCreekAlvisoSloughSanFranciscoBayCalifornia_h 
#define  TidelibCoyoteCreekAlvisoSloughSanFranciscoBayCalifornia_h 
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