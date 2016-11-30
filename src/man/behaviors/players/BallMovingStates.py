from ..headTracker import HeadMoves
#import noggin_constants as field
from objects import RelRobotLocation
from .. import SweetMoves
from ..util import *
import PMotion_proto
from ..Say import *
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

        say(Say.IN_DEBUG, "Put me in front of a ball and switch me to game ready!")

        ball = player.brain.ball #get ball info from Brain

        radius = player.brain.ball.radius

        dist = ball.distance

        xPos = ball.x

        yPos = ball.y 

        say(Say.IN_DEBUG, "The ball is " + str(math.floor(dist)) + " centimeters away.")
        
        print  dist
        print  radius 
        print  xPos 
        print  yPos 
    
    '''if  player.brain.vision.line_size() > 0: #if there are lines in frame
         #find distance to line
        dist = player.brain.visionLines(0).inner.r
        
        if(dist > 10): #if not at line yet
            player.brain.nav.walk(.2,0,0) #walk forward
            dist = player.brain.visionLines(0).inner.r #update distance
            print dist
            return player.stay() #Keep walking to line

        else: #if the line is close
            print "close to line, stopping"
            player.brain.nav.stop()
            return player.goNow('gameSet') #MAKE A NEW FUNCTION THAT JUST DOES PLAYER.STAY"'''
    return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    return player.stay()

@superState('gameControllerResponder')
def gamePenalized(player):
    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
