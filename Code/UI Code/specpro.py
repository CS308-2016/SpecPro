# Team Id: 6
# Author List: Pratyaksh Sharma
# Filename: specpro.py
# Theme: UART input and overall control logic
# Functions: TimestampMillisec64, SendEmail, ReadCtrValue, MainLoop

from __future__ import print_function
import Image
import pytesseract
import serial
import datetime
import time
import os
import fnmatch
from subprocess import call
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText


def TimestampMillisec64():
	# Function name: TimestampMillisec64
	# Input: void
	# Output: Returns the number of milliseconds passed since Jan 1, 1970
	# Logic: subtracts the millisecs till 1970, 1, 1 from the current millisec time
	# Example call: TimestampMillisec64()
	return int((datetime.datetime.utcnow() - datetime.datetime(1970, 1, 1)).total_seconds() * 1000) 


def SendEmail(speed, number, datestamp, timestamp):
	# Function name: SendEmail
	# Input: speed of the vehicle, number plate of the vehicle, date of the event, time of the event
	# Output: Sends an email with contents populated from given speed, number-plate, date and time. 
	# 		  Uses the iitb.ac.in mail server for sending the email from "pratyaksh@iitb.ac.in"
	#		 to "psharma1707@gmail.com"
	# Logic: Simple usage of the smtplib python module
	# Example Call: SendEmail('123', '234', '01-20-2001', '11:33:22')

	server = smtplib.SMTP("smtp-auth.iitb.ac.in", 25) # Change SMTP server as desired
	server.starttls()
	server.ehlo()
	server.esmtp_features['auth'] = 'LOGIN PLAIN'
	server.login("ldap_username", "ldap_password")	# Replace with your IITB ldap credentials
	
	from_email = "pratyaksh@iitb.ac.in"	# Change as desired
	to_email = ["psharma1707@gmail.com"]	# Change as desired

	# Change subject and email text as desired
	subject = "SpecPro "
	text = "Speed: {} \n Number-Plate: {} \n Date: {} \n\n Time: {}".format(speed, number, datestamp, timestamp)

	mail = MIMEMultipart("alternative")
	mail['Subject'] = subject
	mail['From'] = from_email
	mail.attach(MIMEText(text, 'plain'))

	for to in to_email:
	    mail['To'] = to
	    server.sendmail(from_email, to, mail.as_string())

	server.close()


def ReadCtrValue():
	# Function name: ReadCtrValue
	# Input: void
	# Output: integer value saved in ctr.txt
	# Logic: ctr.txt file stores an integer giving the number of images/entries that are
	# 		already present in our database. ReadCtrValue() reads this integer from the
	# 		file ctr.txt. If the file does not exist or is empty, 0 is returned.
	# Example call: ReadCtrValue()

	try:
		f = open('ctr.txt', 'r')
		line = f.readlines()[0].strip()
		f.close()
		return int(line)
	except:
		return 0


def MainLoop():
	# Function name: MainLoop
	# Input: void
	# Output: Debugging output is printed when a car crosses the SpecPro sensors
	# Logic: This function contains the main control logic of SpecPro.
	#		See inline comments for detailed logic.
	# Example call: MainLoop()

	# Run the script to start the webcam and start capturing images.
	call(['sh', 'webcam.sh'])

	# Serial port configuration. Change port as desired.
	# See output of `dmesg | grep tty` to find the correct serial port.
	ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200, bytesize=8, parity=u'N', stopbits=1)

	# img/data.txt is a tabulated file which stores the information about all 
	# previous entries (numberplate/image/speed/timestamp etc.) in SpecPro.  
	# This file is directly used by the SpecPro GUI to keep track of events in 
	# real time. When a car is detected to have overspeeded, an entry is appended
	# to this file.
	dataf = open('./img/data.txt', 'a+', 0)

	# Read lastIndex value from ctr.txt
	lastIndex = ReadCtrValue()

	# Main loop for taking serial input
	while True:
		# Open ctr.txt for writing
		ctrf = open('ctr.txt', 'w', 0)

		# Take serial input. The TIVA board sends the number of cycles elapsed 
		# between the car's crossing of the two IR sensors.
		#cycles = ser.readline()
		time.sleep(2)
		cycles = "719"
		time.sleep(2)
		# Save the millisecond at which this event occurred
		event_time_ms = TimestampMillisec64()
		
		# Split the event_time_ms into second part and millisecond part
		ms_part = event_time_ms % 1000
		ts = str(time.strftime('%d-%m-%Y %H:%M:%S', time.localtime(int(round((event_time_ms)/1000))-0)))
		print("reached here")
		# Pause execution (disk I/O flush for images captured by webcam takes some time)
		time.sleep(1)

		# Get the prefix of the images saved from the time stamp of the event
		if (ms_part < 500):
			prefix = time.strftime('%Y_%m_%d_%H_%M_%S_picture_', time.localtime(int(round((event_time_ms)/1000))-1))
			ms_part += 500
		else:
			prefix = time.strftime('%Y_%m_%d_%H_%M_%S_picture_', time.localtime(int(round((event_time_ms)/1000))-0))
			ms_part -= 500

		# Collect the image files captured in 1s interval before the event time
		filelist = []
		
		# Collect all files that match the regex prefix*
		preregx = prefix + '*'
		for file in os.listdir('/tmp/images/'):
			if fnmatch.fnmatch(file, preregx):
				filelist.append(file)

		# list of serial numbers of images that are relevant to the event
		serlist = [int(file[len(prefix):len(prefix)+9]) for file in filelist]

		# Pick one image in the list obtained above
		imagename = prefix + str(serlist[int(round(ms_part*len(serlist)/1000))]).zfill(9) + '.jpg'

		# Move the correct image to a local directory
		source = '/tmp/images/' + imagename
		dest = './img/'+str(lastIndex)+'.jpg'
		os.rename(source, dest)

		# Run the cut.py script to crop the number plate from the image
		call(['python', 'cut.py', dest])

		# Save the cropped image
		os.rename('my.jpg', './img/'+str(lastIndex)+'-cut.jpg')
		
		# Save the binarized image
		os.rename('my2.jpg', './img/'+str(lastIndex)+'-bin.jpg')

		# Sharpen the binarized image
		call(['convert', './img/'+str(lastIndex)+'-bin.jpg', '-sharpen', '3x5', './img/'+str(lastIndex)+'-sharp.jpg'])

		# Run tesseract OCR on the sharpened image
		call(['tesseract', './img/'+str(lastIndex)+'-sharp.jpg', 'out', '-psm', '7', 'nobatch', 'digits'])
		
		# Read the tesseract output
		f = open('out.txt')
		try:
			num_plate = f.readlines()[0]
		except:
			num_plate = ''
		f.close()

		# Save the tesseract output in the img directory
		os.rename('out.txt', './img/' + str(lastIndex) + '.txt')

		# Send email with the correct contents
		try:
			SendEmail(cycles, num_plate, time.strftime('%d-%m-%Y', time.localtime(int(round((event_time_ms)/1000)))),
				time.strftime('%H:%M:%S', time.localtime(int(round((event_time_ms)/1000)))))
		except:
			pass

		# Print the 'cycles' completed between the crossing of the two IR sensors by the car, for debugging only
		print(cycles)

		# Clean the tesseract output
		num_plate = num_plate.strip()
		if(len(num_plate)>3 and num_plate[len(num_plate)-1]=='1'):
			num_plate=num_plate[0:len(num_plate)-2]
		if(len(num_plate)>3 and num_plate[0]=='1'):
			num_plate=num_plate[1:]

		# Clean the cycles read from serial UART
		cycles = cycles.strip()

		# Append an entry to the data.txt file
		print('{}|{}|0.{} kmph|{}|{}|{}'.format(ts, num_plate, cycles, str(lastIndex) + '.jpg', str(lastIndex)+'-cut.jpg', str(lastIndex)+'-sharp.jpg'), file=dataf)
		lastIndex += 1

		# write lastIndex to file
		print(lastIndex, file=ctrf)
		ctrf.close()


if __name__ == "__main__":
	MainLoop()
