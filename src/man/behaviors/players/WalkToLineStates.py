from ..navigator import Navigator
from ..headTracker import HeadMoves
from ..navigator import BrunswickSpeeds as speeds
from objects import RobotLocation, RelRobotLocation
from ..util import *

def gameInitial(player):
	if player.firstFrame():
		print "initial"
		player.gainsOn()
		player.brain.nav.stand()
	return player.stay()

def gameReady(player):
	return player.stay()

def gameSet(player):
	return player.stay()

def gamePlaying(player):
	if player.firstFrame():
		print "playing"
	line = player.brain.visionlines(0) #first line
	dest = line.inner.r #distance from line

	while dest != 0:
		player.brain.nav.walk(10,0,0) #walk speed towards line
		line = player.brain.visionLines(0)
		dest - line.inner.r

	player.brain.nav.stand()
		
	return player.stay()
