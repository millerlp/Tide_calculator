/*  TidelibAnnapolisUSNavalAcademySevernRiverMaryland.h 
  A library for calculating the current tide height at 
   Annapolis (US Naval Academy), Severn River, Maryland 
  Luke Miller, 2015-06-03 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibAnnapolisUSNavalAcademySevernRiverMaryland_h 
#define  TidelibAnnapolisUSNavalAcademySevernRiverMaryland_h 
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