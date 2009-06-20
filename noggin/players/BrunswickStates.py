from ..playbook.PBConstants import DEFAULT_CHASER_NUMBER
from . import PositionTransitions
from ..WebotsConfig import WEBOTS_ACTIVE

###
# Reimplementation of Game Controller States for pBrunswick
###

def gameReady(player):
    """
    Stand up, and pan for localization
    """

    player.gainsOn()
    player.standup()
    player.brain.tracker.switchTo('locPans')
    return player.goLater('playbookPosition')

def gameSet(player):
    """
    Fixate on the ball, or scan to look for it
    """
    if player.firstFrame():
        #player.brain.resetLocalization()
        player.stopWalking()
        if player.brain.my.playerNumber == DEFAULT_CHASER_NUMBER:
            player.brain.tracker.trackBall()
        else:
            player.brain.tracker.activeLoc()

    return player.stay()

def gamePlaying(player):
    player.gainsOn()
    roleState = player.getRoleState(player.currentRole)
    return player.goNow(roleState)

if WEBOTS_ACTIVE:
    gameInitial=gamePlaying

