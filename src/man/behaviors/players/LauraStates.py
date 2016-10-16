from ..headTracker import HeadMoves
#import noggin_constants as field
from objects import RelRobotLocation
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

        print  player.brain.vision.line_size() #print out number of lines
    
    if  player.brain.vision.line_size() > 0: #if there are lines in frame
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
            return player.goNow('gameSet') #MAKE A NEW FUNCTION THAT JUST DOES PLAYER.STAY
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
