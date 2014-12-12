/*
	Code contributed by Robert Lozyniak
	
	This sketch sets the date and time of a DS1307 or DS3231 real
	time clock chip.
	
	Usage: 
	1. Enter a year, month, day, hour, minute, and seconds values in the
	fields of "timeset" below. Enter the hour value using 24-hr 
	clock values. Make your time a minute or two in the future from
	the time you will upload the sketch to the Arduino.
	2. Once the sketch is uploaded, leave the Arduino plugged in
	to the computer. As the time that you set approaches on your 
	watch, hit the RESET button on the Arduino. You may need to hit 
	the button 1-2 seconds early to allow the bootloader time to run 
	before the sketch actually begins and sets the Real Time Clock value.
	If you time it right, the real time clock's value should exactly
	match your watch/computer time.
	3. After hitting RESET to set the clock, upload a different sketch 
	to the Arduino. The goal is to prevent this sketch 
	(RealTimeClock_reset_exact.ino) from running a 2nd time and 
	resetting the clock again. For example, try uploading the sketch 
	RealTimeClock_read.ino to check that the time was set correctly.


*/

#include "Wire.h"



void setup() {
  int timeset[] = {2014,  12,  10,  19,  15,  15};
					//   year  mo  day   hr  min  sec
  Wire.begin();
  Serial.begin(9600);
  // program to precisely set Chronodot
  Wire.beginTransmission(0x68); // address of DS3231 or DS1307
  Wire.write(0x00); // select register
  
  Wire.write(toBCD(timeset[5])); // seconds
  Wire.write(toBCD(timeset[4])); // minutes
  Wire.write(toBCD(timeset[3])); // hours
  Wire.write(weekday(timeset[0],timeset[1],timeset[2])); // day of week
  Wire.write(toBCD(timeset[2])); // day of month
  Wire.write(toBCD(timeset[1])); // month
  Wire.write(toBCD(timeset[0])); // year
  
  Wire.endTransmission();
}

void loop() {
  delay(500);  
}


byte toBCD(int n) {
  int tens = (n/10)%10;
  int ones = n%10;
  return (byte)(tens*16+ones);
}


byte weekday(int y, int m, int d) {
  if ((y<1583) || (y>9999)) return 0;
  if (d<1) return 0;
  int l = ((y%(((y%100)==0)?400:4)==0)?1:0);
  int n = y + (y/4) - (y/100) + (y/400);
  switch (m) {
    case 1:  if (d>31) return 0;  n+=(1-l); break;
    case 2: if (d>28+l) return 0; n+=(4-l); break;
    case 3:  if (d>31) return 0;  n+= 4;    break;
    case 4:  if (d>30) return 0;  break;
    case 5:  if (d>31) return 0;  n+= 2; break;
    case 6:  if (d>30) return 0;  n+= 5; break;
    case 7:  if (d>31) return 0;  break;
    case 8:  if (d>31) return 0;  n+= 3; break;
    case 9:  if (d>30) return 0;  n+= 6; break;
    case 10: if (d>31) return 0;  n+= 1; break;
    case 11: if (d>30) return 0;  n+= 4; break;
    case 12: if (d>31) return 0;  n+= 6; break;
    default: return 0;
  }
  n += d;
  n = ((n+4)%7)+1;
  return n;  // 1 for Mon, 2 for Tue, ..., 7 for Sun
}


