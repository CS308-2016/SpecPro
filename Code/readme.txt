2016 - CS308  Group 6 : Project SpecPro README  
================================================ 
 
Group Info: 
------------ 
+   Prateesh Goyal (120050013)
+   Pratyaksh Sharma (120050019)
+   Ramprakash K (120050083)
+   Viplov Jain (120050084)
 
Project Description 
------------------- 
TODO:
This is a reame template. It is written using markdown syntax. To know more about markdown you can alwats refer to [Daring Fireball](http://daringfireball.net/projects/markdown/basics).  
You can preview how your mardown looks when rendered [here](http://daringfireball.net/projects/markdown/dingus) 
 
Students are requested to use this format for the sake of uniformity and convinience. Also we can parse these files and then index them for easy searching.  
 
Technologies Used 
------------------- 

+   Embedded C 
+   JAVA 
+   Python 
+   OpenCV 
+   Tesseract
+   Mjpg streamer

 
Installation Instructions 
========================= 
 
Installation of tesseract:
sudo apt-get install tesseract-ocr

Installation of MJPG Streamer:
1. git clone https://github.com/jacksonliam/mjpg-streamer
2. cd mjpg-streamer/mjpg-streamer-experimental
3. make; sudo make install
Usage:
./mjpg_streamer -o "output_file.so -f /tmp/images/ -d 10 " -i "input_uvc.so -y -n -f 1000 -q 100 -d /dev/video0 -r 1280x720" &>webcam.log &


 
Demonstration Video 
=========================  
Screencast: https://www.youtube.com/watch?v=-hxPXcVsC_s
Demo: https://www.youtube.com/watch?v=uXh3ykEPQQw

References 
=========== 
 
Please give references to importance resources.  
 
+ [mjpg-streamer GitHub repository](https://github.com/jacksonliam/mjpg-streamer) 
+ [Tesseract Github repository](https://github.com/tesseract-ocr/tesseract)

