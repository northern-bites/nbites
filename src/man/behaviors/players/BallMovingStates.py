from ..headTracker import HeadMoves
#import noggin_constants as field
from objects import RelRobotLocation
from .. import SweetMoves
from ..util import *
import PMotion_proto
from ..Say import *
import math
import time

@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.runfallController = False
    return player.stay()

@superState('gameControllerResponder')
def gameReady(player):
    return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):

    if player.brain.frameCounter % 30 == 0:

        #say(Say.IN_DEBUG, "Put me in front of a ball and switch me to game ready!")

        ball = player.brain.ball #get ball info from Brain

        dist = ball.distance

        xPos = ball.x

        yPos = ball.y 

        #say(Say.IN_DEBUG, "The ball is " + str(math.floor(dist)) + " centimeters away.")
        
        print  "Distance: " + str(dist) 
        print  "X: " + str(xPos) 
        print  "Y: " + str(yPos)
    return player.stay()

@superState('gameControllerResponder')
def gamePenalized(player):
    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
