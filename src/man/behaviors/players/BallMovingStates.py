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
    if player.firstFrame():
        #continous wide snap panning (see HeadMoves.py for angles)
        # player.brain.tracker.repeatWideSnapPan()

        #keep head still at left angle used in wide snap pan 
        # player.brain.tracker.performSideStayLeft()
        
        #keep head still at right angle used in wide snap pan 
        # player.brain.tracker.performSideStayRight()
        
        #keep head looking straight ahead (center position in wide snap pan)
        player.brain.tracker.performCenterStay()

    # if the robot sees the ball  
    if player.brain.ball.vis.on:

        #say(Say.IN_DEBUG, "Put me in front of a ball and switch me to game ready!")

        ball = player.brain.ball #ball info from Brain

        dist = ball.distance #distance from robot to ball in cm

        #use robot's localization to find ball poition in world space
        #values updated in gamePlaying, are constants in gameReady
        xPos = ball.x #ball x location in world space
        yPos = ball.y #ball y location in world space

        #say(Say.IN_DEBUG, "The ball is " + str(math.floor(dist)) + " centimeters away.")
        
        #print out all the info we are getting! huzzah!
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
