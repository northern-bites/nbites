#
# This file defines the states necessary for kick testing. Each method
# defines a state.
#

import man.motion as motion
import man.motion.HeadMoves as HeadMoves
import man.motion.SweetMoves as SweetMoves

def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.fallController.enableFallProtection(False)
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.brain.fallController.enableFallProtection(False)
    return player.goLater('standup')

def gameSet(player):
    if player.firstFrame():
        player.brain.fallController.enableFallProtection(False)
    return player.goLater('standup')

def gamePlaying(player):
    if player.firstFrame():
        player.brain.fallController.enableFallProtection(False)
    return player.goLater('standup')

def gamePenalized(player):
    if player.firstFrame():
        player.brain.fallController.enableFallProtection(False)
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

        player.executeMove(SweetMoves.STAND_UP_FRONT)

    if player.counter == 50:
        return player.goLater('done')
    return player.stay()

def done(player):
    if player.firstFrame():
        #player.walkPose()
        return player.stay()
    return player.stay()
