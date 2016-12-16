from ..headTracker import HeadMoves
#import noggin_constants as field
from objects import RelRobotLocation
from .. import SweetMoves
from ..util import *
import PMotion_proto
from ..Say import *
import math
import time

HEAD_MOVING = False
HEAD_LEFT = False
HEAD_RIGHT = False
HEAD_STRAIGHT = True

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

        # Empty data structure of ball positions
        player.brain.ballPos = []
        
        #continous wide snap panning (see HeadMoves.py for angles)
        if HEAD_MOVING:
            player.brain.tracker.repeatWideSnapPan()

        #keep head still at left angle used in wide snap pan
        elif HEAD_LEFT:  
            player.brain.tracker.performSideStayLeft()
        
        #keep head still at right angle used in wide snap pan 
        elif HEAD_RIGHT:
            player.brain.tracker.performSideStayRight()
        
        #keep head looking straight ahead (center position in wide snap pan)
        else:
            HEAD_STRAIGHT = True
            player.brain.tracker.performCenterStay()

    if HEAD_MOVING:
        # if the robot sees the ball  
        if player.brain.ball.vis.on:

            #say(Say.IN_SCRIMMAGE, "Put me in front of a ball and switch me to game ready!")

            ball = player.brain.ball #ball info from Brain
            dist = ball.distance #distance from robot to ball in cm
            player.brain.ballPos.append(dist)

            # IN PERFECT WORLD: use robot's localization to find ball poition in world space
            # values updated in gamePlaying, are constants in gameReady
            # xPos = ball.x #ball x location in world space
            # yPos = ball.y #ball y location in world space

            #say(Say.IN_DEBUG, "The ball is " + str(math.floor(dist)) + " centimeters away.")
            
            # print out all the info we are getting! huzzah!
            print  "Distance: " + str(dist) 

    elif player.brain.ball.vis.on and player.brain.frameCounter % 15 == 0:
        ball = player.brain.ball
        dist = ball.distance
        player.brain.ballPos.append(dist)
    
    return player.stay()


@superState('gameControllerResponder')
def gamePenalized(player):
    if player.firstFrame():
        player.brain.tracker.performCenterStay()

        distances = player.brain.ballPos

        # Did the ball move at all??
        counter = 0
        for index in range(1, len(distances)):
            # compare to previous
            if abs(distances[index] - distances[index - 1]) > 1:
                counter += 1

        if float(counter) / len(distances) > 0.5:
            print("Ball has moved! HUZZAH!") 
            say(Say.IN_SCRIMMAGE, "Ball has moved.")
        else:
            print("Ball has not moved!")
            say(Say.IN_SCRIMMAGE, "Ball has not moved.")
        #sanity checks: if one frame to the next, delete
        #check first and last measurements

        print player.brain.ballPos
    return player.stay()


@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
