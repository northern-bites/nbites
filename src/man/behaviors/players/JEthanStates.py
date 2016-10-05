from ..headTracker import HeadMoves
from .. import SweetMoves
from ..util import *
from ..navigator import Navigator
from ..navigator import BrunswickSpeeds as speeds
from objects import RelRobotLocation, Location
import PMotion_proto
import math
#import vision
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
        player.gainsOn()
        player.brain.nav.stand()
        player.runfallController = False
    return player.goNow('walkToLine')
    
@superState('gameControllerResponder')
def gameSet(player):
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    if player.firstFrame():
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)
        player.brain.nav.stand()
    #elif player.counter == 1:
        #player.brain.nav.stand()
        #player.brain.tracker.trackBall()
    ball = player.brain.ball
    dest = RelRobotLocation(100,100,0)
    #player.brain.nav.walkTo(dest,SPEED_SEVEN)
    player.brain.nav.walk(10,10,0)
    return player.stay()
    return player.goNow('kick')

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


def walkToLine(player):
    #player.brain.tracker.repeatWideSnapPan()
    lines = player.brain.visionLines
    x = 0
    y = 0
    if player.brain.vision.line_size() > 0:
        print "I SEE A LINE"
        r = lines(0).inner.r
        t = lines(0).inner.t
        x = r*cos(t)
        y = r*sin(t)
        print "x=" + x + "  y= " +  y
        player.brain.nav.walk(x,y,0)
    
        #print "NO LINES"
    return player.stay()
