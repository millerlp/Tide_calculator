/*  TidelibSanJuanLaPuntillaSanJuanBayPuertoRico.h 
  A library for calculating the current tide height at 
   San Juan, La Puntilla, San Juan Bay, Puerto Rico 
  Luke Miller, 2015-06-03 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibSanJuanLaPuntillaSanJuanBayPuertoRico_h 
#define  TidelibSanJuanLaPuntillaSanJuanBayPuertoRico_h 
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