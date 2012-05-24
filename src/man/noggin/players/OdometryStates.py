import copy

import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
import man.motion.StiffnessModes as StiffnessModes
from ..navigator import NavStates
from objects import RobotLocation, RelRobotLocation

####Change these for picture taking####
FRAME_SAVE_RATE = 1
NUM_FRAMES_TO_SAVE = 150

FRONT_SIDE_SPIN = (((20, 0, 0), 50),
                   ((0, 0, 45), 100),
                   ((20, 0, 0), 100),
                   ((0, 0, -20), 200),
                   ((0, 20, 0), 200),
                   ((0, -15, 0), 200),
                   ((20, 20, 0), 200),
                   ((0.8, 0.8, 0.2), 50))

SPIN_WALK = (((0, 0, 1), 50),
             ((1, 0, 0), 150))


def gameInitial(player):
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.tracker.locPans()
    return player.stay()

def gameSet(player):
    if player.firstFrame():
        player.testCounter = 0
        player.benchmark = FRONT_SIDE_SPIN
        
        my = player.brain.my
        player.my_last_loc = RobotLocation(my.x, my.y, my.h)
        player.my_last_odo = RobotLocation(0, 0, 0)
    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        
        loc = player.brain.loc
        my_current_loc = player.brain.my
        my_current_odo = RobotLocation(loc.lastOdoX, loc.lastOdoY, loc.lastOdoTheta)
        
#        delta_loc = my_current_loc - player.my_last_loc
#        delta_odo = my_current_odo - player.my_last_odo
        
        if (player.testCounter > 0):
            player.printf("Odometry: {0}".format(my_current_odo - player.my_last_odo))
            player.printf("Delta Odo {0}".format(NavStates.walkingTo.deltaDest))
        
        player.my_last_loc = RobotLocation(my_current_loc.x, 
                                           my_current_loc.y,
                                           my_current_loc.h)
        player.my_last_odo = my_current_odo 
        
        if player.testCounter >= len(player.benchmark):
            print "Done!"
            return player.goLater("gamePenalized")
        
        command = player.benchmark[player.testCounter]
        player.testCounter += 1
        player.numTestFrames = command[1]
        walkVector = command[0]
        player.brain.nav.walkTo(RelRobotLocation(*walkVector))
    
    if player.counter == player.numTestFrames:
        return player.goNow('nextCommand')
    
    return player.stay()

def nextCommand(player):
    return player.goLater('gamePlaying')

def gamePenalized(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)
        player.brain.tracker.stopHeadMoves()
    return player.stay()

