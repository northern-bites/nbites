import noggin_constants as NogginConstants
import man.motion.HeadMoves as HeadMoves
import man.noggin.util.MyMath as MyMath
import PositionConstants as constants
from objects import RelRobotLocation

OBJ_SEEN_THRESH = 5
LOOK_DIR_THRESH = 10

# @Summer 2012: This entire state appears to be a hack for localization.
# Consider removing entirely.
def afterPenalty(player):

    gcState = player.brain.gameController.currentState

    if player.firstFrame():
        initPenaltyReloc(player)
        player.brain.tracker.repeatWidePanFixedPitch()
        # walk towards the center of the field
        player.brain.nav.walkTo(RelRobotLocation(2000,0,0))

    if player.brain.ball.vis.framesOn > OBJ_SEEN_THRESH:
        #deal with ball and don't worry about loc
        player.brain.tracker.trackBallFixedPitch()
        return player.goLater(gcState)

    # Would be great if loc worked. Hacked out for US OPEN 2012
    """
    if not player.brain.motion.isHeadActive():
        ##looking to the side
        if player.brain.yglp.vis.on or player.brain.ygrp.vis.on:
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

    # Hacked out for US OPEN 2012
    """
    #if we are looking left too long
    if player.headCount == LOOK_DIR_THRESH:
        player.brain.tracker.performHeadMove(HeadMoves.LOOK_UP_RIGHT)

    #if we are looking right too long
    if player.headCount == 2*LOOK_DIR_THRESH:
        return player.goLater('penaltyRelocalize')
        """

    # If done walking forward, start relocalizing normally
    if player.brain.nav.isStopped() or player.counter > 250:
        player.brain.nav.stop()
        return player.goLater('spinFindBall')


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
            player.brain.loc.resetLocTo(NogginConstants.CENTER_FIELD_X, \
                              NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y, \
                              -90.0)
            return
            #must see blue
        player.brain.loc.resetLocTo(NogginConstants.CENTER_FIELD_X, \
                          NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y, \
                          90.0)
        return
        #must be looking right
    if player.yellowCount > 0:
        player.brain.loc.resetLocTo(NogginConstants.CENTER_FIELD_X, \
                          NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y, \
                          90.0)
        return
        #must see blue
    player.brain.loc.resetLocTo(NogginConstants.CENTER_FIELD_X, \
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

    if player.brain.ball.vis.framesOn >= OBJ_SEEN_THRESH:
        player.brain.tracker.trackBallFixedPitch()
        return player.goLater(gcState)

    if player.brain.my.locScore != NogginConstants.locScore.BAD_LOC:
        player.shouldRelocalizeCounter += 1

        if player.shouldRelocalizeCounter > 30:
            player.shouldRelocalizeCounter = 0
            return player.goLater(gcState)

    else:
        player.shouldRelocalizeCounter = 0

    if not player.brain.motion.isHeadActive():
        player.brain.tracker.repeatWidePanFixedPitch()

    return player.stay()
