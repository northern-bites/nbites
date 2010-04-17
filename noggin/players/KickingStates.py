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
import ChaseBallTransitions
from math import fabs
from ..util import MyMath
import KickingHelpers as helpers


def getKickInfo(player):
    """
    Decides which kick to use
    """
    player.inKickingState = True

    if player.firstFrame():
        player.brain.tracker.switchTo('stopped')
        player.brain.motion.stopHeadMoves()
        player.kickScan()

        player.kickDecider = KickDecider(player)

    # If scanning, then collect data
    if (player.stateTime < SweetMoves.getMoveTime(HeadMoves.KICK_SCAN)):
        player.kickDecider.collectData(player.brain)
        return player.stay()

    # Done scanning time to act
    player.brain.tracker.trackBall()
    player.kickDecider.calculate()

    if not player.brain.ball.on:
        if player.brain.ball.framesOff < 100: # HACK, should be constant
            return player.stay()
        else :
            player.inKickingState = False
            return player.goLater('scanFindBall')

    return player.goNow('decideKick')

def decideKick(player):

    if ChaseBallTransitions.shouldScanFindBall(player):
        player.inKickingState = False
        return player.goLater('scanFindBall')

    # Get references to the collected data
    myLeftPostBearing =  player.kickDecider.myLeftPostBearing
    myRightPostBearing = player.kickDecider.myRightPostBearing
    oppLeftPostBearing = player.kickDecider.oppLeftPostBearing
    oppRightPostBearing = player.kickDecider.oppRightPostBearing

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

    if abs(player.brain.my.h) > constants.ORBIT_OWN_GOAL_HEADING_THRESH and \
            (helpers.inTopOfField(player) or helpers.inBottomOfField(player) ):
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

        if helpers.inCenterOfField(player):
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

        elif helpers.inTopOfField(player):
            if constants.FACING_SIDELINE_ANGLE < my.h:
                if constants.DEBUG_KICKS: print ("\t\ttop1")
                return player.goLater('kickBallRight')
            elif my.h < -90:
                if constants.DEBUG_KICKS: print ("\t\ttop3")
                return player.goLater('kickBallLeft')
            else :
                if constants.DEBUG_KICKS: print ("\t\ttop4")
                player.bigKick = True
                return player.goLater('kickBallStraight')

        elif helpers.inBottomOfField(player):
            if -constants.FACING_SIDELINE_ANGLE > my.h:
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
        if helpers.inCenterOfField(player):
            if constants.DEBUG_KICKS: print ("\t\tcenterfieldkick")
            if avgMyGoalBearing > 0:
                return player.goLater('kickBallRight')
            else :
                return player.goLater('kickBallLeft')
        elif helpers.inTopOfField(player):
            if constants.DEBUG_KICKS: print ("\t\ttopfieldkick")
            if 90 > avgMyGoalBearing > -30:
                return player.goLater('kickBallRight')
            elif avgMyGoalBearing < -30:
                return player.goLater('kickBallLeft')
            else :
                return player.goLater('kickBallStraight')
        elif helpers.inBottomOfField(player):
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

    ballForeFoot = player.kickDecider.ballForeFoot
    if ballForeFoot == constants.LEFT_FOOT:
        player.chosenKick = SweetMoves.SHORT_QUICK_LEFT_KICK
        return player.goNow('kickBallExecute')

    elif ballForeFoot == constants.RIGHT_FOOT:
        player.chosenKick = SweetMoves.SHORT_QUICK_RIGHT_KICK
        return player.goNow('kickBallExecute')

    elif ballForeFoot == constants.MID_RIGHT:
        player.chosenKick = SweetMoves.SHORT_QUICK_RIGHT_KICK
        return player.goNow('stepForRightFootKick')

    elif ballForeFoot == constants.MID_LEFT:
        player.chosenKick = SweetMoves.SHORT_QUICK_LEFT_KICK
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
    if ballForeFoot == constants.LEFT_FOOT:
        if player.bigKick:
            player.chosenKick = SweetMoves.LEFT_BIG_KICK
        else :
            player.chosenKick = SweetMoves.LEFT_FAR_KICK
        return player.goNow('kickBallExecute')

    elif ballForeFoot == constants.RIGHT_FOOT:
        if player.bigKick:
            player.chosenKick = SweetMoves.RIGHT_BIG_KICK
        else :
            player.chosenKick = SweetMoves.RIGHT_FAR_KICK
        return player.goNow('kickBallExecute')

    elif ballForeFoot == constants.MID_RIGHT:
        if player.bigKick:
            player.chosenKick = SweetMoves.RIGHT_BIG_KICK
        else :
            player.chosenKick = SweetMoves.RIGHT_FAR_KICK
        return player.goNow('stepForRightFootKick')

    elif ballForeFoot == constants.MID_LEFT:
        if player.bigKick:
            player.chosenKick = SweetMoves.LEFT_BIG_KICK
        else :
            player.chosenKick = SweetMoves.LEFT_FAR_KICK
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

def kickBallRightShort(player):
    player.chosenKick = SweetMoves.SHORT_LEFT_SIDE_KICK
    return player.goNow('sideStepForSideKick')

def kickBallLeftShort(player):
    player.chosenKick = SweetMoves.SHORT_RIGHT_SIDE_KICK
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
        player.brain.CoA.setRobotSlowGait(player.brain.motion)
        player.brain.tracker.trackBall()
    ball = player.brain.ball
    if ball.on and player.brain.nav.isStopped():
        player.kickDecider.ballForeWhichFoot()
        ballForeFoot = player.kickDecider.ballForeFoot

        if ballForeFoot == constants.MID_LEFT or \
                ballForeFoot == constants.MID_RIGHT:
            player.stopWalking()
            player.brain.CoA.setRobotGait(player.brain.motion)
            return player.goLater('kickBallExecute')

        elif ballForeFoot == constants.INCORRECT_POS:
            player.brain.CoA.setRobotGait(player.brain.motion)
            return player.goLater('positionForKick')

        targetY = ball.relY
        sY = MyMath.sign(targetY) * constants.SIDE_STEP_MAX_SPEED
        player.setSteps(0, sY, 0, constants.NUM_ALIGN_KICK_STEPS)

    if ChaseBallTransitions.shouldScanFindBall(player):
        player.brain.CoA.setRobotGait(player.brain.motion)
        return player.goLater('scanFindBall')
    return player.stay()

def stepForRightFootKick(player):
    if player.firstFrame():
        player.brain.CoA.setRobotSlowGait(player.brain.motion)
        player.brain.tracker.trackBall()

    ball = player.brain.ball
    if ball.on:
        player.kickDecider.ballForeWhichFoot()
        ballForeFoot = player.kickDecider.ballForeFoot

        if ballForeFoot == constants.RIGHT_FOOT:
            player.stopWalking()
            player.brain.CoA.setRobotGait(player.brain.motion)
            return player.goLater('kickBallExecute')
        elif ballForeFoot == constants.LEFT_FOOT:
            player.stopWalking()
            player.chosenKick = SweetMoves.LEFT_FAR_KICK
            player.brain.CoA.setRobotGait(player.brain.motion)
            return player.goLater('kickBallExecute')

        elif ballForeFoot == constants.INCORRECT_POS:
            player.brain.CoA.setRobotGait(player.brain.motion)
            return player.goLater('positionForKick')
        if player.brain.nav.isStopped():
            targetY = ball.relY - constants.RIGHT_FOOT_CENTER_Y
            sY = MyMath.sign(targetY) * constants.SIDE_STEP_MAX_SPEED
            player.setSteps(0, sY, 0, constants.NUM_ALIGN_KICK_STEPS)

    if ChaseBallTransitions.shouldScanFindBall(player):
        player.brain.CoA.setRobotGait(player.brain.motion)
        return player.goLater('scanFindBall')
    return player.stay()

def stepForLeftFootKick(player):
    if player.firstFrame():
        player.brain.CoA.setRobotSlowGait(player.brain.motion)
        player.brain.tracker.trackBall()

    ball = player.brain.ball
    if ball.on and player.brain.nav.isStopped():
        player.kickDecider.ballForeWhichFoot()
        ballForeFoot = player.kickDecider.ballForeFoot

        if ballForeFoot == constants.LEFT_FOOT:
            player.stopWalking()
            player.brain.CoA.setRobotGait(player.brain.motion)
            return player.goLater('kickBallExecute')
        # switch foot!
        elif ballForeFoot == constants.RIGHT_FOOT:
            player.stopWalking()
            player.chosenKick = SweetMoves.RIGHT_FAR_KICK
            player.brain.CoA.setRobotGait(player.brain.motion)
            return player.goLater('kickBallExecute')

        elif ballForeFoot == constants.INCORRECT_POS:
            player.brain.CoA.setRobotGait(player.brain.motion)
            return player.goLater('positionForKick')

        targetY = ball.relY - constants.LEFT_FOOT_CENTER_Y
        sY = MyMath.sign(targetY) * constants.SIDE_STEP_MAX_SPEED
        player.setSteps(0, sY, 0, constants.NUM_ALIGN_KICK_STEPS)

    if ChaseBallTransitions.shouldScanFindBall(player):
        player.brain.CoA.setRobotGait(player.brain.motion)
        return player.goLater('scanFindBall')
    return player.stay()

def alignOnBallStraightKick(player):
    """
    Align on ball with respect to goal
    """
    if player.firstFrame():
        player.hasAlignedOnce = True
        player.brain.CoA.setRobotSlowGait(player.brain.motion)
        player.brain.nav.orbitAngle(-player.angleToAlign)

    # Deal with ball changed positions?
    elif player.brain.nav.isStopped():
        player.brain.CoA.setRobotGait(player.brain.motion)
        return player.goLater('positionForKick')

    return player.stay()

def kickBallExecute(player):
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)
        player.brain.tracker.trackBall()
        player.executeMove(player.chosenKick)

        if not player.penaltyMadeFirstKick:
            player.penaltyMadeFirstKick = True
        elif not player.penaltyMadeSecondKick:
            player.penaltyMadeSecondKick = True

    if not player.brain.ball.on and \
            player.brain.ball.framesOff > constants.LOOK_POST_KICK_FRAMES_OFF:
        player.lookPostKick()

    if player.stateTime >= SweetMoves.getMoveTime(player.chosenKick):
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

    if player.chosenKick == SweetMoves.LEFT_SIDE_KICK or \
            player.chosenKick == SweetMoves.RIGHT_SIDE_KICK:
        player.inKickingState = False
        return player.goLater('spinFindBall')

    if not player.brain.motion.isHeadActive():
        player.inKickingState = False
        return player.goLater('scanFindBall')
    return player.stay()

def kickAtPosition(player):
    """
    Method to simply kick while standing in position
    Used for a very simple goalie behavior
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.executeStiffness(StiffnessModes.LEFT_FAR_KICK_STIFFNESS)
    if player.counter == 2:
        player.executeMove(SweetMoves.LEFT_FAR_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.LEFT_FAR_KICK):
        player.standup()

        if player.brain.nav.isStopped():
            player.brain.CoA.setRobotGait(player.brain.motion)
            player.currentGait = ChaseBallConstants.FAST_GAIT
            return player.goLater('atPosition')

    return player.stay()

class KickDecider:
    """
    Class to hold all the things we need to decide a kick
    """

    def __init__(self,player):
        self.oppGoalLeftPostBearings = []
        self.oppGoalRightPostBearings = []
        self.myGoalLeftPostBearings = []
        self.myGoalRightPostBearings = []

        self.oppGoalLeftPostDists = []
        self.oppGoalRightPostDists = []
        self.myGoalLeftPostDists = []
        self.myGoalRightPostDists = []

        self.oppLeftPostBearing = None
        self.oppRightPostBearing = None
        self.myLeftPostBearing = None
        self.myRightPostBearing = None

        self.oppLeftPostDist = 0.0
        self.oppRightPostDist = 0.0
        self.myLeftPostDist = 0.0
        self.myRightPostDist = 0.0


        self.sawOwnGoal = False
        self.sawOppGoal = False

        self.player = player
        self.ballForeFoot = constants.LEFT_FOOT

    def collectData(self, info):
        """
        Collect info on any observed goals
        """
        if info.myGoalLeftPost.on:
            if info.myGoalLeftPost.certainty == NogginConstants.SURE:
                self.sawOwnGoal = True
                self.myGoalLeftPostBearings.append(info.myGoalLeftPost.bearing)
                self.myGoalLeftPostDists.append(info.myGoalLeftPost.dist)

        if info.myGoalRightPost.on:
            if info.myGoalRightPost.certainty == NogginConstants.SURE:
                self.sawOwnGoal = True
                self.myGoalRightPostBearings.append(info.myGoalRightPost.bearing)
                self.myGoalRightPostDists.append(info.myGoalRightPost.dist)

        if info.oppGoalLeftPost.on:
            if info.oppGoalLeftPost.certainty == NogginConstants.SURE:
                self.sawOppGoal = True
                self.oppGoalLeftPostBearings.append(info.oppGoalLeftPost.bearing)
                self.oppGoalLeftPostDists.append(info.oppGoalLeftPost.dist)

        if info.oppGoalRightPost.on:
            if info.oppGoalRightPost.certainty == NogginConstants.SURE:
                self.sawOppGoal = True
                self.oppGoalRightPostBearings.append(info.oppGoalRightPost.bearing)
                self.oppGoalRightPostDists.append(info.oppGoalRightPost.dist)

    def calculate(self):
        """
        Get usable data from the collected data
        """
        if len(self.myGoalLeftPostBearings) > 0:
            self.myLeftPostBearing = (sum(self.myGoalLeftPostBearings) /
                                      len(self.myGoalLeftPostBearings))
        if len(self.myGoalRightPostBearings) > 0:
            self.myRightPostBearing = (sum(self.myGoalRightPostBearings) /
                                       len(self.myGoalRightPostBearings))
        if len(self.oppGoalLeftPostBearings) > 0:
            self.oppLeftPostBearing = (sum(self.oppGoalLeftPostBearings) /
                                       len(self.oppGoalLeftPostBearings))
        if len(self.oppGoalRightPostBearings) > 0:
            self.oppRightPostBearing = (sum(self.oppGoalRightPostBearings) /
                                        len(self.oppGoalRightPostBearings))

        if len(self.myGoalLeftPostDists) > 0:
            self.myLeftPostDist = (sum(self.myGoalLeftPostDists) /
                                   len(self.myGoalLeftPostDists))
        if len(self.myGoalRightPostDists) > 0:
            self.myRightPostDist = (sum(self.myGoalRightPostDists) /
                                    len(self.myGoalRightPostDists))
        if len(self.oppGoalLeftPostDists) > 0:
            self.oppLeftPostDist = (sum(self.oppGoalLeftPostDists) /
                                    len(self.oppGoalLeftPostDists))
        if len(self.oppGoalRightPostDists) > 0:
            self.oppRightPostDist = (sum(self.oppGoalRightPostDists) /
                                     len(self.oppGoalRightPostDists))


    # Make sure ball is on before doing this
    def ballForeWhichFoot(self):
        ball = self.player.brain.ball

        if not (constants.MAX_KICK_X > ball.relX > constants.MIN_KICK_X) or \
                not ball.on:
            self.ballForeFoot = constants.INCORRECT_POS

        elif constants.LEFT_FOOT_L_Y > ball.relY >= constants.LEFT_FOOT_R_Y:
            self.ballForeFoot = constants.LEFT_FOOT

        elif constants.LEFT_FOOT_R_Y > ball.relY >= 0:
            self.ballForeFoot = constants.MID_LEFT

        elif 0 > ball.relY > constants.RIGHT_FOOT_L_Y:
            self.ballForeFoot = constants.MID_RIGHT

        elif constants.RIGHT_FOOT_L_Y > ball.relY > constants.RIGHT_FOOT_R_Y:
            self.ballForeFoot = constants.RIGHT_FOOT

        else:
            print "ball in incorrect pos, ball at %.2f, %.2f" % (ball.relX,
                                                                 ball.relY)
            self.ballForeFoot = constants.INCORRECT_POS

    def __str__(self):
        s = ""
        if self.myLeftPostBearing is not None:
            s += ("My left post bearing is: " + str(self.myLeftPostBearing) +
                  " dist is: " + str(self.myLeftPostDist) + "\n")
        if self.myRightPostBearing is not None:
            s += ("My right post bearing is: " + str(self.myRightPostBearing) +
                  " dist is: " + str(self.myRightPostDist) +  "\n")
        if self.oppLeftPostBearing is not None:
            s += ("Opp left post bearing is: " + str(self.oppLeftPostBearing) +
                  " dist is: " + str(self.oppLeftPostDist) + "\n")
        if self.oppRightPostBearing is not None:
            s += ("Opp right post bearing is: " + str(self.oppRightPostBearing)
                  + " dist is: " + str(self.oppRightPostDist) +  "\n")
        if s == "":
            s = "No goal posts observed"
        return s
