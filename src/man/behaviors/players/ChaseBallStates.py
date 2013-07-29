"""
Here we house all of the state methods used for chasing the ball
"""
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
import DribbleTransitions as dr_trans
import BoxTransitions as box_trans
from ..navigator import Navigator
from ..kickDecider import HackKickInformation as hackKick
from ..kickDecider import kicks
from ..util import *
from objects import RelRobotLocation, Location
from math import fabs
import noggin_constants as nogginConstants
import BoxTransitions
import time

DRIBBLE_ON_KICKOFF = False

@switch('approachBall')
@ifSwitch(BoxTransitions.ballNotInBox, 'positionAtHome')
@ifSwitch(transitions.shouldFindBall, 'findBall')
def chase(player):
    """
    Super State to determine what to do from various situations
    """
    pass

def kickoff(player):
    """
    Have the robot kickoff if it needs to kick it.
    Otherwise wait 10 seconds or wait for the ball to move.
    """
    if player.shouldKickOff:
        return player.goNow('chase')

    if player.firstFrame():
        kickoff.ballRelX = player.brain.ball.rel_x
        kickoff.ballRelY = player.brain.ball.rel_y

    if (player.brain.gameController.timeSincePlaying > 10 or
        fabs(player.brain.ball.rel_x - kickoff.ballRelX) >
        constants.KICKOFF_BALL_MOVE_THRESH or
        fabs(player.brain.ball.rel_y - kickoff.ballRelY) >
        constants.KICKOFF_BALL_MOVE_THRESH):
        return player.goNow('chase')

    return player.stay()

kickoff.ballRelX = "the relX position of the ball when we started"
kickoff.ballRelY = "the relY position of the ball when we started"


def approachBall(player):
    if BoxTransitions.ballNotInBox(player):
        return player.goLater('positionAtHome')

    if player.firstFrame():
        player.brain.tracker.trackBall()
        if player.shouldKickOff:
            player.brain.nav.chaseBall(Navigator.QUICK_SPEED, fast = True)
        elif player.penaltyKicking:
            return player.goNow('prepareForPenaltyKick')
        else:
            player.brain.nav.chaseBall(fast = True)

    if transitions.shouldFindBall(player):
        return player.goLater('chase')

    if (transitions.shouldPrepareForKick(player) or
        player.brain.nav.isAtPosition()):

        if player.brain.nav.isAtPosition():
            print "isAtPosition() is causing the bug!"
        else:
            print "shouldPrepareForKick() is causing the bug!"
            print player.brain.ball.distance
            print player.brain.ball.vis.distance

        player.inKickingState = True
        if player.shouldKickOff:
            if player.brain.ball.rel_y > 0:
                player.kick = kicks.LEFT_SHORT_STRAIGHT_KICK
            else:
                player.kick = kicks.RIGHT_SHORT_STRAIGHT_KICK
            return player.goNow('positionForKick')
        else:
            return player.goNow('prepareForKick')
    else:
        return player.stay()

def prepareForKick(player):
    if player.firstFrame():
        prepareForKick.hackKick = hackKick.KickInformation(player.brain)
        player.brain.nav.stand()
        return player.stay()


    if player.brain.ball.distance > constants.APPROACH_BALL_AGAIN_DIST:
        # Ball has moved away. Go get it!
        player.inKickingState = False
        return player.goLater('chase')

    player.kick = prepareForKick.hackKick.shoot()

    if hackKick.DEBUG_KICK_DECISION:
        print str(player.kick)

    if not player.shouldKickOff or DRIBBLE_ON_KICKOFF:
        if dr_trans.shouldDribble(player):
            return player.goNow('decideDribble')

    return player.goNow('orbitBall')

def orbitBall(player):
    """
    State to orbit the ball
    """
    # Calculate relative heading every frame
    relH = player.kick.h - player.brain.loc.h

    # Are we within the acceptable heading range?
    if (relH > -constants.ORBIT_GOOD_BEARING and
        relH < constants.ORBIT_GOOD_BEARING):
        print "STOPPED! Because relH is: ", relH
        player.stopWalking()
        player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
        return player.goNow('positionForKick')

    if (transitions.orbitTooLong(player) or
        transitions.orbitBallTooFar(player)):
        player.stopWalking()
        player.inKickingState = False
        return player.goLater('chase')

    # Set our walk. Nav will make sure that we don't set duplicate speeds.
    if relH < 0:
        if relH < -20:
            player.setWalk(0, 0.7, -0.25)
        else:
            player.setWalk(0, 0.5, -0.15)
    elif relH > 0:
        if relH > 20:
            player.setWalk(0, -0.7, 0.25)
        else:
            player.setWalk(0, -0.5, 0.15)

    # DEBUGGING PRINT OUTS
    if constants.DEBUG_ORBIT and player.counter%20 == 0:
        print "desiredHeading is:  | ", player.kick.h
        print "player heading:     | ", player.brain.loc.h
        print "orbit heading:      | ", relH
        print "walk is:            |  (",player.brain.nav.getXSpeed(),",",player.brain.nav.getYSpeed(),",",player.brain.nav.getHSpeed(),")"
        print "==============================="

    # X correction
    if (constants.ORBIT_BALL_DISTANCE + constants.ORBIT_DISTANCE_FAR <
        player.brain.ball.distance): # Too far away
        player.brain.nav.setXSpeed(.15)
    elif (constants.ORBIT_BALL_DISTANCE - constants.ORBIT_DISTANCE_CLOSE >
          player.brain.ball.distance): # Too close
        player.brain.nav.setXSpeed(-.15)
    elif (constants.ORBIT_BALL_DISTANCE + constants.ORBIT_DISTANCE_GOOD >
          player.brain.ball.distance and constants.ORBIT_BALL_DISTANCE -
          constants.ORBIT_DISTANCE_GOOD < player.brain.ball.distance):
        player.brain.nav.setXSpeed(0)

    # H correction
    if relH < 0: # Orbiting clockwise
        if player.brain.ball.rel_y > 2:
            player.brain.nav.setHSpeed(0)
        elif player.brain.ball.rel_y < -2:
            if relH < -20:
                player.brain.nav.setHSpeed(-0.35)
            else:
                player.brain.nav.setHSpeed(-0.2)
        else:
            if relH < -20:
                player.brain.nav.setHSpeed(-0.25)
            else:
                player.brain.nav.setHSpeed(-0.15)
    else: # Orbiting counter-clockwise
        if player.brain.ball.rel_y > 2:
            if relH > 20:
                player.brain.nav.setHSpeed(0.35)
            else:
                player.brain.nav.setHSpeed(0.2)
        elif player.brain.ball.rel_y < -2:
            player.brain.nav.setHSpeed(0)
        else:
            if relH > 20:
                player.brain.nav.setHSpeed(0.25)
            else:
                player.brain.nav.setHSpeed(0.15)

    return player.stay()

def positionForKick(player):
    """
    Get the ball in the sweet spot
    """
    if (transitions.shouldApproachBallAgain(player) or
        transitions.shouldRedecideKick(player)):
        player.inKickingState = False
        return player.goLater('chase')

    if not player.shouldKickOff or DRIBBLE_ON_KICKOFF:
        if dr_trans.shouldDribble(player):
            return player.goNow('decideDribble')

    if player.corner_dribble:
        return player.goNow('executeDribble')

    ball = player.brain.ball
    kick_pos = player.kick.getPosition()
    positionForKick.kickPose = RelRobotLocation(ball.rel_x - kick_pos[0],
                                                ball.rel_y - kick_pos[1],
                                                0)

    if player.firstFrame():
        player.brain.tracker.lookStraightThenTrack()
        player.brain.nav.destinationWalkTo(positionForKick.kickPose,
                                           Navigator.SLOW_SPEED,
                                           True)
    elif player.brain.ball.vis.on: # don't update if we don't see the ball
        player.brain.nav.updateDestinationWalkDest(positionForKick.kickPose)

    if transitions.shouldFindBall(player):
        player.inKickingState = False
        return player.goLater('chase')

    player.ballBeforeKick = player.brain.ball
    if transitions.ballInPosition(player, positionForKick.kickPose):
        if player.motionKick:
            return player.goNow('motionKickExecute')
        else:
            player.brain.nav.stand()
            return player.goNow('kickBallExecute')

    return player.stay()

def prepareForPenaltyKick(player):
    """
    We're waiting here for a short time to psych out the opposing goalie,
    then turn very slightly if the flag is set.
    """
    if player.firstFrame():
        prepareForPenaltyKick.chase = False
        ball = player.brain.ball
        print "player.stateTime: ", player.stateTime
        #pseudo-random spin decision on which direction to kick
        now = time.time()
        if (int(now) % 2) == 0:
            player.penaltyKickRight = True
        else:
            player.penaltyKickRight = False

        print now
        print "Kicking Right? ", player.penaltyKickRight
        ball = player.brain.ball
        if player.penaltyKickRight:
            location = RelRobotLocation(ball.rel_x - 10, ball.rel_y + 5, 0)
        else:
            location = RelRobotLocation(ball.rel_x - 10, ball.rel_y - 5, 0)
        player.brain.nav.goTo(location, Navigator.PRECISELY, Navigator.MEDIUM_SPEED,
                              False, True, False, False)
    else:
        ball = player.brain.ball
        if player.penaltyKickRight:
            location = RelRobotLocation(ball.rel_x - 10, ball.rel_y + 5, 0)
        else:
            location = RelRobotLocation(ball.rel_x - 10, ball.rel_y - 5, 0)
        player.brain.nav.updateDest(location)

    if prepareForPenaltyKick.chase:
        return player.stay()

    if (transitions.shouldPrepareForKick(player) or
        player.brain.nav.isAtPosition()):
        print "X: ", player.brain.ball.rel_x
        print "Y: ", player.brain.ball.rel_y
        player.brain.nav.stand()
        # prepareForPenaltyKick.chase = True
        return player.goNow('penaltyKickSpin')
    return player.stay()

def penaltyKickSpin(player):
    """
    Spin so that we change the heading of the kick
    """
    if player.firstFrame():
        penaltyKickSpin.speed = Navigator.SLOW_SPEED
        penaltyKickSpin.done = False
        penaltyKickSpin.threshCount = 0
        if player.penaltyKickRight:
            penaltyKickSpin.speed = penaltyKickSpin.speed * -1
        player.brain.nav.walk(0,0, penaltyKickSpin.speed)
        print "Spinning at speed: ", penaltyKickSpin.speed
    if penaltyKickSpin.done:
        print "X: ", player.brain.ball.rel_x
        print "Y: ", player.brain.ball.rel_y
        return player.stay()

    postBearing = player.brain.yglp.bearing_deg

    if not player.penaltyKickRight or player.brain.yglp.certainty == 0:
        if player.brain.yglp.certainty == 0:
            print "I MIGHT be using right gp for left"
        postBearing = player.brain.ygrp.bearing_deg

    if player.penaltyKickRight:
        if postBearing > -12:
            penaltyKickSpin.threshCount += 1
            if penaltyKickSpin.threshCount == 3:
                player.brain.nav.stand()
                print "stopped because right post: ", postBearing
                penaltyKickSpin.done = True
                #return player.stay()
                return player.goNow('positionForPenaltyKick')
        else:
            penaltyKickSpin.threshCount = 0
    else:
        if postBearing < 12:
            penaltyKickSpin.threshCount += 1
            if penaltyKickSpin.threshCount == 3:
                player.brain.nav.stand()
                print "stopped because left post: ", postBearing
                penaltyKickSpin.done = True
                #return player.stay()
                return player.goNow('positionForPenaltyKick')
        else:
            penaltyKickSpin.threshCount = 0

    # print "Left post: ", player.brain.ygrp.bearing_deg
    # print "Right post: ", player.brain.yglp.bearing_deg
    # print "-----------------------------"

    return player.stay()


def positionForPenaltyKick(player):
    """
    We're getting ready for a penalty kick
    """
    if player.firstFrame():
        positionForPenaltyKick.position = True
        player.inKickingState = True
        positionForPenaltyKick.yes = False
        if player.brain.ball.rel_y > 0:
            player.kick = kicks.LEFT_STRAIGHT_KICK
            print "Kicking with left"
        else:
            player.kick = kicks.RIGHT_STRAIGHT_KICK
            print "Kicking with right"

    if (transitions.shouldApproachBallAgain(player) or
        transitions.shouldRedecideKick(player)):
        player.inKickingState = False
        print "Going Back to Chase"
        return player.goLater('chase')

    ball = player.brain.ball
    kick_pos = player.kick.getPosition()
    positionForPenaltyKick.kickPose = RelRobotLocation(ball.rel_x - kick_pos[0],
                                                       ball.rel_y - kick_pos[1],
                                                       0)
    #So we stand and wait for two seconds before actually positioning
    if player.stateTime < 2:
        return player.stay()
    elif positionForPenaltyKick.position:
        player.ballBeforeApproach = player.brain.ball
        player.brain.tracker.lookStraightThenTrack()
        positionForPenaltyKick.position = True
        player.brain.nav.goTo(positionForPenaltyKick.kickPose,
                              Navigator.PRECISELY,
                              Navigator.CAREFUL_SPEED,
                              False,
                              Navigator.ADAPTIVE)
        positionForPenaltyKick.position = False
    else:
        player.brain.nav.updateDest(positionForPenaltyKick.kickPose)

    if (transitions.ballInPosition(player, positionForPenaltyKick.kickPose) or
        player.brain.nav.isAtPosition()):
        positionForPenaltyKick.yes = True
        #return player.stay()
        player.brain.nav.stand()
        return player.goNow('kickBallExecute')

    if positionForPenaltyKick.yes:
        print "ball relX: ", ball.rel_x
        print "ball relY: ", ball.rel_y

    return player.stay()


# Currently not used as of 6/7/13.
# TODO: implement this again?
def lookAround(player):
    """
    Nav is stopped. We want to look around to get better loc.
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.stopHeadMoves() # HACK so that tracker goes back to stopped.
        player.brain.tracker.repeatBasicPan()

    # Make sure we leave this state...
    if player.brain.ball.vis.frames_off > 200:
        return player.goLater('chase')

    if player.brain.tracker.isStopped() and player.counter > 2:
            player.brain.tracker.trackBall()
            player.brain.kickDecider.decideKick()
            if transitions.shouldOrbit(player) and not player.penaltyKicking:
                print "Don't have a kick, orbitting"
                return player.goNow('orbitBall')
            else:
                return player.goLater('chase')

    return player.stay()
