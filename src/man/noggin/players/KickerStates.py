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
        player.walkPose()

    if player.counter == 1:
        return player.goLater('kickStraight')
    return player.stay()

def kickStraight(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_SQUAT)
    if player.counter == 100:
        return player.goLater('done')
    return player.stay()

def done(player):
    if player.firstFrame():
        #player.walkPose()
        player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
        return player.stay()
    return player.stay()
