from .. import NogginConstants
from . import ChaseBallConstants as ChaseConstants
import man.motion.HeadMoves as HeadMoves
import man.noggin.util.MyMath as MyMath
import PositionConstants as constants

def positionLocalize(player):
    """
    Localize better in order to position
    """
    return player.stay()

def playbookPosition(player):
    """
    Have the robot navigate to the position reported to it from playbook
    """
    brain = player.brain
    nav = brain.nav
    my = brain.my
    ball = brain.ball
    gcState = brain.gameController.currentState

    if player.firstFrame():
        nav.positionPlaybook()

        if gcState == 'gameReady':
            brain.tracker.locPans()
        else:
            brain.tracker.activeLoc()

    return player.stay()


def initialLocalize(player):

    if player.firstFrame():
        player.brain.tracker.performHeadMove(HeadMoves.LOOK_UP_LEFT)

    if player.brain.my.teamColor == NogginConstants.TEAM_RED:
        if player.brain.yglp.on and player.brain.ygrp.on:
            #both post give better loc. They will be unobstructed.
            player.brain.tracker.stopHeadMoves()
            #YOU FOUND YOUR GOAL!
        elif player.brain.bglp.framesOn > 1 or player.brain.bgrp.framesOn > 1:
            #look the other way early
            player.brain.tracker.performHeadMove(HeadMoves.LOOK_UP_RIGHT)
        else:
            #look the other way (Something's wrong)
            player.brain.tracker.performHeadMove(HeadMoves.LOOK_UP_RIGHT)
    else:
        if player.brain.bglp.on and player.brain.bgrp.on:
            #both post give better loc. They will be unobstructed.
            player.brain.tracker.stopHeadMoves()
            #YOU FOUND YOUR GOAL!
        elif player.brain.yglp.framesOn > 1 or player.brain.ygrp.framesOn > 1:
            #look the other way early
            player.brain.tracker.performHeadMove(HeadMoves.LOOK_UP_RIGHT)
        else:
            #look the other way (Something's wrong)
            player.brain.tracker.performHeadMove(HeadMoves.LOOK_UP_RIGHT)

player.brain.loc.resetLocTo(NogginConstants.CENTER_FIELD_X, \
                              NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y, \
                                        -90.0)

    return player.stay()


def relocalize(player):
    if player.firstFrame():
        player.setWalk(constants.RELOC_X_SPEED, 0, 0)

    if player.brain.my.locScore == NogginConstants.GOOD_LOC or \
            player.brain.my.locScore == NogginConstants.OK_LOC:
        player.shouldRelocalizeCounter += 1

        if player.shouldRelocalizeCounter > 30:
            player.shouldRelocalizeCounter = 0
            return player.goLater(player.lastDiffState)

    else:
        player.shouldRelocalizeCounter = 0

    if not player.brain.motion.isHeadActive():
        player.brain.tracker.locPans()

    if player.counter > constants.RELOC_SPIN_FRAME_THRESH:
        direction = MyMath.sign(player.getWalk()[2])
        if direction == 0:
            direction = 1

        player.setWalk(0, 0, constants.RELOC_SPIN_SPEED * direction)

    return player.stay()

