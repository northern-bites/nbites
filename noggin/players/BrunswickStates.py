
###
# Reimplementation of Game Controller States
###

import man.motion as motion
import man.motion.SweetMoves as SweetMoves

def gamePlaying(player):
    return player.goNow('scanFindBall')

def gamePenalized(player):
    return player.stay()

def gameInitial(player):
    if player.firstFrame():
        player.motion.stopBodyMoves()
        player.brain.tracker.stopHeadMoves()
        player.setHeads(0.,0.)
        player.executeMove(SweetMoves.SIT_POS)
    return player.stay()

def gameSet(player):
    if player.brain.ball.on:
        player.brain.tracker.trackBall()
    elif player.brain.ball.framesOff > 3:
        player.brain.tracker.switchTo('scanBall')
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        dummyWalk = motion.WalkCommand(x=0,y=0,theta=0)
        player.motion.setNextWalkCommand(dummyWalk)

    player.brain.tracker.switchTo('locPans')

    return player.stay()

def gameFinished(player):
    if player.firstFrame():
        player.motion.stopBodyMoves()
        player.brain.tracker.stopHeadMoves()
        player.executeMove(SweetMoves.SIT_POS)
    return player.stay()
