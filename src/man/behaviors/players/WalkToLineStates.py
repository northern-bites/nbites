import NavStates.py
import vision

def gameSet(player)
	if player.firstFrame():
		player.brain.nav.stand()
	return player.stay()

def gamePlaying(player):
	if player.firstFrame():
		player.gainsOn()
		player.brain.nav.stand()
		player.brain.tracker.lookToAngleFixedPitch(0) #look straight ahead
	
	while (not player.brain.motion.calibrated()):
		return player.stay()
	
	return player.goNow('walkToLine')

def walkToLine(player):
	line = player.brain.vision.line #Distance from the line
	if (not line.vis.on)
		print "I don't see a line!"
		return player.stay()
	else:
		player.brain.nav.walk(line.r,0,0) #walk distance away from line
		return player.goNow('gameSet')

