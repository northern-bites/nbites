INTRODUCTION:
The Northern Bites SPL RoboCup team usually has the robots play autonomously; however, sometimes being able to control the robot would be helpful. The team plays with aldebaran nao robots and has places in the top 16 the past year. The purpose of this app is to aid the development of the team by giving us power over the robot that the team has never had.

The purpose of this app is to control movements of the nao robot. The user has the ability to see what robots are online, make the robot walk, perform a sweet move (a soccer move) and control the head turn of the robot.

HOW TO:
In order to control the robot, the phone must be on the same network as the robot and the robot must have 'pControl' installed. When both have been performed the user may connect to the robot on the robot menu by selecting the robot indicated as 'online'. This will give the user a tab bar controller in which the user may choose what they wish the robot would do

Control Walking:
To control the robot walking the user must move the center ball like one would a joystick in a game. The robot will walk until it receives the signal to stop. To make the robot stand the user can also switch states to game ready or game penalized.

Control SweetMove:
To control the sweetmove the user needs to click the desired sweetmove out of the selection and it will be performed.





DESCRIPTION:
This project begins with a split view controller to act as a menu displaying the robots online and offline. If a robot is online, the user may chose said robot. If the user is confused they may read the help menu provided.  Once the choice has been made the user gets brought to a tab bar controller which gives the user the option to choice between controlling walking, sweetmoves, head moves or seeing a help menu. To learn how the user may control the robot they have connected to the user may see the info popovers located in both the walking and sweetmoves view.


REQUIREMENTS:
This code needs to be run on XCode v8 or needs to be installed on an Apple device.

DEVELOPMENT:
This code was developed on XCode V8.1 for the Apple iPhone.

SOURCES:
Eric Chown's UIKit_Extensions
Eric Chown's GravityBehavior
Eric Chown's BezierPathsView

This project was written by Corinne Alini for Eric Chown's Fall 2016 Mobile Computing course at Bowdoin College.
