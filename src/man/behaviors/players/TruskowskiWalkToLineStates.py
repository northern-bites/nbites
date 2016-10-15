from ..headTracker import HeadMoves
from .. import SweetMoves
from ..navigator import BrunswickSpeeds as speeds
from objects import RelRobotLocation, RobotLocation
from ..util import *
import PMotion_proto
import math

@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.runfallController = False
    return player.stay()

@superState('gameControllerResponder')
def gameReady(player):
    if player.firstFrame():
        player.brain.nav.stand()
    return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    #return player.goNow('kick')
    return player.goNow('walkToFieldLine')

@superState('gameControllerResponder')
def gamePenalized(player):
    return player.stay()

@superState('gameControllerResponder')
def kick(player):
    if player.firstFrame():
        # player.brain.nav.callKickEngine(PMotion_proto.messages.Kick.M_Left_Chip_Shot)
        player.executeMove(SweetMoves.CUTE_KICK_LEFT)

    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()

'''
def walkToBall(player):
    ball = player.brain.ball

    if(not ball.vis.on):
        print "no ball"
        dest = RelRobotLocation(10,0,0)
        player.brain.nav.goTo(dest)
        return player.stay()

    elif(ball.vis.dist < 30):
        print "saw the ball, stopping"
        return player.goNow('gameSet')

    else:
        print "see ball, going towards it"
        player.brain.nav.goTo(ball.loc)
        return player.stay()
'''

#walks a straight line and stops when a field line is adequately close
def walkToFieldLine(player):

    if player.firstFrame():
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)

    else:
        for i in range(0, player.brain.vision.line_size()):
            print "saw a line at " + str(player.brain.visionLines(i).inner.r)
            #check to see if the robot has arrived at the line
            if(player.brain.visionLines(i).inner.r < 20):
                print "arrived at a line"
                return player.goNow('playerStand')

        player.brain.nav.walk(0.1, 0, 0) #move straight ahead        

    '''
    elif player.counter > 30 and player.brain.interface.motionStatus.standing:
        player.brain.nav.stand()
    '''

    return player.stay()

def playerStand(player):
    print "player stopped at field line (hopefully)"
    player.brain.nav.stand()
    return player.stay()
