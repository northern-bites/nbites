import math

from .. import SweetMoves
from ..headTracker import HeadMoves
from .. import StiffnessModes
from ..navigator import BrunswickSpeeds as speeds
from objects import RelRobotLocation
from ..navigator import Navigator
from ..util import *
import PMotion_proto

@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.runfallController = False
    return player.goNow('controller')

@superState('gameControllerResponder')
def gameReady(player):
	if player.firstFrame:
		player.brain.nav.stand()
	return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    return player.goNow('controller')

@superState('gameControllerResponder')
def gamePenalized(player):
	if player.firstFrame:
		player.brain.nav.stand()
	return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    player.inKickingState = False
    return player.stay()


@superState('gameControllerResponder')
def controller(player):
    command = player.brain.interface.gameState.robot_command
    if command.command_index != player.latestCommandIndex:
        print "nbControl detecting new command: "
        print command.command_index
        player.latestCommandIndex = command.command_index

        if command.walk_command:
            print "interpreting walk command"
            return player.goNow('walkInDirection')
        elif command.do_sweetmove:
        	print "interpreting sweetmoves command"
        	return player.goNow('doASweetMove')
       	elif command.adjust_head:
       		print "interpreting head move command"
        	return player.goNow('turnHead')
       	else:
       		return player.stay()

    return player.stay()


@superState('gameControllerResponder')
def walkInDirection(player):
    command = player.brain.interface.gameState.robot_command

    if player.firstFrame():
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)
    elif command.walk_stop == False:
        print "walking..."
        player.setWalk(command.walk_x,command.walk_y,command.walk_heading)
    elif command.walk_stop:
        print "stopping walk..."
        player.brain.nav.stand()
        return player.goNow('controller')

    return player.stay()

@superState('gameControllerResponder')
def doASweetMove(player):  
    command = player.brain.interface.gameState.robot_command
     
    if player.firstFrame():
    	player.brain.nav.stand()
    	
    	if command.sweetmove_id == 0:
    		player.executeMove(SweetMoves.GOALIE_SQUAT)
    	elif command.sweetmove_id == 1:
    		player.executeMove(SweetMoves.LEFT_SHORT_STRAIGHT_KICK)
    	elif command.sweetmove_id == 2:
    		player.executeMove(SweetMoves.LEFT_STRAIGHT_KICK)
    	elif command.sweetmove_id == 3:
    		player.executeMove(SweetMoves.LEFT_SIDE_KICK)
    	elif command.sweetmove_id == 4:
    		player.executeMove(SweetMoves.LEFT_BIG_KICK)
    	elif command.sweetmove_id == 5: 
    		player.executeMove(SweetMoves.CUTE_KICK_LEFT)
    	else:
    		player.brain.nav.stand()
    		
    return player.goNow('controller')

@superState('gameControllerResponder')
def turnHead(player):
	command = player.brain.interface.gameState.robot_command
	if player.firstFrame(): 
		player.brain.nav.stand()
		player.brain.tracker.lookToAngle(command.adjusted_head_y)
		player.brain.nav.stand()
	return player.goNow('controller')
