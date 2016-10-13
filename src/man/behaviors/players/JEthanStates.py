from ..headTracker import HeadMoves
from .. import SweetMoves
from ..util import *
from ..navigator import Navigator
from ..navigator import BrunswickSpeeds as speeds
from objects import RelRobotLocation, Location
import PMotion_proto
from noggin_constants import LineID
import noggin_constants as Constants
import math

globalR = 0.0
globalT = 0.0

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
    return player.goNow('walkToLine1')
    
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


def walkToLine1(player):
    #player.brain.tracker.repeatWideSnapPan()
    lines = player.brain.visionLines
    if player.brain.vision.line_size() > 1:
        print "Number of lines:"
        print player.brain.vision.line_size()
        indexOfClosestLine = 0
        #for i in range(0,player.brain.vision.line_size()):
            #if lines(i).inner.r < lines(indexOfClosestLine).inner.r:
                #indexOfClosestLine = i
        r = lines(indexOfClosestLine).inner.r
        t = lines(indexOfClosestLine).inner.t
        x = r * math.cos(t)
        y = r * math.sin(t)
        #dist = math.sqrt(int(x)^2 + int(y)^2)
        print "outside while"
        print r
        while r > 10:
            print "inside while"
            print r
            #if dist > 1:
                #player.brain.nav.walk(.75,0,0)
            #else:
            player.brain.nav.walk(.5,0,0)
            return player.stay()
            #print "x=" + x + "  y= " +  y
        player.brain.nav.stand()
        if r <= 10:
            return player.goNow('doneWalking')
    return player.stay()




def doneWalking(player): 
    return player.stay()