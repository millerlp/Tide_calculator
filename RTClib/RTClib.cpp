// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#include <Wire.h>
#include <SPI.h>             // used by DS3234
#include <avr/pgmspace.h>
#include "RTClib.h"

#define DS1307_ADDRESS 0x68
#define DS3231_ADDRESS 0x68

#define SECONDS_PER_DAY 86400L

#define SECONDS_FROM_1970_TO_2000 946684800

#if (ARDUINO >= 100)
 #include <Arduino.h> // capital A so it is error prone on case-sensitive filesystems
#else
 #include <WProgram.h>
#endif

int i = 0; //The new wire library needs to take an int when you are sending for the zero register


////////////////////////////////////////////////////////////////////////////////
// utility code, some of this could be exposed in the DateTime API if needed

const uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 }; //has to be const or compiler compaints

// number of days since 2000/01/01, valid for 2001..2099
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && y % 4 == 0)
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

////////////////////////////////////////////////////////////////////////////////
// DateTime implementation - ignores time zones and DST changes
// NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second

DateTime::DateTime (uint32_t t) {
  t -= SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (yOff = 0; ; ++yOff) {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m) {
        uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
}

DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
    if (year >= 2000)
        year -= 2000;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;
}

static uint8_t conv2d(const char* p) {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

// A convenient constructor for using "the compiler's time":
//   DateTime now (__DATE__, __TIME__);
// NOTE: using PSTR would further reduce the RAM footprint
DateTime::DateTime (const char* date, const char* time) {
    // sample input: date = "Dec 26 2009", time = "12:34:56"
    yOff = conv2d(date + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec 
    switch (date[0]) {
        case 'J': m = date[1] == 'a' ? 1 : m = date[2] == 'n' ? 6 : 7; break;
        case 'F': m = 2; break;
        case 'A': m = date[2] == 'r' ? 4 : 8; break;
        case 'M': m = date[2] == 'r' ? 3 : 5; break;
        case 'S': m = 9; break;
        case 'O': m = 10; break;
        case 'N': m = 11; break;
        case 'D': m = 12; break;
    }
    d = conv2d(date + 4);
    hh = conv2d(time);
    mm = conv2d(time + 3);
    ss = conv2d(time + 6);
}

uint8_t DateTime::dayOfWeek() const {    
    uint16_t day = date2days(yOff, m, d);
    return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
}

uint32_t DateTime::unixtime(void) const {
  uint32_t t;
  uint16_t days = date2days(yOff, m, d);
  t = time2long(days, hh, mm, ss);
  t += SECONDS_FROM_1970_TO_2000;  // seconds from 1970 to 2000

  return t;
}

////////////////////////////////////////////////////////////////////////////////
// By Coobro
const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

// as a string
char* DateTime::toString(char* buf, int maxlen) const
{
    snprintf(buf,maxlen,"%s %02u %04u %02u:%02u:%02u",
             months[m-1],
             d,
             2000 + yOff,
             hh,
             mm,
             ss
            );
    return buf;
}

void DateTime::operator+=(uint32_t additional)
{
    DateTime after = DateTime( unixtime() + additional );
    *this = after;
}

///////////////////////////////////////////////////////////////////////////////
// BCD conversion tools
	
static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }
	
///////////////////////////////////////////////////////////////////////////////
// Arduino 1.0 addition

#if (ARDUINO >= 100)
	#define WW_  Wire.write
	#define WR_  Wire.read
#else
	#define WW_  Wire.send
	#define WR_  Wire.receive
#endif	
	
#if ARDUINO < 100
#define SEND(x) send(x) 
#define RECEIVE(x) receive(x) 
#else
#define SEND(x) write(static_cast<uint8_t>(x))
#define RECEIVE(x) read(x)
#endif


////////////////////////////////////////////////////////////////////////////////
// RTC_DS1307 implementation

uint8_t RTC_DS1307::begin(void) {
  return 1;
}


uint8_t RTC_DS1307::isrunning(void) {
  Wire.beginTransmission(DS1307_ADDRESS);
  WW_(i);	
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 1);
  uint8_t ss = WR_();
  return !(ss>>7);
}

void RTC_DS1307::adjust(const DateTime& dt) {
    Wire.beginTransmission(DS1307_ADDRESS);
    WW_(i);
    WW_(bin2bcd(dt.second()));
    WW_(bin2bcd(dt.minute()));
    WW_(bin2bcd(dt.hour()));
    WW_(bin2bcd(0));
    WW_(bin2bcd(dt.day()));
    WW_(bin2bcd(dt.month()));
    WW_(bin2bcd(dt.year() - 2000));
    WW_(i);
    Wire.endTransmission();
}

DateTime RTC_DS1307::now() {
  Wire.beginTransmission(DS1307_ADDRESS);
  WW_(i);	
  Wire.endTransmission();
  
  Wire.requestFrom(DS1307_ADDRESS, 7);
  uint8_t ss = bcd2bin(WR_() & 0x7F);
  uint8_t mm = bcd2bin(WR_());
  uint8_t hh = bcd2bin(WR_());
  WR_();
  uint8_t d = bcd2bin(WR_());
  uint8_t m = bcd2bin(WR_());
  uint16_t y = bcd2bin(WR_()) + 2000;
  
  return DateTime (y, m, d, hh, mm, ss);
}

uint8_t RTC_DS1307::readMemory(uint8_t offset, uint8_t* data, uint8_t length) {
  uint8_t bytes_read = 0;

  Wire.beginTransmission(DS1307_ADDRESS);
  WW_(0x08 + offset);
  Wire.endTransmission();
  
  Wire.requestFrom((uint8_t)DS1307_ADDRESS, (uint8_t)length);
  while (Wire.available() > 0 && bytes_read < length) {
    data[bytes_read] = WR_();
    bytes_read++;
  }

  return bytes_read;
}

uint8_t RTC_DS1307::writeMemory(uint8_t offset, uint8_t* data, uint8_t length) {
  uint8_t bytes_written;

  Wire.beginTransmission(DS1307_ADDRESS);
  WW_(0x08 + offset);
#if (ARDUINO >= 100)
  bytes_written =  WW_(data, length);
  Wire.endTransmission();
  return bytes_written;
#else
  WW_(data, length);
  Wire.endTransmission();
  return length;
#endif  
}

////////////////////////////////////////////////////////////////////////////////
// RTC_DS3231 implementation

uint8_t RTC_DS3231::begin(void)
{
    return 1;
}

uint8_t RTC_DS3231::isrunning(void)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.SEND(0);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 1);
    uint8_t ss = Wire.RECEIVE();
    return !(ss>>7);
}

void RTC_DS3231::adjust(const DateTime& dt)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.SEND(0);
    Wire.SEND(bin2bcd(dt.second()));
    Wire.SEND(bin2bcd(dt.minute()));
    Wire.SEND(bin2bcd(dt.hour()));
    Wire.SEND(bin2bcd(0));
    Wire.SEND(bin2bcd(dt.day()));
    Wire.SEND(bin2bcd(dt.month()));
    Wire.SEND(bin2bcd(dt.year() - 2000));
    Wire.SEND(0);
    Wire.endTransmission();
}

DateTime RTC_DS3231::now()
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.SEND(0);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 19);
    
    uint8_t ss = bcd2bin(Wire.RECEIVE() & 0x7F);
    uint8_t mm = bcd2bin(Wire.RECEIVE());
    uint8_t hh = bcd2bin(Wire.RECEIVE());
    Wire.RECEIVE();
    uint8_t d = bcd2bin(Wire.RECEIVE());
    uint8_t m = bcd2bin(Wire.RECEIVE());
    uint16_t y = bcd2bin(Wire.RECEIVE()) + 2000;

    return DateTime (y, m, d, hh, mm, ss);
}

float RTC_DS3231::getTemperature() {
    // Checks the internal thermometer on the DS3231 and returns the 
    // temperature as a floating-point value.
    byte temp;
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.SEND(0x11);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 2);
    temp = Wire.RECEIVE();  // Here's the MSB
    return float(temp) + 0.25*(Wire.RECEIVE()>>6);
}

void RTC_DS3231::getA1Time(byte& A1Day, byte& A1Hour, byte& A1Minute, byte& A1Second, byte& AlarmBits, bool& A1Dy, bool& A1h12, bool& A1PM) {
    byte temp_buffer;
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.SEND(0x07);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 4);

    temp_buffer = Wire.RECEIVE();   // Get A1M1 and A1 Seconds
    A1Second    = bcd2bin(temp_buffer & 0b01111111);
    // put A1M1 bit in position 0 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0b10000000)>>7;

    temp_buffer     = Wire.RECEIVE();   // Get A1M2 and A1 minutes
    A1Minute    = bcd2bin(temp_buffer & 0b01111111);
    // put A1M2 bit in position 1 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0b10000000)>>6;

    temp_buffer = Wire.RECEIVE();   // Get A1M3 and A1 Hour
    // put A1M3 bit in position 2 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0b10000000)>>5;
    // determine A1 12/24 mode
    A1h12       = temp_buffer & 0b01000000;
    if (A1h12) {
        A1PM    = temp_buffer & 0b00100000;         // determine am/pm
        A1Hour  = bcd2bin(temp_buffer & 0b00011111);   // 12-hour
    } else {
        A1Hour  = bcd2bin(temp_buffer & 0b00111111);   // 24-hour
    }

    temp_buffer = Wire.RECEIVE();   // Get A1M4 and A1 Day/Date
    // put A1M3 bit in position 3 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0b10000000)>>4;
    // determine A1 day or date flag
    A1Dy        = (temp_buffer & 0b01000000)>>6;
    if (A1Dy) {
        // alarm is by day of week, not date.
        A1Day   = bcd2bin(temp_buffer & 0b00001111);
    } else {
        // alarm is by date, not day of week.
        A1Day   = bcd2bin(temp_buffer & 0b00111111);
    }
}

void RTC_DS3231::getA2Time(byte& A2Day, byte& A2Hour, byte& A2Minute, byte& AlarmBits, bool& A2Dy, bool& A2h12, bool& A2PM) {
    byte temp_buffer;
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.SEND(0x0b);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 3); 
    temp_buffer = Wire.RECEIVE();   // Get A2M2 and A2 Minutes
    A2Minute    = bcd2bin(temp_buffer & 0b01111111);
    // put A2M2 bit in position 4 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0b10000000)>>3;

    temp_buffer = Wire.RECEIVE();   // Get A2M3 and A2 Hour
    // put A2M3 bit in position 5 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0b10000000)>>2;
    // determine A2 12/24 mode
    A2h12       = temp_buffer & 0b01000000;
    if (A2h12) {
        A2PM    = temp_buffer & 0b00100000;         // determine am/pm
        A2Hour  = bcd2bin(temp_buffer & 0b00011111);   // 12-hour
    } else {
        A2Hour  = bcd2bin(temp_buffer & 0b00111111);   // 24-hour
    }

    temp_buffer = Wire.RECEIVE();   // Get A2M4 and A1 Day/Date
    // put A2M4 bit in position 6 of DS3231_AlarmBits.
    AlarmBits   = AlarmBits | (temp_buffer & 0b10000000)>>1;
    // determine A2 day or date flag
    A2Dy        = (temp_buffer & 0b01000000)>>6;
    if (A2Dy) {
        // alarm is by day of week, not date.
        A2Day   = bcd2bin(temp_buffer & 0b00001111);
    } else {
        // alarm is by date, not day of week.
        A2Day   = bcd2bin(temp_buffer & 0b00111111);
    }
}

void RTC_DS3231::setAlarm1Simple(byte hour, byte minute) {
    setA1Time(0, hour, minute, 00, 0b00001000, false, false, false);
}

void RTC_DS3231::setAlarm2Simple(byte hour, byte minute) {
    setA2Time(0, hour, minute, 0b00001000, false, false, false);
}

void RTC_DS3231::setA1Time(byte A1Day, byte A1Hour, byte A1Minute, byte A1Second, byte AlarmBits, bool A1Dy, bool A1h12, bool A1PM) {
    //  Sets the alarm-1 date and time on the DS3231, using A1* information
    byte temp_buffer;
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.SEND(0x07);    // A1 starts at 07h
    // Send A1 second and A1M1
    Wire.SEND(bin2bcd(A1Second) | ((AlarmBits & 0b00000001) << 7));
    // Send A1 Minute and A1M2
    Wire.SEND(bin2bcd(A1Minute) | ((AlarmBits & 0b00000010) << 6));
    // Figure out A1 hour 
    if (A1h12) {
        // Start by converting existing time to h12 if it was given in 24h.
        if (A1Hour > 12) {
            // well, then, this obviously isn't a h12 time, is it?
            A1Hour = A1Hour - 12;
            A1PM = true;
        }
        if (A1PM) {
            // Afternoon
            // Convert the hour to BCD and add appropriate flags.
            temp_buffer = bin2bcd(A1Hour) | 0b01100000;
        } else {
            // Morning
            // Convert the hour to BCD and add appropriate flags.
            temp_buffer = bin2bcd(A1Hour) | 0b01000000;
        }
    } else {
        // Now for 24h
        temp_buffer = bin2bcd(A1Hour); 
    }
    temp_buffer = temp_buffer | ((AlarmBits & 0b00000100)<<5);
    // A1 hour is figured out, send it
    Wire.SEND(temp_buffer); 
    // Figure out A1 day/date and A1M4
    temp_buffer = ((AlarmBits & 0b00001000)<<4) | bin2bcd(A1Day);
    if (A1Dy) {
        // Set A1 Day/Date flag (Otherwise it's zero)
        temp_buffer = temp_buffer | 0b01000000;
    }
    Wire.SEND(temp_buffer);
    // All done!
    Wire.endTransmission();
}

void RTC_DS3231::setA2Time(byte A2Day, byte A2Hour, byte A2Minute, byte AlarmBits, bool A2Dy, bool A2h12, bool A2PM) {
    //  Sets the alarm-2 date and time on the DS3231, using A2* information
    byte temp_buffer;
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.SEND(0x0b);    // A1 starts at 0bh
    // Send A2 Minute and A2M2
    Wire.SEND(bin2bcd(A2Minute) | ((AlarmBits & 0b00010000) << 3));
    // Figure out A2 hour 
    if (A2h12) {
        // Start by converting existing time to h12 if it was given in 24h.
        if (A2Hour > 12) {
            // well, then, this obviously isn't a h12 time, is it?
            A2Hour = A2Hour - 12;
            A2PM = true;
        }
        if (A2PM) {
            // Afternoon
            // Convert the hour to BCD and add appropriate flags.
            temp_buffer = bin2bcd(A2Hour) | 0b01100000;
        } else {
            // Morning
            // Convert the hour to BCD and add appropriate flags.
            temp_buffer = bin2bcd(A2Hour) | 0b01000000;
        }
    } else {
        // Now for 24h
        temp_buffer = bin2bcd(A2Hour); 
    }
    // add in A2M3 bit
    temp_buffer = temp_buffer | ((AlarmBits & 0b00100000)<<2);
    // A2 hour is figured out, send it
    Wire.SEND(temp_buffer); 
    // Figure out A2 day/date and A2M4
    temp_buffer = ((AlarmBits & 0b01000000)<<1) | bin2bcd(A2Day);
    if (A2Dy) {
        // Set A2 Day/Date flag (Otherwise it's zero)
        temp_buffer = temp_buffer | 0b01000000;
    }
    Wire.SEND(temp_buffer);
    // All done!
    Wire.endTransmission();
}

void RTC_DS3231::turnOnAlarm(byte Alarm) {
    // turns on alarm number "Alarm". Defaults to 2 if Alarm is not 1.
    byte temp_buffer = readControlByte(0);
    // modify control byte
    if (Alarm == 1) {
        temp_buffer = temp_buffer | 0b00000101;
    } else {
        temp_buffer = temp_buffer | 0b00000110;
    }
    writeControlByte(temp_buffer, 0);
}

void RTC_DS3231::turnOffAlarm(byte Alarm) {
    // turns off alarm number "Alarm". Defaults to 2 if Alarm is not 1.
    // Leaves interrupt pin alone.
    byte temp_buffer = readControlByte(0);
    // modify control byte
    if (Alarm == 1) {
        temp_buffer = temp_buffer & 0b11111110;
    } else {
        temp_buffer = temp_buffer & 0b11111101;
    }
    writeControlByte(temp_buffer, 0);
}

bool RTC_DS3231::checkAlarmEnabled(byte Alarm) {
    // Checks whether the given alarm is enabled.
    byte result = 0x0;
    byte temp_buffer = readControlByte(0);
    if (Alarm == 1) {
        result = temp_buffer & 0b00000001;
    } else {
        result = temp_buffer & 0b00000010;
    }
    return result;
}

bool RTC_DS3231::checkIfAlarm(byte Alarm) {
    // Checks whether alarm 1 or alarm 2 flag is on, returns T/F accordingly.
    // Turns flag off, also.
    // defaults to checking alarm 2, unless Alarm == 1.
    byte result;
    byte temp_buffer = readControlByte(1);
    if (Alarm == 1) {
        // Did alarm 1 go off?
        result = temp_buffer & 0b00000001;
        // clear flag
        temp_buffer = temp_buffer & 0b11111110;
    } else {
        // Did alarm 2 go off?
        result = temp_buffer & 0b00000010;
        // clear flag
        temp_buffer = temp_buffer & 0b11111101;
    }
    writeControlByte(temp_buffer, 1);
    return result;
}

void RTC_DS3231::enableOscillator(bool TF, bool battery, byte frequency) {
    // turns oscillator on or off. True is on, false is off.
    // if battery is true, turns on even for battery-only operation,
    // otherwise turns off if Vcc is off.
    // frequency must be 0, 1, 2, or 3.
    // 0 = 1 Hz
    // 1 = 1.024 kHz
    // 2 = 4.096 kHz
    // 3 = 8.192 kHz (Default if frequency byte is out of range)
    if (frequency > 3) frequency = 3;
    // read control byte in, but zero out current state of RS2 and RS1.
    byte temp_buffer = readControlByte(0) & 0b11100111;
    if (battery) {
        // turn on BBSQW flag
        temp_buffer = temp_buffer | 0b01000000;
    } else {
        // turn off BBSQW flag
        temp_buffer = temp_buffer & 0b10111111;
    }
    if (TF) {
        // set ~EOSC to 0 and INTCN to zero.
        temp_buffer = temp_buffer & 0b01111011;
    } else {
        // set ~EOSC to 1, leave INTCN as is.
        temp_buffer = temp_buffer | 0b10000000;
    }
    // shift frequency into bits 3 and 4 and set.
    frequency = frequency << 3;
    temp_buffer = temp_buffer | frequency;
    // And write the control bits
    writeControlByte(temp_buffer, 0);
}

void RTC_DS3231::enable32kHz(bool TF) {
    // turn 32kHz pin on or off
    byte temp_buffer = readControlByte(1);
    if (TF) {
        // turn on 32kHz pin
        temp_buffer = temp_buffer | 0b00001000;
    } else {
        // turn off 32kHz pin
        temp_buffer = temp_buffer & 0b11110111;
    }
    writeControlByte(temp_buffer, 1);
}

bool RTC_DS3231::oscillatorCheck() {
    // Returns false if the oscillator has been off for some reason.
    // If this is the case, the time is probably not correct.
    byte temp_buffer = readControlByte(1);
    bool result = true;
    if (temp_buffer & 0b10000000) {
        // Oscillator Stop Flag (OSF) is set, so return false.
        result = false;
    }
    return result;
}

byte RTC_DS3231::readControlByte(bool which) {
    // Read selected control byte
    // first byte (0) is 0x0e, second (1) is 0x0f
    Wire.beginTransmission(DS3231_ADDRESS);
    if (which) {
        // second control byte
        Wire.SEND(0x0f);
    } else {
        // first control byte
        Wire.SEND(0x0e);
    }
    Wire.endTransmission();
    Wire.requestFrom(DS3231_ADDRESS, 1);
    return Wire.RECEIVE();  
}

void RTC_DS3231::writeControlByte(byte control, bool which) {
    // Write the selected control byte.
    // which=false -> 0x0e, true->0x0f.
    Wire.beginTransmission(DS3231_ADDRESS);
    if (which) {
        Wire.SEND(0x0f);
    } else {
        Wire.SEND(0x0e);
    }
    Wire.SEND(control);
    Wire.endTransmission();
}


////////////////////////////////////////////////////////////////////////////////
// RTC_DS3234 implementation

// Registers we use
const int CONTROL_R = 0x0e;
const int CONTROL_W = 0x8e;
const int CONTROL_STATUS_R = 0x0f;
const int CONTROL_STATUS_W = 0x8f;
const int SECONDS_R = 0x00;
const int SECONDS_W = 0x80;

// Bits we use
const int EOSC = 7;
const int OSF = 7;

uint8_t RTC_DS3234::begin(void)
{
    pinMode(cs_pin,OUTPUT);
    cs(HIGH);
    SPI.setBitOrder(MSBFIRST);

    //Ugh!  In order to get this to interop with other SPI devices,
    //This has to be done in cs()
    SPI.setDataMode(SPI_MODE1);

    //Enable oscillator, disable square wave, alarms
    cs(LOW);
    SPI.transfer(CONTROL_W);
    SPI.transfer(0x0);
    cs(HIGH);
    delay(1);

    //Clear oscilator stop flag, 32kHz pin
    cs(LOW);
    SPI.transfer(CONTROL_STATUS_W);
    SPI.transfer(0x0);
    cs(HIGH);
    delay(1);

    return 1;
}

void RTC_DS3234::cs(int _value)
{
    SPI.setDataMode(SPI_MODE1);
    digitalWrite(cs_pin,_value);
}

uint8_t RTC_DS3234::isrunning(void)
{
    cs(LOW);
    SPI.transfer(CONTROL_R);
    uint8_t ss = SPI.transfer(-1);
    cs(HIGH);
    return !(ss & _BV(OSF));
}

void RTC_DS3234::adjust(const DateTime& dt)
{
    cs(LOW);
    SPI.transfer(SECONDS_W);
    SPI.transfer(bin2bcd(dt.second()));
    SPI.transfer(bin2bcd(dt.minute()));
    SPI.transfer(bin2bcd(dt.hour()));
    SPI.transfer(bin2bcd(dt.dayOfWeek()));
    SPI.transfer(bin2bcd(dt.day()));
    SPI.transfer(bin2bcd(dt.month()));
    SPI.transfer(bin2bcd(dt.year() - 2000));
    cs(HIGH);

}

DateTime RTC_DS3234::now()
{
    cs(LOW);
    SPI.transfer(SECONDS_R);
    uint8_t ss = bcd2bin(SPI.transfer(-1) & 0x7F);
    uint8_t mm = bcd2bin(SPI.transfer(-1));
    uint8_t hh = bcd2bin(SPI.transfer(-1));
    SPI.transfer(-1);
    uint8_t d = bcd2bin(SPI.transfer(-1));
    uint8_t m = bcd2bin(SPI.transfer(-1));
    uint16_t y = bcd2bin(SPI.transfer(-1)) + 2000;
    cs(HIGH);

    return DateTime (y, m, d, hh, mm, ss);
}



////////////////////////////////////////////////////////////////////////////////
// RTC_Millis implementation

long RTC_Millis::offset = 0;

void RTC_Millis::adjust(const DateTime& dt) {
    offset = dt.unixtime() - millis() / 1000;
}

DateTime RTC_Millis::now() {
  return (uint32_t)(offset + millis() / 1000);
}

////////////////////////////////////////////////////////////////////////////////
