#
# This file defines the states necessary for kick testing. Each method
# defines a state.
#

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
from man.motion import StiffnessModes

def gameInitial(player):
    player.executeStiffness(StiffnessModes.NO_HEAD_STIFFNESSES)
    return player.stay()

def gameReady(player):
    return player.goLater('standup')
def gameSet(player):
    return player.goLater('standup')
def gamePlaying(player):
    return player.goLater('standup')
def gamePenalized(player):
    return player.goLater('standup')

def standup(player):
    if player.firstFrame():
        
        player.executeStiffness(StiffnessModes.NO_HEAD_STIFFNESSES)
        walkCommand = motion.WalkCommand(x=0,y=0,theta=0)
        player.motion.setNextWalkCommand(walkCommand)

    if player.counter == 1:
        return player.goLater('kickStraight')
    return player.stay()

def kickStraight(player):
    if player.firstFrame():

        player.executeMove(SweetMoves.LEFT_SIDE_KICK)

    if player.counter == 50:
        return player.goLater('done')
    return player.stay()

def done(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)

    return player.stay()
