// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

// Chronodot keyword added  bt MrAlvin  2012-02-28 
// Merged read/write RAM memory functions from:  github.com/dmalec/RTClib   by  MrAlvin 2012-02-27
// Merged DS3231 & DS3234 functions from: github/coobro/RTClib  by  MrAlvin 2012-02-27


// Alarm code for DS3231 (Chronodot) heavily used/modified from Eric Ayars DS3231 library  by  Coobro
// Eric Ayars code is located at: http://hacks.ayars.org/2011/04/ds3231-real-time-clock.html

#ifndef __RTCLIB_H__
#define __RTCLIB_H__
 
#define Chronodot RTC_DS3231

// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
class DateTime {
public:
    DateTime (uint32_t t =0);
    DateTime (uint16_t year, uint8_t month, uint8_t day,
                uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
    DateTime (const char* date, const char* time);
    uint16_t year() const       { return 2000 + yOff; }
    uint8_t month() const       { return m; }
    uint8_t day() const         { return d; }
    uint8_t hour() const        { return hh; }
    uint8_t minute() const      { return mm; }
    uint8_t second() const      { return ss; }
    uint8_t dayOfWeek() const;

    // 32-bit times as seconds since 1/1/2000
    long secondstime() const;   
    // 32-bit times as seconds since 1/1/1970
    uint32_t unixtime(void) const;
    // as a string
    char* toString(char* buf, int maxlen) const;
    // add additional time
    void operator+=(uint32_t);
	
protected:
    uint8_t yOff, m, d, hh, mm, ss;
};



////////////////////////////////////////////////////////////////////////////////
// RTC based on the DS1307 chip connected via I2C and the Wire library
class RTC_DS1307 {
public:
  static uint8_t begin(void);
    static void adjust(const DateTime& dt);
    uint8_t isrunning(void);
    static DateTime now();
    static uint8_t readMemory(uint8_t offset, uint8_t* data, uint8_t length);
    static uint8_t writeMemory(uint8_t offset, uint8_t* data, uint8_t length);
};


////////////////////////////////////////////////////////////////////////////////
// RTC based on the DS3231 chip connected via I2C
class RTC_DS3231
{
public:
    uint8_t begin(void);
    void adjust(const DateTime& dt);
    uint8_t isrunning(void);
    DateTime now();

    // Temperature function

    float getTemperature();     

    void getA1Time(byte& A1Day, byte& A1Hour, byte& A1Minute, byte& A1Second, byte& AlarmBits, bool& A1Dy, bool& A1h12, bool& A1PM); 
/* Retrieves everything you could want to know about alarm
 * one. 
 * A1Dy true makes the alarm go on A1Day = Day of Week,
 * A1Dy false makes the alarm go on A1Day = Date of month.
 *
 * byte AlarmBits sets the behavior of the alarms:
 *	Dy	A1M4	A1M3	A1M2	A1M1	Rate
 *	X	1		1		1		1		Once per second
 *	X	1		1		1		0		Alarm when seconds match
 *	X	1		1		0		0		Alarm when min, sec match
 *	X	1		0		0		0		Alarm when hour, min, sec match
 *	0	0		0		0		0		Alarm when date, h, m, s match
 *	1	0		0		0		0		Alarm when DoW, h, m, s match
 *
 *	Dy	A2M4	A2M3	A2M2	Rate
 *	X	1		1		1		Once per minute (at seconds = 00)
 *	X	1		1		0		Alarm when minutes match
 *	X	1		0		0		Alarm when hours and minutes match
 *	0	0		0		0		Alarm when date, hour, min match
 *	1	0		0		0		Alarm when DoW, hour, min match
 */	
    void getA2Time(byte& A2Day, byte& A2Hour, byte& A2Minute, byte& AlarmBits, bool& A2Dy, bool& A2h12, bool& A2PM); 
        // Same as getA1Time();, but A2 only goes on seconds == 00.
    void setA1Time(byte A1Day, byte A1Hour, byte A1Minute, byte A1Second, byte AlarmBits, bool A1Dy, bool A1h12, bool A1PM); 
        // Set the details for Alarm 1	
    void setAlarm1Simple(byte hour, byte minute);
        // A simple hour/minute alarm.
    void setA2Time(byte A2Day, byte A2Hour, byte A2Minute, byte AlarmBits, bool A2Dy, bool A2h12, bool A2PM); 
        // Set the details for Alarm 2
    void setAlarm2Simple(byte hour, byte minute);
        // A simple hour/minute alarm.	
    void turnOnAlarm(byte Alarm); 
        // Enables alarm 1 or 2 and the external interrupt pin.
        // If Alarm != 1, it assumes Alarm == 2.
    void turnOffAlarm(byte Alarm); 
        // Disables alarm 1 or 2 (default is 2 if Alarm != 1);
        // and leaves the interrupt pin alone.
    bool checkAlarmEnabled(byte Alarm); 
        // Returns T/F to indicate whether the requested alarm is
        // enabled. Defaults to 2 if Alarm != 1.
    bool checkIfAlarm(byte Alarm); 
        // Checks whether the indicated alarm (1 or 2, 2 default);
        // has been activated.

    // Oscillator functions

    void enableOscillator(bool TF, bool battery, byte frequency); 
        // turns oscillator on or off. True is on, false is off.
        // if battery is true, turns on even for battery-only operation,
        // otherwise turns off if Vcc is off.
        // frequency must be 0, 1, 2, or 3.
        // 0 = 1 Hz
        // 1 = 1.024 kHz
        // 2 = 4.096 kHz
        // 3 = 8.192 kHz (Default if frequency byte is out of range);
    void enable32kHz(bool TF); 
		// Turns the 32kHz output pin on (true); or off (false).
    bool oscillatorCheck();;
        // Checks the status of the OSF (Oscillator Stop Flag);.
        // If this returns false, then the clock is probably not
        // giving you the correct time.
        // The OSF is cleared by function setSecond();.		

protected:
    void cs(int _value);

private:
    int cs_pin;
    byte readControlByte(bool which); 
		// Read selected control byte: (0); reads 0x0e, (1) reads 0x0f
    void writeControlByte(byte control, bool which); 
		// Write the selected control byte. 
		// which == false -> 0x0e, true->0x0f.    
};

////////////////////////////////////////////////////////////////////////////////
// RTC based on the DS3234 chip connected via SPI and the SPI library
class RTC_DS3234
{
public:
    RTC_DS3234(int _cs_pin): cs_pin(_cs_pin) {}
    uint8_t begin(void);
    void adjust(const DateTime& dt);
    uint8_t isrunning(void);
    DateTime now();

protected:
    void cs(int _value);

private:
    int cs_pin;
};


////////////////////////////////////////////////////////////////////////////////
// RTC using the internal millis() clock, has to be initialized before use
// NOTE: this clock won't be correct once the millis() timer rolls over (>49d?)
class RTC_Millis {
public:
    static void begin(const DateTime& dt) { adjust(dt); }
    static void adjust(const DateTime& dt);
    static DateTime now();
	RTC_Millis(void) {	adjust(DateTime(2000,1,1,0,0,0)); }

protected:
    static long offset;
};

#endif // __RTCLIB_H__