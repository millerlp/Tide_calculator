# Filename: read_harmonicsfile.R
# 
# Author: Luke Miller  May 2, 2012
###############################################################################
# These functions are based on code originally written by R. Pawlowicz as part 
# of the T_TIDE harmonic analysis toolbox for MATLAB. The original MATLAB code 
# is available from 
# http://www.eos.ubc.ca/~rich/#T_Tide 
# in t_tide_v1.3beta.zip
# The T_TIDE toolbox is described in: 
# R. Pawlowicz, B. Beardsley, and S. Lentz, "Classical tidal harmonic analysis 
# including error estimates in MATLAB using T_TIDE", Computers and Geosciences 
# 28 (2002), 929-937.

################################################################################
# read_harmonicsfile is a function to parse a tidal harmonics file taken from 
# the XTide program. 

# The harmonic file must be a text format, not the binary 
# tcd format that is generally distributed with XTide. To generate this text 
# version of the harmonics.tcd file, you must use the command line tool 
# restore_tide_db found in the tcd-utils package distributed on the XTide site.
# See http://www.flaterco.com/xtide/files.html for downloads.

read_harmonicsfile = function(fid) {
	# Call noncom function to find first line that isn't a comment line
	line = noncom(fid)
	# First line is the number of constituents. Store as a number.
	ncon = as.numeric(line)
	
	# Create a list to hold the parsed tidal constituents
	xtide = list(name = vector(mode = 'character', length = ncon), 
			speed = vector(mode = 'numeric', length = ncon), 
			startyear = 0)
	
	# Run through the list of tidal constituent speeds found in the next section
	# of the harmonics file.
	for (i in 1:ncon) {
		line = noncom(fid)
		# Split line into the constituent name and speed value
		temp = strsplit(line,'[ ]+')
		# Place constituent name in xtide$name
		xtide$name[i] = temp[[1]][1]
		# Place speed in xtide$speed
		xtide$speed[i] = as.numeric(temp[[1]][2])
	}
	
	# Find next non-comment line
	line = noncom(fid)
	
	# Store start year for equilibrium values
	xtide$startyear = as.numeric(line)
	
	# Store number of years available
	nyear = as.numeric(noncom(fid))
	
	# Add new matrices in the xtide list to hold the years worth of equilibrium
	# and nodefactor values that are coming next.
	xtide$equilarg = matrix(0, ncon, nyear)
	xtide$nodefactor = matrix(0, ncon, nyear)
	
	# The next section starts with the constituent name (J1, K1 etc), followed 
	# by numerical values. The number of values following a constituent name 
	# should be equal to nyear, but they are stored 10 values per line. 
	for (i in 1:ncon) {
		# Skip over the constituent name
		line = noncom(fid)
		# Next read in all data values for this constituent, which should be 
		# equal in length to nyear. This will also advance the position in the 
		# file to the end of the section, similar to what readLines does.
		temp = scan(fid, what = numeric(), n = nyear, quiet = TRUE)
		# Write data to xtide$equilarg
		xtide$equilarg[i, ] = temp
	}
	# Skip over the *END* marker line
	line = noncom(fid)
	
	# Store number of years available
	nyear = as.numeric(noncom(fid))
	
	# The next section is similar to the equilibrium arguments section above, 
	# each section starts with a constituent name on a line, then nodefactor 
	# values equal in number to the value stored in nyear. 
	for (i in 1:ncon) {
		# Skip over the constituent name
		line = noncom(fid)
		# Next read in all data values for this constituent, which should be 
		# equal in length to nyear. This will also advance the position in the 
		# file to the end of the section.
		temp = scan(fid, what = numeric(), n = nyear, quiet = TRUE)
		# Write data to xtide$nodefactor
		xtide$nodefactor[i, ] = temp
	}
	# Skip over the *END* marker line
	line = noncom(fid)
	
	######################################
	# That marks the end of the 'congen' section of the harmonics file. The 
	# remainder of the file has the harmonic constants for each individual 
	# reference tidal station, of which there are a few hundred. There are many
	# more tidal sub-stations (non-reference stations) for which predictions are
	# made by adjusting tide estimates from the reference stations in this file.
	# Those sub-station adjustments are kept in a separate xml file created by
	# the restore_tide_db package, and are not dealt with here. 

	# Create a list to hold the harmonic constituents. The list will grow in 
	# length as data for each station is appended. This isn't a very efficient 
	# way to do things, but without knowing the exact number of stations in the 
	# data file when opening the file, this is the most flexible way to do it. 
	xharm = list(station = vector('character',1),
			stationIDnumber = vector('numeric',1),
			units = vector('character',1),
			longitude = vector('numeric',1),
			latitude = vector('numeric',1),
			timezone = vector('numeric',1),
			tzfile = vector('character',1),
			datum = vector('numeric',1),
			A = matrix(0,1,ncon),
			kappa = matrix(0,1,ncon))
	
	
	# Create a counter to index each station that we work through.
	nh = 0
	
	# Search for next line that starts with # ! symbols or is a regular 
	# non-commented line, using the noncom function
#	line = noncom(fid)
	# Search for the first line that contains # station_id: 
	line = noncomST(fid)
	
	# Go through the remainder of the file, and finish when you hit a final
	# empty line (which should be marked -1 by the noncom function).
	while (line != '-1') {
		
		# Start each new station by incrementing the counter
		nh = nh + 1
		
		# Create some empty temporary objects
		units = ''
		longitude = 0
		latitude = 0
		stationIDnumber = NA
		
		# When you hit a line with the # station_id: at the start, parse out
		# the stationID number
		if (substr(line,1,13) == '# station_id:') {
			# Look for numeric digits in the line, this will return -1 if none
		# are found
			testID = regexpr("[[:digit:]]",line)
			if (testID > 0) {
				# If testID has a valid location in it, use that to pull out
				# the rest of the line, which is presumably all digits
				stationIDnumber = as.numeric(substring(line,testID[1]))
				# If the above fails, stationIDnumber will be NA
			}
			# After extracting a station id number, move to the next line
			line = noncom(fid)
		}
		
		
		# When you hit a line with the # ! symbols at the start, parse out the 
		# units and lat/long if available.
		while (substr(line,1,3) == '# !') {
			if (substr(line,4,7) == 'unit') {
				units = strsplit(line, ': ')[[1]][2]
			} else if (substr(line,4,7) == 'long') {
				longitude = as.numeric(strsplit(line, ': ')[[1]][2])
			} else if (substr(line,4,7) == 'lati') {
				latitude = as.numeric(strsplit(line, ': ')[[1]][2])
			}
			# Advance to next line
			line = noncom(fid)
		}
		
		# If the while loop above has finished, we should be on a regular 
		# non-comment line (the station name). 
		if (substr(line,1,1) != "#") {
			xharm$station[nh] = line
		}
		
		line = noncom(fid)
		# Next get offset from GMT. Split the line at the colons
		temp = strsplit(line,':')
		# Extract the minutes and convert to fractions of an hour
		minutes = as.character(as.numeric(temp[[1]][2])/60)
		# Paste the hour and fractional hour together and convert to a number
		xharm$timezone[nh] = as.numeric(paste(temp[[1]][1], minutes, sep = '.'))
		# Keep the time zone code as well.
		xharm$tzfile[nh] = temp[[1]][3]
		
		line = noncom(fid)
		# Get datum, which is either a current speed or a tidal height
		# For NOAA tide stations, this datum is the difference between the 
		# mean sea level and mean lower low water 
		temp = strsplit(line,' +')
		xharm$datum[nh] = as.numeric(temp[[1]][1])
		
		# Write in the units, longitude, latitude if they were found
		xharm$units[nh] = units
		xharm$longitude[nh] = longitude
		xharm$latitude[nh] = latitude
		# Write in the stationID number if it was found
		xharm$stationIDnumber[nh] = stationIDnumber

		# For each constant (total equal to ncon), pull out the A and kappa 
		# values
		tempA = vector('numeric',ncon)
		tempkappa = vector('numeric',ncon)
		for (i in 1:ncon) {
			line = noncom(fid)
			# Split line on spaces or tabs
			temp = strsplit(line,'[ |\t]+')
			# The A value should appear in the 2nd position in temp
			tempA[i] = as.numeric(temp[[1]][2])
			# The kappa value will be the 3rd value in the split string in temp
			tempkappa[i] = as.numeric(temp[[1]][3])
		}
		xharm$A = rbind(xharm$A, tempA)
		xharm$kappa = rbind(xharm$kappa, tempkappa)
		
		# Read next line
		line = noncomST(fid)
		
	} # end of while loop
	
	# This is a cheap hack to remove the row of zeros at the start of the A and
	# kappa matrices that were created as a result of using rbind to grow 
	# those matrices.
	xharm$A = xharm$A[-1,]
	xharm$kappa = xharm$kappa[-1,]
	
	# Rename the attributes of the A and kappa matrices to more descriptive 
	# values
	attr(xharm$A, 'dimnames')[[1]] = xharm$station
	attr(xharm$A, 'dimnames')[[2]] = xtide$name
	attr(xharm$kappa, 'dimnames')[[1]] = xharm$station
	attr(xharm$kappa, 'dimnames')[[2]] = xtide$name
	
	# Combine the two lists for output
	xtide = c(xtide, xharm)
	
	# The combined list is returned to the calling script or function.
	return(xtide)
} # end of read_harmonicsfile function

#########################################################
# noncom is a function to skip over comment lines in the harmonics text file.
# It needs to be supplied with a file connection (fid) passed from the calling 
# function. The output is a character vector of the current (non-comment) line's
# contents.
noncom = function(fid) {
	# Read next line of file
	line = readLines(fid, n = 1)
	# Next check if the line has anyything in it
	if (length(line) > 0) {
		# If the line has contents, test whether the first character is a #
		# and the 3rd character is NOT a !. If this is true (because it only 
		# has a # but no !, then the line is a normal comment line. We want to
		# skip over it in that case, by reading in a new line. The while loop
		# will repeat until it hits a line that starts with # !, at which point
		# the while test will be false and the while loop will quit, causing 
		# this function to return the contents of the line with the # !. Note
		# that it will also test false if the line isn't a comment line, so 
		# all non-comment lines will be returned to the calling function.
		while (substr(line,1,1) == '#' & substr(line,3,3) != '!') {
			# If the first character is a comment character, and the third 
			# character is NOT an exclamation point, this is just a comment 
			# line. Move to the next line.
			line = readLines(fid, n = 1)
			# If the line is empty (end of file) return -1
			if (length(line) == 0) {line = '-1'; break}
		}
	} else line = '-1' # If the line was empty, return -1
	# Return the current line of text
	line
}

############################################################
# noncomST is a function to skip over comment lines in the harmonics text file.
# It is designed to find the station_id line and skip over any other comment
# lines.
# It needs to be supplied with a file connection (fid) passed from the calling 
# function. The output is a character vector of the current (non-comment) line's
# contents.
noncomST = function(fid) {
	# Read next line of file
	line = readLines(fid, n = 1)
	# Next check if the line has anyything in it
	if (length(line) > 0) {
		# If the line has contents, test whether the first character is a #
		# and the 3rd character is NOT a !. If this is true (because it only 
		# has a # but no !, then the line is a normal comment line. We want to
		# skip over it in that case, by reading in a new line. The while loop
		# will repeat until it hits a line that starts with # !, at which point
		# the while test will be false and the while loop will quit, causing 
		# this function to return the contents of the line with the # !. Note
		# that it will also test false if the line isn't a comment line, so 
		# all non-comment lines will be returned to the calling function.
		while (substr(line,1,1) == '#' & substr(line,3,3) != '!') {
			# If the first character is a comment character, and the third 
			# character is NOT an exclamation point, this is just a comment 
			# line. 
			# Check to see if it happens to be a station_id line:
			if (substr(line,3,13) == 'station_id:'){
				# If true, break out of the while loop, so the current 
				# contents of line will be returned to the calling function at
				# the end of this function.
				break
			}
			# Otherwise, move to the next line.
			line = readLines(fid, n = 1)
			# If the next line is empty (end of file) return -1
			if (length(line) == 0) {line = '-1'; break}
		}
	} else line = '-1' # If the line was empty, return -1
	# Return the current line of text
	line
}



