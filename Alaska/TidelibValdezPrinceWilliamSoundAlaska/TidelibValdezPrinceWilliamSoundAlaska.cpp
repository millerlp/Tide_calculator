/*  TidelibValdezPrinceWilliamSoundAlaska.cpp 
 This source file contains a tide calculation function for the site listed
 below. This file and the associated header file should be placed in the
 Ardiuno/libraries/ directory inside a single folder.
 Luke Miller, 2019-02-07 
 http://github.com/millerlp/Tide_calculator
 Released under the GPL version 3 license.
 Compiled for Arduino v1.8.8 circa 2019

 The harmonic constituents used here were originally derived from 
 the Center for Operational Oceanic Products and Services (CO-OPS),
 National Ocean Service (NOS), National Oceanic and Atmospheric 
 Administration, U.S.A.
 The data were originally processed by David Flater for use with XTide,
 available at http://www.flaterco.com/xtide/files.html
 The predictions from this program should not be used for navigation
 and no accuracy or warranty is given or implied for these tide predictions.
 */
#include <Arduino.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include "RTClib.h" // https://github.com/millerlp/RTClib
#include "TidelibValdezPrinceWilliamSoundAlaska.h"

unsigned int YearIndx = 0; // Used to index rows in the Equilarg/Nodefactor arrays
float currHours = 0;          // Elapsed hours since start of year
const int adjustGMT = 9;     // Time zone adjustment to get time in GMT.
//Make sure this is correct for the local standard time of the tide station.
// 8 = Pacific Standard Time (America/Los_Angeles)
/* Initialize harmonic constituent arrays. These each hold 37 values for
the tide site that was extracted using the R scripts:
tide_harmonics_parse.R and tide_harmonics_library_generator.R

The values are available from NOAA's http://tidesandcurrent.noaa.gov site.
Kappa here is referred to as 'Phase' on NOAA's site. The order of the
constituents is shown below in the names. Unfortunately this does not match
NOAA's order, so you will have to rearrange NOAA's values if you want to
put new site values in here by hand.
The Speed, Equilarg and Nodefactor arrays can all stay the same for any site.
*/

// Selected station: Valdez, Prince William Sound, Alaska
char stationID[] = "Valdez, Prince William Sound, Alaska";
// Selection station ID number: 9454240
const long stationIDnumber = 9454240;
// The 'datum' printed here is the difference between mean sea level and 
// mean lower low water for the NOAA station. These two values can be 
// found for NOAA tide reference stations on the tidesandcurrents.noaa.gov
//  site under the datum page for each station.
const float Datum = 6.5 ; // units in feet
// Harmonic constant names: J1, K1, K2, L2, M1, M2, M3, M4, M6, M8, N2, 2N2, O1, OO1, P1, Q1, 2Q1, R2, S1, S2, S4, S6, T2, LDA2, MU2, NU2, RHO1, MK3, 2MK3, MN4, MS4, 2SM2, MF, MSF, MM, SA, SSA
// These names match the NOAA names, except LDA2 here is LAM2 on NOAA's site
typedef float PROGMEM prog_float_t; // Need to define this type before use
// Amp is the amplitude of each of the harmonic constituents for this site
const prog_float_t Amp[] PROGMEM = {0.095,1.64,0.43,0.102,0.052,4.573,0.023,0.154,0.01,0,0.942,0.112,0.994,0.052,0.531,0.174,0.023,0.02,0.02,1.572,0.039,0,0.092,0.03,0.108,0.177,0.03,0,0,0.039,0.115,0.01,0.085,0,0,0.367,0};
// Kappa is the 'modified' or 'adapted' phase lag (Epoch) of each of the 
// harmonic constituents for this site.
const prog_float_t Kappa[] PROGMEM = {282.2,272,313.8,300.1,286.8,287.7,273.6,19.4,197.6,0,263.1,237.9,256,298.1,269.7,249.4,249.6,9.6,359.1,320.4,351.1,0,306.2,303.5,244.4,267.6,251.8,0,0,338.4,103.4,136.1,176.5,0,0,243.1,0};
// Speed is the frequency of the constituent, denoted as little 'a' by Hicks 2006
const prog_float_t Speed[] PROGMEM = {15.58544,15.04107,30.08214,29.52848,14.49669,28.9841,43.47616,57.96821,86.95231,115.9364,28.43973,27.89535,13.94304,16.1391,14.95893,13.39866,12.85429,30.04107,15,30,60,90,29.95893,29.45563,27.96821,28.51258,13.47151,44.02517,42.92714,57.42383,58.9841,31.0159,1.098033,1.015896,0.5443747,0.0410686,0.0821373};
const prog_float_t Equilarg[10][37] PROGMEM = { 
{76.92,1.58,183.41,39.83,314.62,126.83,10.24,253.65,20.48,147.31,47.32,327.82,129.41,45.42,349.63,49.91,330.4,177.1,180,0,0,0,2.9,98.47,255.68,335.19,337.77,128.41,252.07,174.15,126.83,233.17,48,233.17,79.5,280.37,200.73},
{165.67,1.37,182.52,211.35,206.58,227.47,341.21,94.94,322.41,189.88,59.24,251.02,230.02,304.87,349.87,61.8,253.57,176.84,180,0,0,0,3.16,9.64,97.08,265.3,267.86,228.84,93.57,286.71,227.47,132.53,307.42,132.53,168.23,280.13,200.25},
{269.75,3.07,185.62,17.26,90.25,303.97,275.96,247.95,191.92,135.9,33.96,123.94,304.24,235.22,349.13,34.23,124.21,177.57,180,0,0,0,2.43,267.98,249.95,159.97,160.24,307.04,244.88,337.93,303.97,56.03,235.49,56.03,270.01,280.87,201.75},
{0.7,4.49,188.41,224.35,24.63,45.09,247.63,90.18,135.26,180.35,46.35,47.61,43.13,140.79,349.36,44.39,45.65,177.32,180,0,0,0,2.68,179.62,91.82,90.56,88.6,49.58,85.68,91.44,45.09,314.91,138.83,314.91,358.74,280.64,201.27},
{92.29,6.43,192.41,74.18,317.02,146.39,219.58,292.78,79.17,225.56,58.93,331.47,141.56,48.09,349.6,54.1,326.64,177.06,180,0,0,0,2.94,91.44,293.87,21.33,16.51,152.82,286.35,205.32,146.39,213.61,43.26,213.61,87.46,280.4,200.79},
{184.25,8.66,197.15,246.07,213.12,247.82,191.73,135.64,23.46,271.27,71.64,255.45,239.75,316.39,349.84,63.57,247.38,176.8,180,0,0,0,3.2,3.4,136.06,312.24,304.17,256.48,126.97,319.45,247.82,112.18,308.32,112.18,176.18,280.16,200.32},
{290.4,12,204.13,41.02,96.21,324.92,127.37,289.83,254.75,219.66,46.94,128.97,312.48,252.38,349.1,34.51,116.54,177.53,180,0,0,0,2.47,262.34,289.52,207.49,195.06,336.91,277.83,11.86,324.92,35.08,239.95,35.08,277.97,280.9,201.81},
{22.4,14.26,208.93,250.96,25.28,66.36,99.53,132.71,199.07,265.43,59.66,52.97,50.65,160.77,349.33,43.96,37.26,177.28,180,0,0,0,2.72,174.3,131.72,138.41,122.7,80.62,118.45,126.02,66.36,293.64,145.06,293.64,6.69,280.67,201.33},
{114.06,16.25,213.08,101.09,323.67,167.68,71.52,335.37,143.05,310.73,72.27,336.85,149.03,68.26,349.57,53.62,318.2,177.02,180,0,0,0,2.98,86.15,333.8,69.21,50.56,183.94,319.11,239.95,167.68,192.32,49.61,192.32,95.42,280.43,200.85},
{205.14,17.78,216.09,279.46,220.94,268.83,43.24,177.66,86.49,355.32,84.69,260.55,247.83,334.17,349.81,63.69,239.55,176.77,180,0,0,0,3.23,357.82,175.7,359.84,338.83,286.61,159.88,353.52,268.83,91.17,313.17,91.17,184.14,280.19,200.38} 
 };

const prog_float_t Nodefactor[10][37] PROGMEM = { 
{0.9761,0.9782,0.9272,0.9582,1.6115,1.0117,1.0176,1.0235,1.0354,1.0475,1.0117,1.0117,0.9643,0.8745,1,0.9643,0.9643,1,1,1,1,1,1,1.0117,1.0117,1.0117,0.9643,0.9897,1.0012,1.0235,1.0117,1.0117,0.9188,1.0117,1.039,1,1},
{1.0336,1.0176,1.0225,0.7337,1.9813,0.9992,0.9989,0.9985,0.9977,0.9969,0.9992,0.9992,1.0279,1.0859,1,1.0279,1.0279,1,1,1,1,1,1,0.9992,0.9992,0.9992,1.0279,1.0168,1.016,0.9985,0.9992,0.9992,1.0571,0.9992,0.9955,1,1},
{1.0836,1.0529,1.1201,1.0649,1.5936,0.987,0.9805,0.9741,0.9614,0.9489,0.987,0.987,1.085,1.309,1,1.085,1.085,1,1,1,1,1,1,0.987,0.987,0.987,1.085,1.0392,1.0257,0.9741,0.987,0.987,1.1924,0.987,0.953,1,1},
{1.1226,1.0812,1.2075,1.3148,1.0585,0.9763,0.9646,0.9531,0.9305,0.9084,0.9763,0.9763,1.131,1.5151,1,1.131,1.131,1,1,1,1,1,1,0.9763,0.9763,0.9763,1.131,1.0555,1.0305,0.9531,0.9763,0.9763,1.3097,0.9763,0.9161,1,1},
{1.1492,1.1009,1.2736,1.0393,1.7582,0.9683,0.9528,0.9376,0.9079,0.8791,0.9683,0.9683,1.1631,1.6753,1,1.1631,1.1631,1,1,1,1,1,1,0.9683,0.9683,0.9683,1.1631,1.066,1.0322,0.9376,0.9683,0.9683,1.3967,0.9683,0.8888,1,1},
{1.163,1.1112,1.3104,0.5914,2.2831,0.9639,0.9464,0.9291,0.8956,0.8633,0.9639,0.9639,1.1801,1.7658,1,1.1801,1.1801,1,1,1,1,1,1,0.9639,0.9639,0.9639,1.1801,1.0711,1.0324,0.9291,0.9639,0.9639,1.4444,0.9639,0.8738,1,1},
{1.164,1.112,1.3131,1.0008,1.8466,0.9636,0.9459,0.9285,0.8947,0.8621,0.9636,0.9636,1.1814,1.7726,1,1.1814,1.1814,1,1,1,1,1,1,0.9636,0.9636,0.9636,1.1814,1.0715,1.0324,0.9285,0.9636,0.9636,1.4479,0.9636,0.8727,1,1},
{1.1522,1.1031,1.2815,1.3288,1.0994,0.9674,0.9515,0.9358,0.9052,0.8757,0.9674,0.9674,1.1668,1.6946,1,1.1668,1.1668,1,1,1,1,1,1,0.9674,0.9674,0.9674,1.1668,1.0671,1.0323,0.9358,0.9674,0.9674,1.4069,0.9674,0.8856,1,1},
{1.1276,1.0849,1.2196,1.091,1.6268,0.9748,0.9625,0.9502,0.9263,0.903,0.9748,0.9748,1.137,1.544,1,1.137,1.137,1,1,1,1,1,1,0.9748,0.9748,0.9748,1.137,1.0576,1.0309,0.9502,0.9748,0.9748,1.3257,0.9748,0.9111,1,1},
{1.0904,1.0578,1.1347,0.6859,2.1026,0.9852,0.9779,0.9705,0.9561,0.942,0.9852,0.9852,1.093,1.3431,1,1.093,1.093,1,1,1,1,1,1,0.9852,0.9852,0.9852,1.093,1.0422,1.0267,0.9705,0.9852,0.9852,1.2123,0.9852,0.9468,1,1} 
 };

// Define unix time values for the start of each year.
//                                      2019       2020       2021       2022       2023       2024       2025       2026       2027       2028
const unsigned long startSecs[] PROGMEM = {1546300800,1577836800,1609459200,1640995200,1672531200,1704067200,1735689600,1767225600,1798761600,1830297600};

// 1st year of data in the Equilarg/Nodefactor/startSecs arrays.
const unsigned int startYear = 2019;
//------------------------------------------------------------------
// Define some variables that will hold extract values from the arrays above
float currAmp, currSpeed, currNodefactor, currEquilarg, currKappa, tideHeight;

// Constructor function, doesn't do anything special
TideCalc::TideCalc(void){}

// Return tide station name
char* TideCalc::returnStationID(void){
    return stationID;
}

// Return NOAA tide station ID number
long TideCalc::returnStationIDnumber(void){
    return stationIDnumber;
}

// currentTide calculation function, takes a DateTime object from real time clock
float TideCalc::currentTide(DateTime now) {
	// Calculate difference between current year and starting year.
	YearIndx = now.year() - startYear;
 	// Calculate hours since start of current year. Hours = seconds / 3600
	currHours = (now.unixtime() - pgm_read_dword_near(&startSecs[YearIndx])) / float(3600);
   // Shift currHours to Greenwich Mean Time
   currHours = currHours + adjustGMT;
   // *****************Calculate current tide height*************
   tideHeight = Datum; // initialize results variable, units of feet.
   for (int harms = 0; harms < 37; harms++) {
       // Step through each harmonic constituent, extract the relevant
       // values of Nodefactor, Amplitude, Equilibrium argument, Kappa
       // and Speed.
       currNodefactor = pgm_read_float_near(&Nodefactor[YearIndx][harms]);
 		currAmp = pgm_read_float_near(&Amp[harms]);
       currEquilarg = pgm_read_float_near(&Equilarg[YearIndx][harms]);
       currKappa = pgm_read_float_near(&Kappa[harms]);
       currSpeed = pgm_read_float_near(&Speed[harms]);
    // Calculate each component of the overall tide equation
    // The currHours value is assumed to be in hours from the start of the
    // year, in the Greenwich Mean Time zone, not the local time zone.
       tideHeight = tideHeight + (currNodefactor * currAmp *
           cos( (currSpeed * currHours + currEquilarg - currKappa) * DEG_TO_RAD));
    }
    //******************End of Tide Height calculation*************
    return tideHeight;  // Output of tideCalc is the tide height, units of feet
}
