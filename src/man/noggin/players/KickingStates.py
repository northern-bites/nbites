"""
Here we house all of the state methods used for kicking the ball
"""

import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
import man.motion.StiffnessModes as StiffnessModes
import KickingConstants as constants
import ChaseBallConstants
from man.noggin.typeDefs.Location import Location, RobotLocation
from .. import NogginConstants
from ..typeDefs.LocationConstants import FACING_SIDELINE_ANGLE
import ChaseBallTransitions
from math import fabs
from ..util import MyMath
import KickingHelpers as helpers

def decideKick(player):

    if ChaseBallTransitions.shouldScanFindBall(player):
        player.inKickingState = False
        return player.goLater('scanFindBall')

    player.printf(player.kickDecider)

    if player.penaltyKicking:
        return player.goNow('penaltyKickBall')

    player.kickObjective = helpers.getKickObjective(player)

    if player.kickObjective == constants.OBJECTIVE_CLEAR:
        return player.goNow('clearBall')
    elif player.kickObjective == constants.OBJECTIVE_SHOOT_FAR or \
            player.kickObjective == constants.OBJECTIVE_SHOOT_CLOSE:
        return player.goNow('shootBall')
    elif player.kickObjective == constants.OBJECTIVE_KICKOFF:
        player.hasKickedOffKick = True
        player.bigKick = False
        return player.goNow('kickBallStraight')
    else :
        return player.goNow('clearBall')

def clearBall(player):
    """
    Get the ball out of our zone!
    """
    # Get references to the collected data
    myLeftPostBearing =  player.kickDecider.myLeftPostBearing
    myRightPostBearing = player.kickDecider.myRightPostBearing
    oppLeftPostBearing = player.kickDecider.oppLeftPostBearing
    oppRightPostBearing = player.kickDecider.oppRightPostBearing

    # Things to do if we saw our own goal
    # Saw the opponent goal
    my = player.brain.my

    if abs(my.h) > constants.ORBIT_OWN_GOAL_HEADING_THRESH and \
            (my.inTopOfField() or my.inBottomOfField() ):
        return player.goLater('orbitBeforeKick')

    if oppLeftPostBearing is not None and \
            oppRightPostBearing is not None:

        avgOppBearing = (oppLeftPostBearing + oppRightPostBearing)/2
        if fabs(avgOppBearing) < constants.ALIGN_FOR_KICK_BEARING_THRESH:
            if constants.DEBUG_KICKS: print ("\t\t Straight 1")
            player.bigKick = True
            return player.goLater('kickBallStraight')

        elif avgOppBearing > constants.ALIGN_FOR_KICK_BEARING_THRESH:
            if constants.DEBUG_KICKS: print ("\t\t Left 5")
            return player.goLater('kickBallLeft')
        elif avgOppBearing < -constants.ALIGN_FOR_KICK_BEARING_THRESH:
            if constants.DEBUG_KICKS: print ("\t\t Right 5")
            return player.goLater('kickBallRight')

    elif player.kickDecider.sawOwnGoal:
        if myLeftPostBearing is not None and myRightPostBearing is not None:
            # Goal in front
            avgMyGoalBearing = (myRightPostBearing + myLeftPostBearing)/2

            if avgMyGoalBearing > 0:
                if constants.DEBUG_KICKS: print ("\t\tright 1")
                return player.goLater('kickBallRight')
            else :
                if constants.DEBUG_KICKS: print ("\t\tleft 1")
                return player.goLater('kickBallLeft')
        else :
            postBearing = 0.0
            if myLeftPostBearing is not None:
                postBearing = myLeftPostBearing
            else :
                postBearing = myRightPostBearing
            if postBearing > 0:
                return player.goLater('kickBallRight')
            else :
                return player.goLater('kickBallLeft')
    else:
        # use localization for kick
        my = player.brain.my

        if my.inCenterOfField():
            if abs(my.h) <= constants.CLEAR_CENTER_FIELD_STRAIGHT_ANGLE:
                if constants.DEBUG_KICKS: print ("\t\tcenter1")
                player.bigKick = True
                return player.goLater('kickBallStraight')
            elif my.h < -constants.CLEAR_CENTER_FIELD_STRAIGHT_ANGLE:
                if constants.DEBUG_KICKS: print ("\t\tcenter2")
                return player.goLater('kickBallLeft')
            elif my.h > constants.CLEAR_CENTER_FIELD_STRAIGHT_ANGLE:
                if constants.DEBUG_KICKS: print ("\t\tcenter3")
                return player.goLater('kickBallRight')

        elif my.inTopOfField():
            if FACING_SIDELINE_ANGLE < my.h:
                if constants.DEBUG_KICKS: print ("\t\ttop1")
                return player.goLater('kickBallRight')
            elif my.h < -90:
                if constants.DEBUG_KICKS: print ("\t\ttop3")
                return player.goLater('kickBallLeft')
            else :
                if constants.DEBUG_KICKS: print ("\t\ttop4")
                player.bigKick = True
                return player.goLater('kickBallStraight')

        elif my.inBottomOfField():
            if -FACING_SIDELINE_ANGLE > my.h:
                if constants.DEBUG_KICKS: print ("\t\tbottom1")
                return player.goLater('kickBallLeft')
            elif my.h > 90:
                if constants.DEBUG_KICKS: print ("\t\tbottom3")
                return player.goLater('kickBallRight')
            else :
                if constants.DEBUG_KICKS: print ("\t\tbottom4")
                player.bigKick = True
                return player.goLater('kickBallStraight')
    player.bigKick = False
    return player.goLater('kickBallStraight')

def shootBallClose(player):
    """
    Slam-dunk!
    """
    my = player.brain.my
    shotAimPoint = helpers.getShotCloseAimPoint(player)
    leftPostBearing = my.getRelativeBearing(player.brain.oppGoalLeftPost)
    rightPostBearing = my.getRelativeBearing(player.brain.oppGoalRightPost)

    # Am I looking between the posts?
    if (rightPostBearing < -constants.KICK_STRAIGHT_POST_BEARING and
        leftPostBearing > constants.KICK_STRAIGHT_POST_BEARING):
        return player.goNow('kickBallStraight')

    leftShotPointBearing = my.getRelativeBearing(constants.SHOOT_AT_LEFT_AIM_POINT)

    rightShotPointBearing = my.getRelativeBearing(constants.SHOOT_AT_RIGHT_AIM_POINT)

    # Turn to the closer shot point
    if fabs(rightShotPointBearing) < fabs(leftShotPointBearing):
        angleToAlign = rightShotPointBearing
    else :
        angleToAlign = leftShotPointBearing

    if constants.SHOOT_BALL_SIDE_KICK_ANGLE > abs(angleToAlign) > \
            constants.SHOOT_BALL_LOC_ALIGN_ANGLE and \
            not player.hasAlignedOnce:
        player.angleToAlign = angleToAlign
        player.bigKick = False
        return player.goNow('alignOnBallStraightKick')
    elif angleToAlign > constants.SHOOT_BALL_SIDE_KICK_ANGLE:
        return player.goNow('kickBallLeft')
    elif angleToAlign < -constants.SHOOT_BALL_SIDE_KICK_ANGLE:
        return player.goNow('kickBallRight')
    else :
        player.bigKick = False
        return player.goLater('kickBallStraight')

def shootBallFar(player):
    """
    From 3 point range!
    """
    my = player.brain.my
    shotAimPoint = helpers.getShotFarAimPoint(player)
    bearingToGoal = my.getRelativeBearing(shotAimPoint)
    if constants.DEBUG_KICKS: print "bearing to goal is ", bearingToGoal
    if constants.SHOOT_BALL_FAR_SIDE_KICK_ANGLE > abs(bearingToGoal) > \
            constants.SHOOT_BALL_FAR_LOC_ALIGN_ANGLE and \
            not player.hasAlignedOnce:
        player.angleToAlign = bearingToGoal
        player.bigKick = True
        return player.goNow('alignOnBallStraightKick')
    elif bearingToGoal > constants.SHOOT_BALL_SIDE_KICK_ANGLE:
        return player.goNow('kickBallLeft')
    elif bearingToGoal < -constants.SHOOT_BALL_SIDE_KICK_ANGLE:
        return player.goNow('kickBallRight')
    else :
        player.bigKick = True
        return player.goNow('kickBallStraight')

def shootBall(player):
    """
    Put it in the hole!
    """
    # Get references to the collected data
    myLeftPostBearing =  player.kickDecider.myLeftPostBearing
    myRightPostBearing = player.kickDecider.myRightPostBearing
    oppLeftPostBearing = player.kickDecider.oppLeftPostBearing
    oppRightPostBearing = player.kickDecider.oppRightPostBearing
    my = player.brain.my

    if oppLeftPostBearing is not None and \
            oppRightPostBearing is not None:

        if (oppRightPostBearing < -constants.KICK_STRAIGHT_POST_BEARING and
            oppLeftPostBearing > constants.KICK_STRAIGHT_POST_BEARING):
            player.bigKick = True
            return player.goLater('kickBallStraight')

        avgOppBearing = (oppLeftPostBearing + oppRightPostBearing)/2
        if fabs(avgOppBearing) < constants.KICK_STRAIGHT_BEARING_THRESH:
            if constants.DEBUG_KICKS: print ("\t\t Straight 1")
            player.bigKick = True
            return player.goLater('kickBallStraight')

        elif fabs(avgOppBearing) < constants.ALIGN_FOR_KICK_BEARING_THRESH and \
                not player.hasAlignedOnce:
            if constants.DEBUG_KICKS: print ("\t\t Align 1")
            player.angleToAlign = avgOppBearing
            player.bigKick = True
            return player.goLater('alignOnBallStraightKick')

        elif avgOppBearing > constants.ALIGN_FOR_KICK_BEARING_THRESH:
            if constants.DEBUG_KICKS: print ("\t\t Left 5")
            return player.goLater('kickBallLeft')
        elif avgOppBearing < -constants.ALIGN_FOR_KICK_BEARING_THRESH:
            if constants.DEBUG_KICKS: print ("\t\t Right 5")
            return player.goLater('kickBallRight')

    elif myLeftPostBearing is not None and myRightPostBearing is not None:

        avgMyGoalBearing = (myRightPostBearing + myLeftPostBearing)/2
        if my.inCenterOfField():
            if constants.DEBUG_KICKS: print ("\t\tcenterfieldkick")
            if avgMyGoalBearing > 0:
                return player.goLater('kickBallRight')
            else :
                return player.goLater('kickBallLeft')
        elif my.inTopOfField(player):
            if constants.DEBUG_KICKS: print ("\t\ttopfieldkick")
            if 90 > avgMyGoalBearing > -30:
                return player.goLater('kickBallRight')
            elif avgMyGoalBearing < -30:
                return player.goLater('kickBallLeft')
            else :
                return player.goLater('kickBallStraight')
        elif my.inBottomOfField():
            if constants.DEBUG_KICKS: print ("\t\tbottomfieldkick")
            if -90 < avgMyGoalBearing < 30:
                return player.goLater('kickBallRight')
            elif avgMyGoalBearing > 30:
                return player.goLater('kickBallLeft')
            else :
                return player.goLater('kickBallStraight')

    # if somehow we didn't return already with our kick choice,
    # use localization for kick
    if player.kickObjective == constants.OBJECTIVE_SHOOT_CLOSE:
        return player.goNow('shootBallClose')
    else :
        return player.goNow('shootBallFar')

def penaltyKickBall(player):
    ball = player.brain.ball

    if not player.penaltyMadeFirstKick:
        return player.goLater('penaltyKickShortDribble')
        #return player.goLater('kickBallStraightShort')
    if not player.penaltyMadeSecondKick:

        ballLoc = RobotLocation(ball.x, ball.y, NogginConstants.OPP_GOAL_HEADING)

        goalLoc = Location( NogginConstants.OPP_GOAL_MIDPOINT[0],
                            NogginConstants.OPP_GOAL_MIDPOINT[1])

        ballBearingToGoal = ballLoc.getRelativeBearing(goalLoc)

        player.angleToAlign = ballBearingToGoal - player.brain.my.h
        if player.angleToAlign < constants.ALIGN_FOR_KICK_MIN_ANGLE:
            player.bigKick = True
            return player.goLater('kickBallStraight')
        else :
            return player.goLater('alignOnBallStraightKick')
    return player.stay()

def kickBallStraightShort(player):
    if player.brain.ball.on:
        player.kickDecider.ballForeWhichFoot()
    elif ChaseBallTransitions.shouldScanFindBall(player):
        player.inKickingState = False
        return player.goLater('scanFindBall')
    else :
        return player.stay()


    player.brain.tracker.trackBall()
    ball = player.brain.ball
    ballForeFoot = player.kickDecider.ballForeFoot
    if (ballForeFoot == constants.LEFT_FOOT or
        ballForeFoot == constants.RIGHT_FOOT):
        player.chosenKick = SweetMoves.DREW_KICK(ball.relX, ball.relY, 150)
        return player.goNow('kickBallExecute')

    elif ballForeFoot == constants.MID_RIGHT:
        player.chosenKick = SweetMoves.DREW_KICK(ball.relX, ball.relY, 150)
        return player.goNow('stepForRightFootKick')

    elif ballForeFoot == constants.MID_LEFT:
        player.chosenKick = SweetMoves.DREW_KICK(ball.relX, ball.relY, 150)
        return player.goNow('stepForLeftFootKick')

    else :                  # INCORRECT_POS
        return player.goLater('positionForKick')

def kickBallStraight(player):
    """
    Kick the ball forward.
    """
    if player.brain.ball.on:
        player.kickDecider.ballForeWhichFoot()
    elif ChaseBallTransitions.shouldScanFindBall(player):
        player.inKickingState = False
        return player.goLater('scanFindBall')
    else :
        return player.stay()

    player.brain.tracker.trackBall()

    ballForeFoot = player.kickDecider.ballForeFoot
    ball = player.brain.ball

    if ballForeFoot == constants.LEFT_FOOT:
        if player.bigKick:
            player.chosenKick = SweetMoves.LEFT_BIG_KICK
        else :
            player.chosenKick = SweetMoves.DREW_KICK(ball.relX, ball.relY, 500)
        return player.goNow('kickBallExecute')

    elif ballForeFoot == constants.RIGHT_FOOT:
        if player.bigKick:
            player.chosenKick = SweetMoves.RIGHT_BIG_KICK
        else :
            player.chosenKick = SweetMoves.DREW_KICK(ball.relX, ball.relY, 500)
        return player.goNow('kickBallExecute')

    elif ballForeFoot == constants.MID_RIGHT:
        if player.bigKick:
            player.chosenKick = SweetMoves.RIGHT_BIG_KICK
        else :
            player.chosenKick = SweetMoves.DREW_KICK(ball.relX, ball.relY, 500)
        return player.goNow('stepForRightFootKick')

    elif ballForeFoot == constants.MID_LEFT:
        if player.bigKick:
            player.chosenKick = SweetMoves.LEFT_BIG_KICK
        else :
            player.chosenKick = SweetMoves.DREW_KICK(ball.relX, ball.relY, 500)
        return player.goNow('stepForLeftFootKick')

    else :                  # INCORRECT_POS
        return player.goLater('positionForKick')


def kickBallLeft(player):
    """
    Kick the ball to the left, with right foot
    """
    player.chosenKick = SweetMoves.RIGHT_SIDE_KICK
    return player.goNow('sideStepForSideKick')

def kickBallRight(player):
    """
    Kick the ball to the right, using the left foot
    """
    player.chosenKick = SweetMoves.LEFT_SIDE_KICK
    return player.goNow('sideStepForSideKick')

def sideStepForSideKick(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()

    # Which foot is the ball in front of?
    if player.brain.ball.on:
        player.kickDecider.ballForeWhichFoot()
        ballForeFoot = player.kickDecider.ballForeFoot

        if ballForeFoot == constants.MID_RIGHT or \
                ballForeFoot == constants.MID_LEFT:
            return player.goNow('kickBallExecute')

        # Ball too far outside to kick with
        elif ballForeFoot == constants.RIGHT_FOOT:
            return player.goNow('alignForSideKick')

        # Ball in front of wrong foot
        elif ballForeFoot == constants.LEFT_FOOT:
            return player.goNow('alignForSideKick')

        # Ball must be in wrong place
        else :
            player.inKickingState = False
            return player.goLater('chase')

    elif player.brain.ball.framesOff > ChaseBallConstants.BALL_OFF_THRESH:
        player.inKickingState = False
        return player.goLater('scanFindBall')
    return player.stay()


def alignForSideKick(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()

    ball = player.brain.ball

    if ball.on and player.brain.nav.isStopped():
        player.kickDecider.ballForeWhichFoot()
        ballForeFoot = player.kickDecider.ballForeFoot

        if ballForeFoot == constants.MID_LEFT or \
                ballForeFoot == constants.MID_RIGHT:
            player.stopWalking()
            return player.goLater('kickBallExecute')

        elif ballForeFoot == constants.INCORRECT_POS:
            return player.goLater('positionForKick')

        targetY = ball.relY
        sY = MyMath.sign(targetY) * constants.SIDE_STEP_MAX_SPEED
        player.setSteps(0, sY, 0, constants.NUM_ALIGN_KICK_STEPS)

    if ChaseBallTransitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    return player.stay()

def stepForRightFootKick(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()

    ball = player.brain.ball
    if ball.on:
        player.kickDecider.ballForeWhichFoot()
        ballForeFoot = player.kickDecider.ballForeFoot

        if ballForeFoot == constants.RIGHT_FOOT:
            player.stopWalking()
            return player.goLater('kickBallExecute')

        elif ballForeFoot == constants.LEFT_FOOT:
            player.stopWalking()
            player.chosenKick = SweetMoves.DREW_KICK(ball.relX, ball.relY, 500)
            return player.goLater('kickBallExecute')

        elif ballForeFoot == constants.INCORRECT_POS:
            return player.goLater('positionForKick')

        if player.brain.nav.isStopped():
            targetY = ball.relY - constants.RIGHT_FOOT_CENTER_Y
            sY = MyMath.sign(targetY) * constants.SIDE_STEP_MAX_SPEED
            player.setSteps(0, sY, 0, constants.NUM_ALIGN_KICK_STEPS)

    if ChaseBallTransitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    return player.stay()

def stepForLeftFootKick(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()

    ball = player.brain.ball
    if ball.on and player.brain.nav.isStopped():
        player.kickDecider.ballForeWhichFoot()
        ballForeFoot = player.kickDecider.ballForeFoot

        if ballForeFoot == constants.LEFT_FOOT:
            player.stopWalking()
            return player.goLater('kickBallExecute')
        # switch foot!
        elif ballForeFoot == constants.RIGHT_FOOT:
            player.stopWalking()
            player.chosenKick = SweetMoves.DREW_KICK(ball.relX, ball.relY, 500)
            return player.goLater('kickBallExecute')

        elif ballForeFoot == constants.INCORRECT_POS:
            return player.goLater('positionForKick')

        targetY = ball.relY - constants.LEFT_FOOT_CENTER_Y
        sY = MyMath.sign(targetY) * constants.SIDE_STEP_MAX_SPEED
        player.setSteps(0, sY, 0, constants.NUM_ALIGN_KICK_STEPS)

    if ChaseBallTransitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    return player.stay()

def alignOnBallStraightKick(player):
    """
    Align on ball with respect to goal
    """
    if player.firstFrame():
        player.hasAlignedOnce = True
        player.brain.nav.orbitAngle(-player.angleToAlign)

    # Deal with ball changed positions?
    elif player.brain.nav.isStopped():
        return player.goLater('positionForKick')

    return player.stay()

def kickBallExecute(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.executeMove(player.brain.kickDecider.currentKick)

        if not player.penaltyMadeFirstKick:
            player.penaltyMadeFirstKick = True
        elif not player.penaltyMadeSecondKick:
            player.penaltyMadeSecondKick = True

    if not player.brain.ball.on and \
            player.brain.ball.framesOff > constants.LOOK_POST_KICK_FRAMES_OFF:
        player.lookPostKick()

    if player.stateTime >= \
            SweetMoves.getMoveTime(player.brain.kickDecider.currentKick):
        return player.goLater('afterKick')

    return player.stay()

def afterKick(player):
    """
    State to follow up after a kick.
    Currently exits after one frame.
    """
    tracker = player.brain.tracker
    chosenKick = player.chosenKick

    # trick the robot into standing up instead of leaning to the side
    if player.firstFrame():
        player.hasAlignedOnce = False
        player.standup()

        if player.penaltyKicking:
            return player.goLater('penaltyKickRelocalize')

        if player.brain.ball.on:
            tracker.trackBall()
        else:
           player.lookPostKick()
        return player.stay()

    if not player.brain.motion.isHeadActive():
        player.inKickingState = False
        return player.goLater('scanFindBall')
    return player.stay()
