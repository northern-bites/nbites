import noggin_constants as NogginConstants
from ..headTracking import HeadMoves
from   ..util import MyMath
import PositionConstants as constants
import ChaseBallTransitions as transitions
from objects import RelRobotLocation

OBJ_SEEN_THRESH = 5
LOOK_DIR_THRESH = 10

# @Summer 2012: This entire state appears to be a hack for localization.
# Consider removing entirely.
def afterPenalty(player):

    if player.firstFrame():
        # pan for the ball
        player.brain.tracker.repeatWidePanFixedPitch()
        # walk towards your own field cross
        player.brain.nav.walkTo(RelRobotLocation(200,0,0))

    if transitions.shouldChaseBall(player):
        if not player.brain.play.isChaser():
            return player.goLater('playbookPosition')
        else:
            return player.goLater('chase')

    # TODO: Make this work for two yellow goals & resetLoc to one place?
    # Would be great if loc worked. Hacked out for US OPEN 2012
    """
    if not player.brain.motion.head_is_active:
        ##looking to the side
        if player.brain.yglp.on or player.brain.ygrp.on:
            #see the goal posts in multiple frames for safety
            seeYellow(player)
            if player.yellowCount >= OBJ_SEEN_THRESH:
                setLocInfo(player)
                #now you know where you are!
                return player.goLater(gcState)

        if player.brain.bglp.vis.on or player.brain.bgrp.vis.on:
            #see the goal posts in multiple frames for safety
            seeBlue(player)
            if player.blueCount >= OBJ_SEEN_THRESH:
                setLocInfo(player)
                #now you know where you are!
                return player.goLater(gcState)
        player.headCount += 1
    """

    # If done walking forward, start relocalizing normally
    if player.brain.nav.isStopped() or player.counter > 250:
        player.brain.nav.stop()
        return player.goLater('findBall')

    return player.stay()

def initPenaltyReloc(player):
    player.headCount = 0
    player.yellowCount = 0
    player.blueCount = 0

"""
Note: the way that yellowCount and blueCount are set is:  if a
post of a different color is seen after the first color, the first
counter will reset. So if I see a blue post in frame 1, nothing in
frame 2, and a yellow post in frame 3, and nothing in frame 4,
my counters will be:
blueCount == 0
yellowCount == 1
This should never happen if vision is set correctly, but just in
case it isn't...
"""

def seeYellow(player):
    player.blueCount = 0
    player.yellowCount += 1

def seeBlue(player):
    player.yellowCount = 0
    player.blueCount += 1

def setLocInfo(player):
    if player.headCount <= LOOK_DIR_THRESH:
        #looking left
        if player.yellowCount > 0:
            player.brain.resetLocTo(NogginConstants.CENTER_FIELD_X, \
                              NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y, \
                              -90.0)
            return
            #must see blue
        player.brain.resetLocTo(NogginConstants.CENTER_FIELD_X, \
                          NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y, \
                          90.0)
        return
        #must be looking right
    if player.yellowCount > 0:
        player.brain.resetLocTo(NogginConstants.CENTER_FIELD_X, \
                          NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y, \
                          90.0)
        return
        #must see blue
    player.brain.resetLocTo(NogginConstants.CENTER_FIELD_X, \
                      NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y, \
                      -90.0)
    return

def penaltyRelocalize(player):
    """
    Note: This is the old code that I'm using as a back-up in case we can't
    see any goal posts. It may be possible to make this smarter. -Wils
    """
    #@todo: Go back to game playing rather than the gameControllerState?
    gcState = player.brain.gameController.currentState

    if player.firstFrame():
        player.setWalk(1, 0, 0)

    if player.brain.ball.vis.frames_on >= OBJ_SEEN_THRESH:
        player.brain.tracker.trackBallFixedPitch()
        return player.goLater(gcState) #TODO: This won't work.

    if player.brain.my.locScore != NogginConstants.locScore.BAD_LOC:
        player.shouldRelocalizeCounter += 1

        if player.shouldRelocalizeCounter > 30:
            player.shouldRelocalizeCounter = 0
            return player.goLater(gcState) #TODO: This won't work.

    else:
        player.shouldRelocalizeCounter = 0

    if not player.brain.motion.head_is_active:
        player.brain.tracker.repeatWidePanFixedPitch()

    return player.stay()
