#code adapted from KickerStates, WalkToBallStates and GoalieTransitions

from ..headTracker import HeadMoves
from .. import SweetMoves
from ..util import *
import PMotion_proto

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
    return player.goNow('lineWalk')

@superState('gameControllerResponder')
def gamePenalized(player):
    return player.stay()

@superState('gameControllerResponder')
def lineWalk(player):
    #ball = player.brain.ball #get ball info from Brain
    
    lines = player.brain.visionLines #get line info from Brain

        if player.brain.vision.line_size() == 0: #if no lines in frame
            print "My brain sees no lines right now"
            dest = RelRobotLocation(10, 0, 0)  #Create a destination directly in front of the robot
            player.brain.nav.goTo(dest)  #walk there, i.e. walk forward
            return player.stay()  #Keep walking to line
        
        """elif(lines(0).vis.dist < 30):  #if the ball is close
            print "saw a line, stopping"
            return player.goNow('gameSet') #stop"""

        else:    #otherwise, the line is visible but not too close
            print "see line, going towards it"
            player.brain.nav.goTo(lines(0).loc)  #go to the line location
            return player.stay()   #keep going towards it

"""
        if(not ball.vis.on):  #if the ball is not in frame
            print "no ball"
            dest = RelRobotLocation(10, 0, 0)  #Create a destination directly in front of the robot
            player.brain.nav.goTo(dest)  #walk there, i.e. walk forward
            return player.stay()  #Keep walking to ball

        elif(ball.vis.dist < 30):  #if the ball is close
            print "saw the ball, stopping"
            return player.goNow('gameSet') #stop

        else:    #otherwise, the ball is visible but not too close
            print "see ball, going towards it"
            player.brain.nav.goTo(ball.loc)  #go to the ball's location
            return player.stay()   #keep going towards it
"""

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
