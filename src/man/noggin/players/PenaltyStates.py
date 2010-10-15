from .. import NogginConstants
import man.motion.HeadMoves as HeadMoves
import man.noggin.util.MyMath as MyMath
import PositionConstants as constants #TODO: create a PenaltyConstants file instead

BALL_SEEN_THRESH = 5

def afterPenalty(player):

    if player.firstFrame():
        initPenaltyReloc(player)
        player.brain.tracker.performHeadMove(HeadMoves.LOOK_UP_LEFT)

    if player.brain.ball.framesOn > BALL_SEEN_THRESH:
        #deal with ball and don't worry about loc
        player.brain.tracker.trackBall()
        return player.goLater('gamePlaying')

    if not player.brain.motion.isHeadActive():
        ##looking to the side
        if player.brain.yglp.on or player.brain.ygrp.on:
            #see the goal posts in multiple frames for safety
            seeYellow(player)
            if player.yellowCount == 5:
                setLocInfo(player)
                #now you know where you are!
                return player.goLater('gamePlaying')

        if player.brain.bglp.on or player.brain.bgrp.on:
            #see the goal posts in multiple frames for safety
            seeBlue(player)
            if player.blueCount == 5:
                setLocInfo(player)
                #now you know where you are!
                return player.goLater('gamePlaying')
        """
        Note: the way that yellowCount and blueCount are set, is such that if a
        post of a different color is seen after the first color, the first
        counter will reset. So if I see a blue post in frame 1, nothing in
        frame 2, and a yellow post in frame 3, and nothing in frame 4,
        my counters will be:
        blueCount == 0
        yellowCount == 1
        This should never happen if vision is set correctly, but just in
        case it isn't...
        """
        player.headCount += 1

    #if we are looking left too long
    if player.headCount == 10:
        player.brain.tracker.performHeadMove(HeadMoves.LOOK_UP_RIGHT)

    #if we are looking right too long
    if player.headCount == 20:
        return player.goLater('penaltyRelocalize')

    return player.stay()

def initPenaltyReloc(player):
    player.headCount = 0
    player.yellowCount = 0
    player.blueCount = 0

def seeYellow(player):
    player.blueCount = 0
    player.yellowCount += 1

def seeBlue(player):
    player.yellowCount = 0
    player.blueCount += 1

def setLocInfo(player):
    if player.headCount <= 10:
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
    if player.firstFrame():
        player.setWalk(1, 0, 0)

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
