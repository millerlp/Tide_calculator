/*  TidelibCapeCharlesHarborUSCGWharfChesapeakeBayVirginia.h 
  A library for calculating the current tide height at 
   Cape Charles Harbor (USCG Wharf), Chesapeake Bay, Virginia 
  Luke Miller, 2015-06-03 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibCapeCharlesHarborUSCGWharfChesapeakeBayVirginia_h 
#define  TidelibCapeCharlesHarborUSCGWharfChesapeakeBayVirginia_h 
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