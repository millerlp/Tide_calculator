/*  TidelibYaquinaBayandRiverOregon.h 
  A library for calculating the current tide height at 
   South Beach, Yaquina Bay and River, Oregon 
  Luke Miller, 2015-03-27 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibYaquinaBayandRiverOregon_h 
#define  TidelibYaquinaBayandRiverOregon_h 
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