A Real Time Clock (RTC) Library for DS1307 compatible chips.
-------------------------------------------------------------
Tested to compile under Arduino 1.0, 0023, 0022 and 0021

Also inclues DS1307, DS3231 (Chronodot) and DS3234-SPI specific functions.


Basic functions  (see examples of how to use):
  .begin
  .adjust
  .isrunning
  .now
  
DS1307 specific functions:
  .readMemory
  .writeMemory

DS3231 specific functions:
  .getTemperature
  .getA1Time
  .getA2Time
  .setA1Time
  .setAlarm1Simple
  .setA2Time
  .setAlarm2Simple
  .turnOnAlarm
  .turnOffAlarm
  .checkAlarmEnabled
  .checkIfAlarm
  
  
  
Credits:
---------

- Code by JeeLabs http://news.jeelabs.org/code/
  Released to the public domain! Enjoy!

- Optimized DS1307 code by ADAfruit and others

-  Merged read/write RAM memory functions from:  
   github.com/dmalec/RTClib   by  MrAlvin 2012-02-27
   
-  Merged DS3231 & DS3234 functions from: 
   github/coobro/RTClib  by  MrAlvin 2012-02-27

       Alarm code for DS3231 (Chronodot) heavily used/modified 
	   from Eric Ayars DS3231 library  by  Coobro
    
	   Eric Ayars code is located at: 
	   http://hacks.ayars.org/2011/04/ds3231-real-time-clock.html

-  Chronodot keyword added  by MrAlvin  2012-02-28 

