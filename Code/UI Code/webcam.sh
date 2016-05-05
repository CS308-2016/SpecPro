# Team Id: 6
# Author List: Pratyaksh Sharma
# Filename: webcam.sh
# Theme: Webcam script
# Functions: None
# Global Variables: None

#!/bin/bash

# Change directory to the mjpg-streamer directory
cd ./mjpg-streamer/mjpg-streamer-experimental/;

# Delete any previously captured images
rm -f /tmp/images/*

# Create the images folder if it doesn't already exist
mkdir -p /tmp/images/

# Run the mjpg_streamer executable with appropriate arguments
# See https://github.com/jacksonliam/mjpg-streamer/blob/master/mjpg-streamer-experimental/plugins/input_uvc/README.md and
# https://github.com/jacksonliam/mjpg-streamer for documentation
./mjpg_streamer -o "output_file.so -f /tmp/images/ -d 10 " -i "input_uvc.so -y -n -f 1000 -q 100 -d /dev/video0 -r 1280x720" &>webcam.log &

