

import math

import man.motion as motion
from man.corpus.sensors import inst as sensors

###### PLAYER STATES ########
def player(player):
    player.brain.motion.setWalkExtraConfig(5.85,-5.85,0.19,5.0)
    player.brain.motion.setWalkConfig(.04,.015,.04,.3,.018,.025)

    return player.goLater('walkForward')

def walkForward(player):
    if player.firstFrame():
        player.brain.nav.walkStraight()
        player.brain.tracker.switchTo('tracking',player.brain.ball)

    elif player.counter>70:
        print "spinning left now"
        player.brain.nav.stopWalking()
        return player.goLater('spinLeft')



    return player.stay()

def spinLeft(player):
    if player.firstFrame():
        turn = motion.WalkTurn(150.,30)
        player.brain.motion.setNextWalkCommand(turn)

    elif (player.counter>60):
        return player.goLater('walkForward')

    return player.stay()


##### GOALIE STATES
def goalie(player):
    player.brain.motion.setWalkExtraConfig(4.2,-4.2,0.19,5.0)
    player.brain.motion.setWalkConfig(.02,.015,.02,.3,.018,0.014)

    return player.goLater('strafeLeft')

def strafeLeft(player):
    if player.firstFrame():
        walkLeft = motion.WalkSideways(40.,30)
        player.brain.motion.setNextWalkCommand(walkLeft)

    elif not player.brain.motion.isWalkActive():
        return player.goLater('turnAdjust')

    return player.stay()

def turnAdjust(player):
    if player.firstFrame():
        turn = motion.WalkTurn(30.,30)
        player.brain.motion.setNextWalkCommand(turn)

    if player.counter>25:
        return player.goLater('strafeRight')

    return player.stay()

def strafeRight(player):
    if player.firstFrame():
        walkRight = motion.WalkSideways(-40.,30)
        player.brain.motion.setNextWalkCommand(walkRight)

    elif not player.brain.motion.isWalkActive():
        return player.goLater('step')

    return player.stay()

def step(player):
    if player.firstFrame():
        step = motion.WalkStraight(10.,30)
        player.brain.motion.setNextWalkCommand(step)

    if player.counter>25:
        return player.goLater('strafeLeft')

    return player.stay()

"""
Game Controller States
"""
def gamePlaying(player):
    return player.goNow('player')

def gamePenalized(player):
    return player.goNow('stop')

def gameInitial(player):
    return player.goNow('gamePlaying')
#return player.stay()

def gameSet(player):
    return player.stay()

def gameReady(player):
    return player.stay()

def gameFinish(player):
    return player.stay()

