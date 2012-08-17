/* ReatTimeClock_reset
  Use this sketch to reset the DS1307 or DS3231 Real Time Clock. 
  It will grab the computer's current time during compilation and 
  send that value to the real time clock. 
  
  Afterwards, immediately upload a different sketch to the Arduino
  so that it doesn't try to constantly reset the clock when it 
  powers up the next time. If it does reset, the Arduino will reset 
  the clock with the old compile time stamp, which will be out of
  date.
*/

#include <Wire.h>
#include <SPI.h> // Required for RTClib to compile properly
#include <RTClib.h> // From https://github.com/MrAlvin/RTClib

RTC_DS1307 RTC;

boolean resetFlag = false; // flag for clock reset

void setup(void)
{
  delay(2000);
  Wire.begin();
  RTC.begin();  
  RTC.adjust(DateTime(__DATE__, __TIME__));


  DateTime now = RTC.now();
  Serial.begin(9600);
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);  
  delay(2000);
}

void loop(void)
{
  DateTime now = RTC.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  delay(2000);
  

}
  
