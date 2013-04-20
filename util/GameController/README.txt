This is the GameController2 developed by team B-Human for the RoboCup SPL.

If there are any question, please contact yuzong@informatik.uni-bremen.de .


########## 1. Executing the Jar ##########

Double-click GameController2.jar or run 

java -jar GameController2.jar [-b <address> | --broadcast <address>]

If no broadcast address is specified, 255.255.255.255 is used.


########## 2. Shortcuts ##########

While the GameController2 is running, you may use the following keys on the keyboard instead of pushing buttons:

Esc		- press it twice to close the GameController2
Delete		- toggle test-mode (everything is legal, every button is visible and enabled)
Backspace	- undo last action

1-5		- player 1-5 on the left side
6-0		- player 1-5 on the right side

The following keys are mapped on the keyboard nearly equal to the buttons on the GUI.

Q	- goal left side
I	- goal right side
A	- out by left side
K	- out by right side
Y	- time-out left side
,	- time-out right side

E	- initial
R	- ready
T	- set
Z	- play
There actually is no key for finish because pushing it by mistake would be troublesome.

D	- pushing
F	- leaving the field
G	- fallen robot
H	- inactive robot
C	- illegal defender
V	- ball holding
B	- playing with hands
N	- request for pickup


########## 3. Misc ##########

The format of the packets the GameController2 broadcasts and receives is defined in the file RoboCupGameControlData.h, which is identical to the one that was used in 2012.
