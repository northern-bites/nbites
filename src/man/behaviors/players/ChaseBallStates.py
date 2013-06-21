"""
Here we house all of the state methods used for chasing the ball
"""
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
import DribbleTransitions as dr_trans
from ..navigator import Navigator
from ..kickDecider import HackKickInformation as hackKick
from ..kickDecider import kicks
from objects import RelRobotLocation, Location
from math import fabs
import noggin_constants as nogginConstants
import time

DRIBBLE_ON_KICKOFF = False

def chase(player):
    """
    Super State to determine what to do from various situations
    """

    if transitions.shouldFindBall(player):
        return player.goNow('findBall')

    else:
        return player.goNow('approachBall')

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
    if player.firstFrame():
        player.brain.tracker.trackBall()
        if player.shouldKickOff:
            player.brain.nav.chaseBall(Navigator.QUICK_SPEED, fast = True)
        elif player.penaltyKicking:
            return player.goNow('prepareForPenaltyKick')
        else:
            player.brain.nav.chaseBall(fast = True)

    # if (transitions.shouldFindBall(player)):
    #     return player.goLater('chase')

    # goalCenter = Location(nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X,
    #                               nogginConstants.CENTER_FIELD_Y)
    # ballLocation = Location(player.brain.ball.x, player.brain.ball.y)
    # headingBallToGoalCenter = ballLocation.headingTo(goalCenter)
    # print "headingBallToGoalCenter: ", headingBallToGoalCenter
    # return player.stay()

    if (transitions.shouldPrepareForKick(player) or
        player.brain.nav.isAtPosition()):
        player.inKickingState = True
        if player.shouldKickOff:
            if player.brain.ball.rel_y > 0:
                player.kick = kicks.LEFT_STRAIGHT_KICK
            else:
                player.kick = kicks.RIGHT_STRAIGHT_KICK
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
    return player.goNow('orbitBall')

def orbitBall(player):
    """
    State to orbit the ball
    """
    if player.firstFrame():
        if hackKick.DEBUG_KICK_DECISION:
            print "Orbiting at angle: ",player.kick.h

        if player.kick.h == 0:
            return player.goNow('positionForKick')

        elif player.kick.h < 0:
            #set y vel at 50% speed
            print "Turn to left, move right"
            player.brain.nav.walk(0, -.7, .25)

        elif player.kick.h > 0:
            #set y vel at 50% speed in opposite direction
            print "Turn to right, move left"
            player.brain.nav.walk(0, .7, -.25)

    elif player.brain.nav.isStopped():
        player.shouldOrbit = False
        player.kick.h = 0
        player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
        return player.goNow('positionForKick')

    #all of this is basically the same as in shoot() in hackKickInformation
    goalCenter = Location(nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X,
                                  nogginConstants.CENTER_FIELD_Y)
    ballLocation = Location(player.brain.ball.x, player.brain.ball.y)
    headingBallToGoalCenter = ballLocation.headingTo(goalCenter)
    bearingForKick = headingBallToGoalCenter - player.brain.loc.h

    #the kick was chosen before we came into orbitBall()
    if (player.kick.isStraightKick()):
        orbitBall.desiredHeading = 0 - bearingForKick
    elif player.kick == kicks.RIGHT_SIDE_KICK:
        orbitBall.desiredHeading = 70 - bearingForKick
    elif player.kick == kicks.LEFT_SIDE_KICK:
        orbitBall.desiredHeading = -70 -bearingForKick
    elif (player.kick.isBackKick()):
        if bearingForKick < -125:
            orbitBall.desiredHeading = -180 - bearingForKick
        else:
            orbitBall.desiredHeading = 180 - bearingForKick

    # #debugging
    # if player.counter%20 == 0:
    #     print "desiredHeading is:  | ", orbitBall.desiredHeading
    #     print "ball to goal center:| ", headingBallToGoalCenter
    #     print "player heading:     | ", player.brain.loc.h
    #     print "bearing for kick:   | ", bearingForKick
    #     print "walk is:            |  (",player.brain.nav.getXSpeed(),",",player.brain.nav.getYSpeed(),",",player.brain.nav.getHSpeed(),")"
    #     print "=====================++++++++++"

    #our in-house heading checker is of the opinion that we're pointed in the right direction
    if orbitBall.desiredHeading > -5 and orbitBall.desiredHeading < 5:
        player.stopWalking()
        print "Done orbiting, going to positionForKick"
        player.shouldOrbit = False
        player.kick.h = 0
        player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
        return player.goNow('positionForKick')

    if player.stateTime > 8:
        print "In state orbitBall for too long, switching to chase"
        player.shouldOrbit = False
        player.stopWalking()
        player.inKickingState = False
        return player.goLater('chase')

    #These next three if statements might need some fine tuning
    #ATM that doesn't appear to be the case
    if constants.ORBIT_BALL_DISTANCE < player.brain.ball.distance - 7:
        #We're too far away
        player.brain.nav.setXSpeed(.15)

    if constants.ORBIT_BALL_DISTANCE > player.brain.ball.distance + 1:
        #We're too close
        player.brain.nav.setXSpeed(-.15)

    if (constants.ORBIT_BALL_DISTANCE > player.brain.ball.distance - 1
        and constants.ORBIT_BALL_DISTANCE < player.brain.ball.distance + 1):
        #print "We're at a good distance"
        player.brain.nav.setXSpeed(0)

    if (transitions.shouldFindBallKick(player) or
        transitions.shouldCancelOrbit(player)):
        player.inKickingState = False
        return player.goLater('chase')

    if player.kick.h > 0: # Orbiting clockwise
        if player.brain.ball.rel_y > 2:
            player.brain.nav.setHSpeed(0)
            #print "turn clockwise SLOWER"
        elif player.brain.ball.rel_y < 2:
            player.brain.nav.setHSpeed(-.35)
            #print "turn clockwise FASTER"
        else:
            player.brain.nav.setHSpeed(-.25)
            #print "turn clockwise NORMAL"
    else: # Orbiting counter-clockwise
        if player.brain.ball.rel_y > 2:
            player.brain.nav.setHSpeed(.35)
            #print "turn counterclockwise FASTER"
        elif player.brain.ball.rel_y < 2:
            player.brain.nav.setHSpeed(0)
            #print "turn counterclockwise SLOWER"
        else:
            player.brain.nav.setHSpeed(.25)
            #print "turn clockwise NORMAL"

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

    ball = player.brain.ball
    kick_pos = player.kick.getPosition()
    positionForKick.kickPose = RelRobotLocation(ball.rel_x - kick_pos[0],
                                                ball.rel_y - kick_pos[1],
                                                0)

    if player.firstFrame():
        # Safer when coming from orbit in 1 frame. Still works otherwise, too.
        player.brain.tracker.lookStraightThenTrack()
        #only enque the new goTo destination once and update it afterwards
        player.brain.nav.goTo(positionForKick.kickPose,
                              Navigator.PRECISELY,
                              Navigator.GRADUAL_SPEED,
                              False,
                              Navigator.ADAPTIVE)
    else:
        player.brain.nav.updateDest(positionForKick.kickPose)

    if transitions.shouldFindBallKick(player):
        player.inKickingState = False
        return player.goLater('chase')

    if (transitions.ballInPosition(player, positionForKick.kickPose) or
        player.brain.nav.isAtPosition()):
        print "DEBUG_SUITE: In 'positionForKick', either ballInPosition or nav.isAtPosition. Switching to 'kickBallExecute'."
        player.ballBeforeKick = player.brain.ball
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
        print "DEBUG_SUITE: In 'positionForPenaltyKick', either ballInPosition or nav.isAtPosition. Switching to 'kickBallExecute'."
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
