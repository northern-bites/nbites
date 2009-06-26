#
# This file defines the states necessary for kick testing. Each method
# defines a state.
#

import man.motion as motion
import man.motion.SweetMoves as SweetMoves

def gameInitial(player):
    player.gainsOn()
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
        player.gainsOn()
        walkCommand = motion.WalkCommand(x=0,y=0,theta=0)
        player.motion.setNextWalkCommand(walkCommand)

    if player.counter == 1:
        return player.goLater('kickStraight')
    return player.stay()

def kickStraight(player):
    if player.firstFrame():

        player.executeMove(SweetMoves.LEFT_FARTHER_KICK)

    if player.counter == 50:
        return player.goLater('done')
    return player.stay()

def done(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)
        return player.stay()
    if not player.brain.motion.isBodyActive():
        player.gainsOff()

    return player.stay()
