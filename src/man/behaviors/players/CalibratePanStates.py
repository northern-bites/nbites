import os

from ..headTracker import HeadTracker as tracker
from ..headTracker import TrackingConstants as constants 
from ..util import *

@superState('gameControllerResponder')
def panTop(player):
	# print "Got to pan"
	if player.panIndex < constants.NUMBER_OF_PANS and player.firstFrame():
		player.brain.tracker.helper.executeHeadMove(tracker.HeadMoves.CALIBRATION_TOP_PAN[player.panIndex])
		player.panIndex += 1

	elif player.stateTime >= constants.TIME_OF_PAN*2:
		return player.goLater('logTop')

	return player.stay()

@superState('gameControllerResponder')
def logTop(player):
	if player.firstFrame():
		player.brain.tracker.stopHeadMoves()
		os.environ["LOG_THIS"] = 'top'
		if player.panIndex == constants.NUMBER_OF_PANS:
			player.panIndex = 0

	elif player.stateTime >= constants.TIME_PER_LOG:
		return player.goLater('panTop')

	return player.stay()

@superState('gameControllerResponder')
def panBottom(player):
	# print "Got to pan"
	if player.panIndex < constants.NUMBER_OF_PANS and player.firstFrame():
		player.brain.tracker.helper.executeHeadMove(tracker.HeadMoves.CALIBRATION_BOTTOM_PAN[player.panIndex])
		player.panIndex += 1

	elif player.stateTime >= constants.TIME_OF_PAN*2:
		return player.goLater('logBottom')

	return player.stay()

@superState('gameControllerResponder')
def logBottom(player):
	if player.firstFrame():
		player.brain.tracker.stopHeadMoves()
		os.environ["LOG_THIS"] = 'bottom'
		if player.panIndex == constants.NUMBER_OF_PANS:
			player.panIndex = 0

	elif player.stateTime >= constants.TIME_PER_LOG:
		return player.goLater('panBottom')
		
	return player.stay()