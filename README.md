Tide_controller
===============

Arduino code and associated files for calculating tide height. This assumes that you 
have downloaded and installed the latest version of the Arduino software (1.0.1 or newer), 
and have an Arduino with attached real time clock, based on either a DS1307 or DS3231
I2C-based chip. If you real time clock hasn't previously been set, see the clock section below
for info on how to do this. 

### Download:
If you don't already have a git application installed and don't want to deal with learning git, you can simply download these files using the Download ZIP link on the right side of the webpage. Extract the ZIP archive and grab the directories you need (detailed below in the Installation section). 

### Installation:

To generate a prediction of the current tide height for a site, install the following:

1. Copy the RTClib folder into Arduino/libraries/. If you don't already have a folder called 'libraries' inside your Arduino folder, go ahead and create one now. 

2. Copy the folder with the library for your site (i.e. TidelibMontereyHarbor) into Arduino/libraries/

3. Open the Tide_calculator example sketch in the Arduino IDE by going to File>Examples>TidelibMontereyHarbor>Tide_calculator. The code to call the tide prediction library is referenced in the Initial Setup section, near line 44, with a line like:

	\#include "TidelibMontereyHarbor.h"

That line should contain the name of the library for your local site that you copied into Arduino/libraries/.

4. Plug the Arduino in to the computer using a USB cable. Upload the program to the Arduino. 
Open the serial monitor to view the output. See the http://arduino.cc site for help with 
these steps. 

------------------------------
### Setting a DS1307 or DS3231 Real Time Clock
If the real time clock attached to the Arduino is not yet set, you need to set it one time.

Find the sketch "RealTimeClock_reset_exact.ino" or "RealTimeClock_reset.ino" in the Tide_calculator
GitHub directory. Read the instructions at the top of either sketch to see how to use it
to set the Real Time Clock. "RealTimeClock_reset_exact" is more useful if you want the time
to be as close to correct as possible (for instance if you're also going to be displaying the
time with seconds values). *Always* make sure that you set the real time clock to the local
standard time of the location you're making predictions for. Do not use Daylight Savings Time 
adjustments, this library only works in Standard Time. 

After running the RealTimeClock_reset_exact or RealTimeClock_reset sketch, before unplugging the Arduino, 
immediately upload a different sketch (such as RealTimeClock_read.ino sketch) to the Arduino so that the 
clock doesn't try to reset itself repeatedly when the Arduino restarts.

The RealTimeClock_reset_exact.ino and RealTimeClock_read.ino sketches were contributed by
Robert Lozyniak. 

-------------------------------
### Generating tide prediction libaries for other sites
If there is no folder containing a tide prediction library for your desired site, it
will be necessary to generate a library using the R scripts found in the 
Generate_new_site_libraries directory. The harmonic data for NOAA sites are all in
the Harmonics_20120302.Rdata file. With these data, you can generate a new library
by running the R script tide_harmonics_library_generator.R. Inside that file, you must
enter a name for the site you're interested in on the line
stationID = 'Monterey Harbor'
Change the value inside the quote marks to match your site, save the file, and run the
script in R. It will create a new directory with the necessary library files inside that
can be copied into the Arduino/libraries/ folder. Find the name for your site by looking 
through the XTide website http://www.flaterco.com/xtide/locations.html 
XTide only produces harmonics for 635 reference tide stations (all others are approximated 
from these tide stations), so you need to find the nearest station listed as "Ref" on that 
page, rather than "Sub" stations.

The other R scripts in the Generate_new_site_libraries directory could be used to make an
updated Rdata file when XTide updates the original harmonics database and after you
convert it to a text file using the libtcd library from the XTide site (see the notes inside
the R scripts). 
