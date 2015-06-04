/*  TidelibSanDiegoSanDiegoBay.cpp 
 This source file contains a tide calculation function for the site listed
 below. This file and the associated header file should be placed in the
 Ardiuno/libraries/ directory inside a single folder.
 Luke Miller, 2015-06-04 
 http://github.com/millerlp/Tide_calculator
 Released under the GPL version 3 license.
 Compiled for Arduino v1.6.4 circa 2015

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
#include "TidelibSanDiegoSanDiegoBay.h"

unsigned int YearIndx = 0; // Used to index rows in the Equilarg/Nodefactor arrays
float currHours = 0;          // Elapsed hours since start of year
const int adjustGMT = 8;     // Time zone adjustment to get time in GMT.
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

// Selected station: San Diego, San Diego Bay, California
char stationID[] = "San Diego, San Diego Bay, California";
// Selection station ID number: 9410170
const long stationIDnumber = 9410170;
// The 'datum' printed here is the difference between mean sea level and 
// mean lower low water for the NOAA station. These two values can be 
// found for NOAA tide reference stations on the tidesandcurrents.noaa.gov
//  site under the datum page for each station.
const float Datum = 2.9396 ; // units in feet
// Harmonic constant names: J1, K1, K2, L2, M1, M2, M3, M4, M6, M8, N2, 2N2, O1, OO1, P1, Q1, 2Q1, R2, S1, S2, S4, S6, T2, LDA2, MU2, NU2, RHO1, MK3, 2MK3, MN4, MS4, 2SM2, MF, MSF, MM, SA, SSA
// These names match the NOAA names, except LDA2 here is LAM2 on NOAA's site
typedef float PROGMEM prog_float_t; // Need to define this type before use
// Amp is the amplitude of each of the harmonic constituents for this site
const prog_float_t Amp[] PROGMEM = {0.066,1.137,0.221,0.049,0.043,1.824,0.011,0,0.014,0,0.426,0.053,0.723,0.036,0.357,0.135,0.014,0.006,0.013,0.751,0,0,0.047,0.01,0.051,0.083,0.028,0,0,0,0,0,0,0,0,0.227,0};
// Kappa is the 'modified' or 'adapted' phase lag (Epoch) of each of the 
// harmonic constituents for this site.
const prog_float_t Kappa[] PROGMEM = {220.6,208,134.3,144,220.4,143.2,346.1,0,61.4,0,123.8,97.5,192.4,241.1,205.5,184.7,182.1,140.1,338.8,140.2,0,0,127.3,128.8,93,128.8,184.3,0,0,0,0,0,0,0,0,179.3,0};
// Speed is the frequency of the constituent, denoted as little 'a' by Hicks 2006
const prog_float_t Speed[] PROGMEM = {15.58544,15.04107,30.08214,29.52848,14.49669,28.9841,43.47616,57.96821,86.95231,115.9364,28.43973,27.89535,13.94304,16.1391,14.95893,13.39866,12.85429,30.04107,15,30,60,90,29.95893,29.45563,27.96821,28.51258,13.47151,44.02517,42.92714,57.42383,58.9841,31.0159,1.098033,1.015896,0.5443747,0.0410686,0.0821373};
const prog_float_t Equilarg[10][37] PROGMEM = { 
{83.38,11.3,202.4,3.66,322.99,110.42,165.63,220.84,331.26,81.68,38.87,327.32,98.58,105.09,349.67,27.03,315.49,177.14,180,0,0,0,2.86,108.65,220.64,292.19,280.36,121.72,209.54,149.29,110.42,249.58,93.25,249.58,71.55,280.33,200.67},
{166.42,7.56,195.61,182.09,211.44,210.46,135.69,60.92,271.39,121.85,50.19,249.92,204.31,348,349.9,44.04,243.77,176.88,180,0,0,0,3.12,19.21,61.44,221.71,215.56,218.02,53.36,260.65,210.46,149.54,341.84,149.54,160.27,280.1,200.19},
{264.33,5.26,191.35,1.96,102.4,286.18,69.27,212.36,138.54,64.72,24.12,122.06,283.91,260.62,349.16,21.85,119.79,177.61,180,0,0,0,2.39,276.77,213.53,115.59,113.32,291.44,207.11,310.3,286.18,73.82,258.36,73.82,262.06,280.84,201.69},
{349.75,2.87,186.44,202.17,42.75,26.41,39.61,52.82,79.23,105.64,35.63,44.85,27.45,150.48,349.4,36.66,45.88,177.35,180,0,0,0,2.65,187.53,54.51,45.29,46.33,29.28,49.95,62.04,26.41,333.59,151.52,333.59,350.78,280.6,201.21},
{76.92,1.58,183.41,39.83,314.62,126.83,10.24,253.65,20.48,147.31,47.32,327.82,129.41,45.42,349.63,49.91,330.4,177.1,180,0,0,0,2.9,98.47,255.68,335.19,337.77,128.41,252.07,174.15,126.83,233.17,48,233.17,79.5,280.37,200.73},
{165.67,1.37,182.52,211.35,206.58,227.47,341.21,94.94,322.41,189.88,59.24,251.02,230.02,304.87,349.87,61.8,253.57,176.84,180,0,0,0,3.16,9.64,97.08,265.3,267.86,228.84,93.57,286.71,227.47,132.53,307.42,132.53,168.23,280.13,200.25},
{269.75,3.07,185.62,17.26,90.25,303.97,275.96,247.95,191.92,135.9,33.96,123.94,304.24,235.22,349.13,34.23,124.21,177.57,180,0,0,0,2.43,267.98,249.95,159.97,160.24,307.04,244.88,337.93,303.97,56.03,235.49,56.03,270.01,280.87,201.75},
{0.7,4.49,188.41,224.35,24.63,45.09,247.63,90.18,135.26,180.35,46.35,47.61,43.13,140.79,349.36,44.39,45.65,177.32,180,0,0,0,2.68,179.62,91.82,90.56,88.6,49.58,85.68,91.44,45.09,314.91,138.83,314.91,358.74,280.64,201.27},
{92.29,6.43,192.41,74.18,317.02,146.39,219.58,292.78,79.17,225.56,58.93,331.47,141.56,48.09,349.6,54.1,326.64,177.06,180,0,0,0,2.94,91.44,293.87,21.33,16.51,152.82,286.35,205.32,146.39,213.61,43.26,213.61,87.46,280.4,200.79},
{184.25,8.66,197.15,246.07,213.12,247.82,191.73,135.64,23.46,271.27,71.64,255.45,239.75,316.39,349.84,63.57,247.38,176.8,180,0,0,0,3.2,3.4,136.06,312.24,304.17,256.48,126.97,319.45,247.82,112.18,308.32,112.18,176.18,280.16,200.32} 
 };

const prog_float_t Nodefactor[10][37] PROGMEM = { 
{0.8278,0.8824,0.7472,0.878,1.5575,1.0377,1.0571,1.0768,1.1173,1.1594,1.0377,1.0377,0.8068,0.4868,1,0.8068,0.8068,1,1,1,1,1,1,1.0377,1.0377,1.0377,0.8068,0.9156,0.9501,1.0768,1.0377,1.0377,0.6271,1.0377,1.1307,1,1},
{0.8343,0.8864,0.7533,0.9704,1.4048,1.0367,1.0556,1.0747,1.1141,1.155,1.0367,1.0367,0.8135,0.5,1,0.8135,0.8135,1,1,1,1,1,1,1.0367,1.0367,1.0367,0.8135,0.9189,0.9526,1.0747,1.0367,1.0367,0.6381,1.0367,1.1272,1,1},
{0.8669,0.9068,0.7865,1.1656,0.9653,1.0315,1.0477,1.0641,1.0977,1.1323,1.0315,1.0315,0.8475,0.5711,1,0.8475,0.8475,1,1,1,1,1,1,1.0315,1.0315,1.0315,0.8475,0.9354,0.965,1.0641,1.0315,1.0315,0.6961,1.0315,1.109,1,1},
{0.9176,0.9394,0.8458,1.204,0.9343,1.0229,1.0345,1.0463,1.0702,1.0947,1.0229,1.0229,0.9011,0.6981,1,0.9011,0.9011,1,1,1,1,1,1,1.0229,1.0229,1.0229,0.9011,0.9609,0.9829,1.0463,1.0229,1.0229,0.7936,1.0229,1.0783,1,1},
{0.9761,0.9782,0.9272,0.9582,1.6115,1.0117,1.0176,1.0235,1.0354,1.0475,1.0117,1.0117,0.9643,0.8745,1,0.9643,0.9643,1,1,1,1,1,1,1.0117,1.0117,1.0117,0.9643,0.9897,1.0012,1.0235,1.0117,1.0117,0.9188,1.0117,1.039,1,1},
{1.0336,1.0176,1.0225,0.7337,1.9813,0.9992,0.9989,0.9985,0.9977,0.9969,0.9992,0.9992,1.0279,1.0859,1,1.0279,1.0279,1,1,1,1,1,1,0.9992,0.9992,0.9992,1.0279,1.0168,1.016,0.9985,0.9992,0.9992,1.0571,0.9992,0.9955,1,1},
{1.0836,1.0529,1.1201,1.0649,1.5936,0.987,0.9805,0.9741,0.9614,0.9489,0.987,0.987,1.085,1.309,1,1.085,1.085,1,1,1,1,1,1,0.987,0.987,0.987,1.085,1.0392,1.0257,0.9741,0.987,0.987,1.1924,0.987,0.953,1,1},
{1.1226,1.0812,1.2075,1.3148,1.0585,0.9763,0.9646,0.9531,0.9305,0.9084,0.9763,0.9763,1.131,1.5151,1,1.131,1.131,1,1,1,1,1,1,0.9763,0.9763,0.9763,1.131,1.0555,1.0305,0.9531,0.9763,0.9763,1.3097,0.9763,0.9161,1,1},
{1.1492,1.1009,1.2736,1.0393,1.7582,0.9683,0.9528,0.9376,0.9079,0.8791,0.9683,0.9683,1.1631,1.6753,1,1.1631,1.1631,1,1,1,1,1,1,0.9683,0.9683,0.9683,1.1631,1.066,1.0322,0.9376,0.9683,0.9683,1.3967,0.9683,0.8888,1,1},
{1.163,1.1112,1.3104,0.5914,2.2831,0.9639,0.9464,0.9291,0.8956,0.8633,0.9639,0.9639,1.1801,1.7658,1,1.1801,1.1801,1,1,1,1,1,1,0.9639,0.9639,0.9639,1.1801,1.0711,1.0324,0.9291,0.9639,0.9639,1.4444,0.9639,0.8738,1,1} 
 };

// Define unix time values for the start of each year.
//                                      2015       2016       2017       2018       2019       2020       2021       2022       2023       2024
const unsigned long startSecs[] PROGMEM = {1420070400,1451606400,1483228800,1514764800,1546300800,1577836800,1609459200,1640995200,1672531200,1704067200};

// 1st year of data in the Equilarg/Nodefactor/startSecs arrays.
const unsigned int startYear = 2015;
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
