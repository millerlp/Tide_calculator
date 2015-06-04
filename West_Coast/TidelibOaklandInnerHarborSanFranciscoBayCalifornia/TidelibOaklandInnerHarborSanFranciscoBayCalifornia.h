/*  TidelibOaklandInnerHarborSanFranciscoBayCalifornia.h 
  A library for calculating the current tide height at 
   Oakland Inner Harbor, San Francisco Bay, California 
  Luke Miller, 2015-06-03 
  https://github.com/millerlp/Tide_calculator
*/ 
 
#ifndef  TidelibOaklandInnerHarborSanFranciscoBayCalifornia_h 
#define  TidelibOaklandInnerHarborSanFranciscoBayCalifornia_h 
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