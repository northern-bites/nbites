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

def relocalize(player):
    if player.firstFrame():
        player.setWalk(10 , 0, 0)

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

        player.setWalk(0 , 0, constants.RELOC_SPIN_SPEED * direction)

    return player.stay()

def afterPenalty(player):
#Management State

    #Catch in case relocalize is called
    if player.lastDiffState == 'relocalize':
        return player.goLater('gamePlaying')

    if player.lastDiffState == 'penaltyLookLeft':
        return player.goLater('penaltyLookRight')

    if player.lastDiffState == 'penaltyLookRight':
        return player.goLater('penaltyRelocalize')

    return player.goLater('penaltyLookLeft')

def penaltyLookLeft(player):

    if player.firstFrame():
        player.headCount = 0
        player.brain.tracker.performHeadMove(HeadMoves.LOOK_UP_LEFT)

    if not player.brain.motion.isHeadActive():
        ##looking left

        if player.brain.yglp.on or player.brain.ygrp.on:
            #set loc info
            player.brain.loc.resetLocTo(NogginConstants.CENTER_FIELD_X, \
                                        NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y, \
                                        -90.0)
            #now you know where you are!
            return player.goLater('gamePlaying')

        if player.brain.bglp.on or player.brain.bgrp.on:
            #set loc info
            player.brain.loc.resetLocTo(NogginConstants.CENTER_FIELD_X, \
                                        NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y, \
                                        90.0)
            #now you know where you are!
            return player.goLater('gamePlaying')

        if player.brain.ball.on:
            #deal with ball and don't worry about loc
            player.brain.tracker.trackBall()
            return player.goLater('gamePlaying')

        player.headCount += 1

    #if we are looking for too long
    if player.headCount > 10:
        return player.goLater(player.lastDiffState)

    return player.stay()

def penaltyLookRight(player):

    if player.firstFrame():
        #setup counter
        player.headCount = 0
        player.brain.tracker.performHeadMove(HeadMoves.LOOK_UP_RIGHT)

    if not player.brain.motion.isHeadActive():
        ##looking right

        if player.brain.yglp.on or player.brain.ygrp.on:
            #set loc info
            player.brain.loc.resetLocTo(NogginConstants.CENTER_FIELD_X, \
                                        NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y, \
                                        90.0)
            #now you know where you are!
            return player.goLater('gamePlaying')

        if player.brain.bglp.on or player.brain.bgrp.on:
            #set loc info
            player.brain.loc.resetLocTo(NogginConstants.CENTER_FIELD_X, \
                                        NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y, \
                                        -90.0)
            #now you know where you are!
            return player.goLater('gamePlaying')

        if player.brain.ball.on:
            #deal with ball and don't worry about loc
            player.brain.tracker.trackBall()
            return player.goLater('gamePlaying')

        #increment counter
        player.headCount += 1

    #if we are looking for too long
    if player.headCount > 10:
        #no luck
        return player.goLater(player.lastDiffState)

    return player.stay()

def penaltyRelocalize(player):
    if player.firstFrame():
        player.setWalk(10 , 0, 0)

    if player.brain.ball.on:
        player.brain.tracker.trackBall()
        return player.goLater('gamePlaying')

    if player.brain.my.locScore == NogginConstants.GOOD_LOC or \
            player.brain.my.locScore == NogginConstants.OK_LOC:
        player.shouldRelocalizeCounter += 1

        if player.shouldRelocalizeCounter > 30:
            player.shouldRelocalizeCounter = 0
            return player.goLater('gamePlaying')

    else:
        player.shouldRelocalizeCounter = 0

    if not player.brain.motion.isHeadActive():
        player.brain.tracker.locPans()

    if player.counter > constants.RELOC_SPIN_FRAME_THRESH:
        direction = MyMath.sign(player.getWalk()[2])
        if direction == 0:
            direction = 1

        player.setWalk(0 , 0, constants.RELOC_SPIN_SPEED * direction)

    return player.stay()
