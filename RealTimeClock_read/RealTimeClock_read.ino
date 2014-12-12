/*
	Code contributed by Robert Lozyniak

	Use this sketch to check that your real time clock
	(DS1307 or DS3231) is set properly. 
	Simply upload this sketch to an Arduino connected to
	an already-set RTC. Open the Arduino Serial Monitor
	and set the baud rate to 9600. 
	The sketch will print the real time clock's current
	value to the serial monitor. 

*/

#include <Wire.h>
 
void setup()
{
  Wire.begin();
  Serial.begin(9600);
 
  // clear /EOSC bit
  // Sometimes necessary to ensure that the clock
  // keeps running on just battery power. Once set,
  // it shouldn't need to be reset but it's a good
  // idea to make sure.
//  Wire.beginTransmission(0x68); // address of DS3231 or DS1307
//  Wire.write(0x0E); // select register
//  Wire.write(0b00011100); // write register bitmap, bit 7 is /EOSC
//  Wire.endTransmission();
}
 
void loop()
{
  // send request to receive data starting at register 0
  Wire.beginTransmission(0x68); // 0x68 is always DS3231 or DS1307 device address
  Wire.write((byte)0); // start at register 0
  Wire.endTransmission();
  Wire.requestFrom(0x68, 7); // request seven bytes (ss, mi, hr, wd, dd, mo, yy)
 
  while(Wire.available())
  { 
    byte ss = Wire.read(); // get seconds
    byte mi = Wire.read(); // get minutes
    byte hh = Wire.read(); // get hours
    byte wd = Wire.read();
    byte dd = Wire.read();
    byte mo = Wire.read();
    byte yr = Wire.read();
 
    Serial.print ("\'");
    if (yr<0x10) Serial.print("0"); Serial.print(yr,HEX); Serial.print("-");
    if (mo<0x10) Serial.print("0"); Serial.print(mo,HEX); Serial.print("-");
    if (dd<0x10) Serial.print("0"); Serial.print(dd,HEX); Serial.print("(");
    switch (wd) {
      case 1: Serial.print("Mon"); break;
      case 2: Serial.print("Tue"); break; 
      case 3: Serial.print("Wed"); break; 
      case 4: Serial.print("Thu"); break; 
      case 5: Serial.print("Fri"); break; 
      case 6: Serial.print("Sat"); break; 
      case 7: Serial.print("Sun"); break;
      default: Serial.print("Bad");  
    }
    Serial.print(") ");
    if (hh<0x10) Serial.print("0"); Serial.print(hh,HEX); Serial.print(":");
    if (mi<0x10) Serial.print("0"); Serial.print(mi,HEX); Serial.print(":");
    if (ss<0x10) Serial.print("0"); Serial.print(ss,HEX); Serial.println("");
    
  }
 
  delay(1000);
}
