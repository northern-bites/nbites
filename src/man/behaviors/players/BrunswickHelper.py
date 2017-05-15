from objects import Location, RobotLocation
from ..Say import *


### Transitions ###
def shouldWalkToReadyPosition(player):
	return player.brain.motion.calibrated

def inReadyPosition(player):
	print "nav current state: " + player.brain.nav.currentState
	print "brain counter: " + str(player.brain.counter)
	return player.brain.counter > 1000

def noLongerInReadyPosition(player):
	return False


### Actions

def walkToReadyPosition(brain, number):
	print "going to location" + str(number)
	if number == 1:
		brain.nav.goTo(Location(10, 10))
	elif number == 2:
		brain.nav.goTo(Location(10, 20))
	elif number == 3:
		brain.nav.goTo(Location(10, 30))
	elif number == 4:
		say(Say.IN_SCRIMMAGE, "Going to ten forty")
		brain.nav.goTo(Location(10, 40))
	elif number == 5:
		brain.nav.goTo(Location(10, 50))
	else:
		say(Say.IN_SCRIMMAGE, "Role is not between 1 and 5. Cannot walk to ready position.")