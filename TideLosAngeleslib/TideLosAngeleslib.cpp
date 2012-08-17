/*  TideLosAngeleslib.cpp 
 This source file contains a tide calculation function for the site listed
 below. This file and the associated header file should be placed in the
 Ardiuno/libraries/ directory inside a single folder.
 Luke Miller, June 2012
 Released under the GPL version 3 license.
 The harmonic constituents used here were originally derived from 
 XTide, available at http://www.flaterco.com/xtide/files.html
 The predictions from this program should not be used for navigation
 and no accuracy or warranty is given or implied for these tide predictions.
 */
#include <Arduino.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include "RTClib.h"
#include "TideLosAngeleslib.h"

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

// Selected station:  Los Angeles (outer harbor), California 
char stationID[] = " Los Angeles (outer harbor), California ";
// The 'datum' printed here is the difference between mean sea level and 
// mean lower low water for the NOAA station. These two values can be 
// found for NOAA tide reference stations on the tidesandcurrents.noaa.gov
//  site under the datum page for each station.
const float Datum = 2.8248 ; // units in feet
// Harmonic constant names: J1, K1, K2, L2, M1, M2, M3, M4, M6, M8, N2, 2N2, O1, OO1, P1, Q1, 2Q1, R2, S1, S2, S4, S6, T2, LDA2, MU2, NU2, RHO1, MK3, 2MK3, MN4, MS4, 2SM2, MF, MSF, MM, SA, SSA
// These names match the NOAA names, except LDA2 here is LAM2 on NOAA's site
typedef float PROGMEM prog_float_t; // Need to define this type before use
PROGMEM prog_float_t Amp[] = {0.064,1.124,0.196,0.026,0.039,1.691,0.01,0,0,0,0.396,0.05,0.714,0.035,0.352,0.129,0.013,0.01,0.01,0.666,0,0,0.04,0.01,0.055,0.077,0.025,0,0,0,0,0,0,0,0,0.218,0};
PROGMEM prog_float_t Kappa[] = {219.1,207.7,135.5,134.2,222.6,145.5,350.1,0,0,0,123.7,95.1,192.3,236.9,204.6,185,186.1,167.5,314.1,141.1,0,0,129.7,166.4,88.1,130.2,182.1,0,0,0,0,0,0,0,0,184.4,0};
PROGMEM prog_float_t Speed[] = {15.58544,15.04107,30.08214,29.52848,14.49669,28.9841,43.47616,57.96821,86.95231,115.9364,28.43973,27.89535,13.94304,16.1391,14.95893,13.39866,12.85429,30.04107,15,30,60,90,29.95893,29.45563,27.96821,28.51258,13.47151,44.02517,42.92714,57.42383,58.9841,31.0159,1.098033,1.015896,0.5443747,0.0410686,0.0821373};
PROGMEM prog_float_t Equilarg[10][37] = { 
{174.95,18.51,216.55,157.82,231.31,194.25,291.37,28.5,222.75,57,41.93,249.62,171.62,53.64,349.93,19.3,226.99,176.92,180,0,0,0,3.08,29.59,26.6,178.91,156.28,212.76,9.99,236.18,194.25,165.75,31.01,165.75,152.32,280.07,200.13},
{275.37,17.7,214.57,348.14,159.57,270.19,225.29,180.38,90.58,0.77,16.09,121.98,248.92,333.61,349.19,354.82,100.72,177.65,180,0,0,0,2.35,287.37,178.91,73.02,51.75,287.89,162.69,286.28,270.19,89.81,312.35,89.81,254.1,280.81,201.62},
{0.02,14.86,208.97,182.74,72.78,10.35,195.53,20.71,31.06,41.42,27.53,44.7,353.16,221.23,349.43,10.33,27.51,177.39,180,0,0,0,2.61,198.06,19.83,2.65,345.46,25.21,5.85,37.88,10.35,349.65,204.04,349.65,342.83,280.57,201.15},
{83.38,11.3,202.4,3.66,322.99,110.42,165.63,220.84,331.26,81.68,38.87,327.32,98.58,105.09,349.67,27.03,315.49,177.14,180,0,0,0,2.86,108.65,220.64,292.19,280.36,121.72,209.54,149.29,110.42,249.58,93.25,249.58,71.55,280.33,200.67},
{166.42,7.56,195.61,182.09,211.44,210.46,135.69,60.92,271.39,121.85,50.19,249.92,204.31,348,349.9,44.04,243.77,176.88,180,0,0,0,3.12,19.21,61.44,221.71,215.56,218.02,53.36,260.65,210.46,149.54,341.84,149.54,160.27,280.1,200.19},
{264.33,5.26,191.35,1.96,102.4,286.18,69.27,212.36,138.54,64.72,24.12,122.06,283.91,260.62,349.16,21.85,119.79,177.61,180,0,0,0,2.39,276.77,213.53,115.59,113.32,291.44,207.11,310.3,286.18,73.82,258.36,73.82,262.06,280.84,201.69},
{349.75,2.87,186.44,202.17,42.75,26.41,39.61,52.82,79.23,105.64,35.63,44.85,27.45,150.48,349.4,36.66,45.88,177.35,180,0,0,0,2.65,187.53,54.51,45.29,46.33,29.28,49.95,62.04,26.41,333.59,151.52,333.59,350.78,280.6,201.21},
{76.92,1.58,183.41,39.83,314.62,126.83,10.24,253.65,20.48,147.31,47.32,327.82,129.41,45.42,349.63,49.91,330.4,177.1,180,0,0,0,2.9,98.47,255.68,335.19,337.77,128.41,252.07,174.15,126.83,233.17,48,233.17,79.5,280.37,200.73},
{165.67,1.37,182.52,211.35,206.58,227.47,341.21,94.94,322.41,189.88,59.24,251.02,230.02,304.87,349.87,61.8,253.57,176.84,180,0,0,0,3.16,9.64,97.08,265.3,267.86,228.84,93.57,286.71,227.47,132.53,307.42,132.53,168.23,280.13,200.25},
{269.75,3.07,185.62,17.26,90.25,303.97,275.96,247.95,191.92,135.9,33.96,123.94,304.24,235.22,349.13,34.23,124.21,177.57,180,0,0,0,2.43,267.98,249.95,159.97,160.24,307.04,244.88,337.93,303.97,56.03,235.49,56.03,270.01,280.87,201.75} 
 };

PROGMEM prog_float_t Nodefactor[10][37] = { 
{0.9491,0.9602,0.8878,1.1647,1.1232,1.017,1.0257,1.0343,1.0519,1.0698,1.017,1.017,0.9349,0.7887,1,0.9349,0.9349,1,1,1,1,1,1,1.017,1.017,1.017,0.9349,0.9765,0.9931,1.0343,1.017,1.017,0.8592,1.017,1.0577,1,1},
{0.8928,0.9234,0.8156,1.2048,0.878,1.0272,1.0411,1.0552,1.0839,1.1134,1.0272,1.0272,0.8748,0.6334,1,0.8748,0.8748,1,1,1,1,1,1,1.0272,1.0272,1.0272,0.8748,0.9485,0.9743,1.0552,1.0272,1.0272,0.7448,1.0272,1.0937,1,1},
{0.8492,0.8957,0.768,1.0216,1.3201,1.0344,1.052,1.0699,1.1067,1.1448,1.0344,1.0344,0.829,0.5315,1,0.829,0.829,1,1,1,1,1,1,1.0344,1.0344,1.0344,0.829,0.9265,0.9583,1.0699,1.0344,1.0344,0.6642,1.0344,1.119,1,1},
{0.8278,0.8824,0.7472,0.878,1.5575,1.0377,1.0571,1.0768,1.1173,1.1594,1.0377,1.0377,0.8068,0.4868,1,0.8068,0.8068,1,1,1,1,1,1,1.0377,1.0377,1.0377,0.8068,0.9156,0.9501,1.0768,1.0377,1.0377,0.6271,1.0377,1.1307,1,1},
{0.8343,0.8864,0.7533,0.9704,1.4048,1.0367,1.0556,1.0747,1.1141,1.155,1.0367,1.0367,0.8135,0.5,1,0.8135,0.8135,1,1,1,1,1,1,1.0367,1.0367,1.0367,0.8135,0.9189,0.9526,1.0747,1.0367,1.0367,0.6381,1.0367,1.1272,1,1},
{0.8669,0.9068,0.7865,1.1656,0.9653,1.0315,1.0477,1.0641,1.0977,1.1323,1.0315,1.0315,0.8475,0.5711,1,0.8475,0.8475,1,1,1,1,1,1,1.0315,1.0315,1.0315,0.8475,0.9354,0.965,1.0641,1.0315,1.0315,0.6961,1.0315,1.109,1,1},
{0.9176,0.9394,0.8458,1.204,0.9343,1.0229,1.0345,1.0463,1.0702,1.0947,1.0229,1.0229,0.9011,0.6981,1,0.9011,0.9011,1,1,1,1,1,1,1.0229,1.0229,1.0229,0.9011,0.9609,0.9829,1.0463,1.0229,1.0229,0.7936,1.0229,1.0783,1,1},
{0.9761,0.9782,0.9272,0.9582,1.6115,1.0117,1.0176,1.0235,1.0354,1.0475,1.0117,1.0117,0.9643,0.8745,1,0.9643,0.9643,1,1,1,1,1,1,1.0117,1.0117,1.0117,0.9643,0.9897,1.0012,1.0235,1.0117,1.0117,0.9188,1.0117,1.039,1,1},
{1.0336,1.0176,1.0225,0.7337,1.9813,0.9992,0.9989,0.9985,0.9977,0.9969,0.9992,0.9992,1.0279,1.0859,1,1.0279,1.0279,1,1,1,1,1,1,0.9992,0.9992,0.9992,1.0279,1.0168,1.016,0.9985,0.9992,0.9992,1.0571,0.9992,0.9955,1,1},
{1.0836,1.0529,1.1201,1.0649,1.5936,0.987,0.9805,0.9741,0.9614,0.9489,0.987,0.987,1.085,1.309,1,1.085,1.085,1,1,1,1,1,1,0.987,0.987,0.987,1.085,1.0392,1.0257,0.9741,0.987,0.987,1.1924,0.987,0.953,1,1} 
 };

// Define unix time values for the start of each year.
//                                      2012       2013       2014       2015       2016       2017       2018       2019       2020       2021
PROGMEM prog_uint32_t startSecs[] = {1325376000,1356998400,1388534400,1420070400,1451606400,1483228800,1514764800,1546300800,1577836800,1609459200};

// 1st year of data in the Equilarg/Nodefactor/startSecs arrays.
const unsigned int startYear = 2012;
//------------------------------------------------------------------
// Define some variables that will hold extract values from the arrays above
float currAmp, currSpeed, currNodefactor, currEquilarg, currKappa, tideHeight;

// Constructor function, doesn't do anything special
TideCalc::TideCalc(void){}

// Return tide station name
char* TideCalc::returnStationID(void){
    return stationID;
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
