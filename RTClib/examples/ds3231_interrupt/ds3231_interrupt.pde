// Date, Time and Alarm functions using a DS3231 RTC connected via I2C and Wire lib

#include <Wire.h>
#include <SPI.h>   // not used here, but needed to prevent a RTClib compile error
#include <avr/sleep.h>
#include <RTClib.h>


RTC_DS3231 RTC;
int INTERRUPT_PIN = 2;
volatile int state = LOW;

void setup () {
  pinMode(INTERRUPT_PIN, INPUT);
  //pull up the interrupt pin
  digitalWrite(INTERRUPT_PIN, HIGH);
  
  Serial.begin(57600);
  Wire.begin();
  RTC.begin();
    
  RTC.adjust(DateTime(__DATE__, __TIME__));
  DateTime now = RTC.now();
  RTC.setAlarm1Simple(22, 11);
  RTC.setAlarm2Simple(11, 10);
  RTC.turnOnAlarm(1);
  RTC.turnOnAlarm(2);
  if (RTC.checkAlarmEnabled(1) && RTC.checkAlarmEnabled(2)) {
    Serial.println("Alarms Enabled");
  }
  attachInterrupt(0, logData, LOW);
}

void loop () {
  DateTime now = RTC.now();
  if (RTC.checkIfAlarm(1) || RTC.checkIfAlarm(2)) {
    Serial.println("Alarm Triggered");
  }
  
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  Serial.println("Going to Sleep");
  delay(600);
  sleepNow();
  Serial.println("AWAKE");
}

void sleepNow() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(0,logData, LOW);
  sleep_mode();
  //HERE AFTER WAKING UP
  sleep_disable();
  detachInterrupt(0);
}

void logData() {
  //do something quick, flip a flag, and handle in loop();
}