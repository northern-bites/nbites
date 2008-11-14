Here are the current instructions for using this code. Currently it works only on linux, since haven't gotten around to compiling our vision library for the mac.


DIRECTIONS (LINUX):
Get, and un tar, the example files from http://robocup.bowdoin.edu/files/random/jnitoolframe.tar  This tarball contains two frames and a color table

Edit `compile` in this directory to correctly set the path to the man directory.
If your tool directory and man directory are in the same folder, you should be
able to leave this alone.

To compile and run the code, type ./compile in this directory.


SAMPLE OUTPUT ( for the given frames is):
Ball Width: 19.000000
Pose Left Hor Y240
Pose Right Hor Y240
Ball Width: 37.000000
Pose Left Hor Y240
Pose Right Hor Y240



FILES:
]0.NFRM]
[1.NFRM] - example frame files with a ball in them
compile - the script to compile and run the main method in java
[lib_sensors.a, libvision.a] - the vision libraries we link against
			       lib_sensors.a has the classes NaoPose and Sensors
			       libvision.a has the classes Vision, Threshold ...
README.txt - This readme
[table.mtb] - example table which recognizes the ball in two frames in this dir
TOOLVisionLink.cpp - the cpp implementation of all the `native` methods located
		     in the TOOLVisionLink.java (i.e. the cpp side of the bridge
		     to vision)
[TOOLVisionLink.h] - Auto generated header (using javah ...) for the cpp file
TOOLVisionLink.java - Java side of the vision bridge. Loads the native library,
		      and calls it with an image to do vision processing.

