Tide_calculator
===============

Arduino code and associated files for calculating tide height. This assumes that you 
have downloaded and installed the latest version of the Arduino software (1.6.4 or newer), 
and have an Arduino with attached real time clock, based on either a DS1307 or DS3231
I2C-based chip. If your real time clock hasn't previously been set, see the clock section below
for info on how to do this. 

### Download:
If you don't already have a git application installed and don't want to deal with learning git, you can simply download these files using the green "clone or download" link on the right side of the webpage. Extract the ZIP archive and grab the directories you need (detailed below in the Installation section). 

### Installation:

To generate a prediction of the current tide height for a site, install the following:

1. Install a copy of RTClib in your Arduino/libraries/ directory. If there is not already a libraries directory, create it now. RTClib can be downloaded from https://github.com/millerlp/RTClib. If the copy you download and unzip is called "RTClib-master", rename the folder to simply be "RTClib".

2. Copy the folder with the library for your site (i.e. TidelibMontereyMontereyHarborCalifornia) into Arduino/libraries/. As with the RTClib download, if the folder you unzip has "-master" tacked on to the name, remove "-master" from the folder name when you put it into the Arduino/libraries/ directory.

3. Open the Tide_calculator_check example sketch in the Arduino IDE by going to File>Examples>TidelibMontereyHarbor>Tide_calculator_check. The code to call the tide prediction library is referenced in the Initial Setup section, near line 55, with a line like:

	\#include "TidelibMontereyMontereyHarborCalifornia.h"

That line should contain the name of the library for your local site that you copied into Arduino/libraries/.

4. Plug the Arduino in to the computer using a USB cable. Upload the program to the Arduino. 
Open the serial monitor to view the output. See the http://arduino.cc site for help with 
these steps. 

5. You can use Tide_calculator_check.ino to ensure that the predicted
tide levels match your "real" tide tables from a more reliable source. In the Serial
Monitor, enter a date and time in the format YYYY MM DD HH MM, with the spaces between
the number values. For example, noon on Jan 1, 2019 would be typed into the Serial Monitor as 2019 1 1 12 00. You must set the Serial Monitor to send a 'newline' character when you hit return (see the dropdown menu in the lower right of the Serial
Monitor, choose 'newline'). If you are successful, the Arduino should report back the 
predicted tide height for your date and time, in units of feet above the zero tide line. Keep in mind that this library does not know about daylight savings time, so it assumes your time values are always being entered (and predicted back to you) in local standard time (i.e. the time zone offset for ~November to ~March). If you ask for tide heights during daylight savings time (~March to ~November), the predicted tide height will still be for standard time, and thus it may be one hour off from your tide tables or other software that accounts for the time shift.

------------------------------
### Setting a DS1307 or DS3231 Real Time Clock
If the real time clock attached to the Arduino is not yet set, you need to set it one time 
before loading the tide prediction software onto the Arduino. Open the Arduino software and 
go to File>Examples>RTClib>settime_Serial.ino. Follow the instructions at the top of that 
sketch to upload the correct time to your real time clock. Make sure the time you enter 
is in your local standard time, **NOT** Daylight Savings Time (which runs Mar-Nov in most places). 
The tide prediction routine relies on the time being set to local standard time for your site, 
otherwise you won't get the correct current tide height out, instead you'll get the tide for one hour offset. 

-------------------------------
### Generating tide prediction libaries for other sites
If there is no folder containing a tide prediction library for your desired site, it
will be necessary to generate a library using the R scripts found in the 
Generate_new_site_libraries directory. The harmonic data for NOAA sites are all in
the Harmonics_20181227.Rdata file. With these data, you can generate a new library
by running the R script tide_harmonics_library_generator.R. Inside that file, you must
enter a name for the site you're interested in on the line
stationID = 'Monterey Harbor'
Change the value inside the quote marks to match your site, save the file, and run the
script in R. It will create a new directory with the necessary library files inside that
can be copied into the Arduino/libraries/ folder. Find the name for your site by looking 
through the XTide website http://www.flaterco.com/xtide/locations.html 
XTide only produces harmonics for ~865 reference tide stations (all others are approximated 
from these tide stations), so you need to find the nearest station listed as "Ref" on that 
page, rather than "Sub" stations.

The other R scripts in the Generate_new_site_libraries directory could be used to make an
updated Rdata file when XTide updates the original harmonics database and after you
convert it to a text file using the libtcd library from the XTide site (see the notes inside
the R scripts). 
