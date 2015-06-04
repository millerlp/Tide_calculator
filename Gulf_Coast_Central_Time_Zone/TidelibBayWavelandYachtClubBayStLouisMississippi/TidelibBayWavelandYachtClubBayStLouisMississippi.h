/*  TidelibBayWavelandYachtClubBayStLouisMississippi.h 
  A library for calculating the current tide height at 
   Bay Waveland Yacht Club, Bay St. Louis, Mississippi 
  Luke Miller, 2015-06-03 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibBayWavelandYachtClubBayStLouisMississippi_h 
#define  TidelibBayWavelandYachtClubBayStLouisMississippi_h 
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