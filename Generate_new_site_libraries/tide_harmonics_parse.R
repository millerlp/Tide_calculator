# Filename: tide_harmonics_parse.R
# 
# Author: Luke Miller  May 1, 2012
# Updated 2015-06-03 to use new harmonics input file from 2014-12-24
###############################################################################
# This is essentially a one-time use script to call the read_harmonicsfile.R 
# functions and parse the tidal harmonics file from XTide. The resulting data 
# for the ~637 reference stations can be stored in a Rdata file for quick 
# loading and use in tide predictions.
# The harmonics file must be a text format, not the binary 
# tcd format that is generally distributed with XTide. To generate this text 
# version of the harmonics.tcd file, you must use the command line tool 
# 'restore_tide_db' found in the tcd-utils package distributed on the XTide 
# site. This will require a Linux or Mac (not Windows) machine.
# See http://www.flaterco.com/xtide/files.html for downloads.

# Load the functions from read_harmonicsfile.R
source('./read_harmonicsfile.R')

# Create a connection to the harmonics text file for reading.
fid = file('W:/xtide/harmonics-20141224.txt', open = 'r')

# Call the read_harmonicsfile function (found in read_harmonicsfile.R) to parse
# the harmonics file into a usable format.
harms = read_harmonicsfile(fid)

# Close file connection when finished.
close(fid)

##############
# Now remove the current speed stations from the data set to avoid potential
# ambiguities in finding tide stations by name
harms2 = list(name = harms$name, speed = harms$speed, 
		startyear = harms$startyear, 
		equilarg = harms$equilarg,
		nodefactor = harms$nodefactor, 
		station = NULL,
		stationIDnumber = NULL,
		units = NULL,
		longitude = NULL,
		latitude = NULL,
		timezone = NULL,
		tzfile = NULL,
		datum = NULL,
		A = NULL,
		kappa = NULL)

for (i in 1:length(harms$units)) {
	if (harms$units[i] == 'feet') {
		harms2$station = c(harms2$station, harms$station[i])
		harms2$stationIDnumber = c(harms2$stationIDnumber, 
				harms$stationIDnumber[i])
		harms2$units = c(harms2$units, harms$units[i])
		harms2$longitude = c(harms2$longitude, harms$longitude[i])
		harms2$latitude = c(harms2$latitude, harms$latitude[i])
		harms2$timezone = c(harms2$timezone, harms$timezone[i])
		harms2$tzfile = c(harms2$tzfile, harms$tzfile[i])
		harms2$datum = c(harms2$datum, harms$datum[i])
		harms2$A = rbind(harms2$A, harms$A[i, ])
		harms2$kappa = rbind(harms2$kappa, harms$kappa[i, ])
	}
}

# Replace harms with harms2 contents
harms = harms2

# Save the results to a Rdata file, since there's no need to re-parse the 
# harmonics file once you've done it once. 
save(harms, file = 'Harmonics_20141224_2.Rdata')

