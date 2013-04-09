from ..headTracking.HeadMoves import (FIXED_PITCH_LEFT_SIDE_PAN,
                                      FIXED_PITCH_RIGHT_SIDE_PAN,
                                      FIXED_PITCH_PAN,
                                      FIXED_PITCH_SLOW_GOALIE_PAN)
#from vision import certainty
from ..navigator import Navigator as nav
from ..util import Transition
#import goalie
from GoalieConstants import RIGHT, LEFT, UNKNOWN
import GoalieTransitions
from objects import RelRobotLocation, RelLocation
from noggin_constants import LINE_CROSS_OFFSET, GOALBOX_DEPTH, GOALBOX_WIDTH
#from vision import cornerID as IDs
from math import fabs, degrees
from ..kickDecider import kicks
import noggin_constants as Constants

DEBUG_OBSERVATIONS = False
DEBUG_POSITION = False

def walkToGoal(player):
    """
    Has the goalie walk in the general direction of the goal.
    """
    if player.firstFrame():
        if player.side == RIGHT:
            if (player.brain.gameController.teamColor ==
                Constants.teamColor.TEAM_BLUE):
                player.brain.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                        Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                        Constants.HEADING_UP)
            else:
                player.brain.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                        Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                        Constants.HEADING_DOWN)
        else:
            if (player.brain.gameController.teamColor ==
                Constants.teamColor.TEAM_BLUE):
                player.brain.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                        Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                        Constants.HEADING_UP)
            else:
                player.brain.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                        Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                        Constants.HEADING_DOWN)

    player.brain.nav.goTo(RobotLocation(Constants.FIELD_GREEN_LEFT_SIDELINE_X,
                                        Constants.MIDFIELD_Y,
                                        Constants.HEADING_RIGHT))

    return player.stay()

def dodgeBall(player):
    if player.firstFrame():
        if player.brain.ball.rel_y < 0.0:
            dodgeDestY = player.brain.ball.rel_y + 20.0
        else:
            dodgeDestY = player.brain.ball.rel_y - 20.0
        player.brain.tracker.trackBallFixedPitch()
        dodgeBall.dodgeDest = RelRobotLocation(player.brain.ball.rel_x,
                                           dodgeDestY,
                                           0.0)
        player.brain.nav.goTo(dodgeBall.dodgeDest)

    # update dest based on ball loc
    if player.brain.ball.rel_y < 0.0:
        dodgeBall.dodgeDest.relY = player.brain.ball.rel_y + 20.0
    else:
        dodgeBall.dodgeDest.relY = player.brain.ball.rel_y - 20.0
    dodgeBall.dodgeDest.relX = player.brain.ball.rel_x

    return Transition.getNextState(player, dodgeBall)

def spinAtGoal(player):
    if player.firstFrame():
        spinAtGoal.home = RelRobotLocation(0, 0, 0)
        # Decide which way to rotate based on the way we came from
        if player.side == RIGHT:
            spinAtGoal.home.relH = -90
        else:
            spinAtGoal.home.relH = 90
        player.brain.nav.goTo(spinAtGoal.home,
                              nav.CLOSE_ENOUGH, nav.CAREFUL_SPEED)

        player.brain.tracker.lookToAngle(0.0)

    return Transition.getNextState(player, spinAtGoal)

# clearIt->kickBall->didIKickIt->returnToGoal
def clearIt(player):
    if player.firstFrame():
        player.brain.tracker.trackBallFixedPitch()
        if player.brain.ball.rel_y < 0.0:
            player.side = RIGHT
            player.kick = kicks.RIGHT_STRAIGHT_KICK
        else:
            player.side = LEFT
            player.kick = kicks.LEFT_STRAIGHT_KICK

        kickPose = player.kick.getPosition()
        clearIt.ballDest = RelRobotLocation(player.brain.ball.rel_x -
                                            kickPose[0],
                                            player.brain.ball.rel_y -
                                            kickPose[1],
                                            0.0)

        # WE NEED TO BE ABLE TO DO THIS
        # reset odometry
        #player.brain.motion.resetOdometry()
        clearIt.odoDelay = True
        return player.stay()

    if clearIt.odoDelay:
        clearIt.odoDelay = False
        player.brain.nav.goTo(clearIt.ballDest,
                              nav.CLOSE_ENOUGH,
                              nav.FAST_SPEED)

    kickPose = player.kick.getPosition()
    clearIt.ballDest.relX = player.brain.ball.rel_x - kickPose[0]
    clearIt.ballDest.relY = player.brain.ball.rel_y - kickPose[1]

    return Transition.getNextState(player, clearIt)

def shouldISaveIt(player):
    if player.firstFrame():
        player.brain.tracker.trackBallFixedPitch()
        player.brain.nav.stop()
    return Transition.getNextState(player, shouldISaveIt)

def didIKickIt(player):
    if player.firstFrame():
        player.brain.nav.stop()
    return Transition.getNextState(player, didIKickIt)

def spinToFaceBall(player):
    if player.firstFrame():
        facingDest = RelRobotLocation(0.0, 0.0, 0.0)
        if degrees(player.brain.ball.filter_bearing) < 0.0:
            facingDest.relH = -90
        else:
            facingDest.relH = 90
        player.brain.nav.goTo(facingDest,
                              nav.CLOSE_ENOUGH, nav.CAREFUL_SPEED)

    return Transition.getNextState(player, spinToFaceBall)

def decideLeftSide(player):
    if player.firstFrame():
        player.side = UNKNOWN
        player.brain.tracker.lookToAngle(90)

    return Transition.getNextState(player, decideLeftSide)

def decideRightSide(player):
    if player.firstFrame():
        player.side = UNKNOWN
        player.brain.tracker.lookToAngle(-90)

    return Transition.getNextState(player, decideRightSide)

def returnToGoal(player):
    if player.firstFrame():
        if player.lastDiffState == 'didIKickIt':
            correctedDest =(RelRobotLocation(0.0, 0.0, 0.0 ) -
                            returnToGoal.kickPose)
        else:
            correctedDest = (RelRobotLocation(0.0, 0.0, 0.0) -
                             RelRobotLocation(player.brain.interface.odometry.x,
                                              player.brain.interface.odometry.y,
                                              0.0))

        if fabs(correctedDest.relX) < 5:
            correctedDest.relX = 0.0
        if fabs(correctedDest.relY) < 5:
            correctedDest.relY = 0.0
        if fabs(correctedDest.relH) < 5:
            correctedDest.relH = 0.0

        player.brain.nav.walkTo(correctedDest)

    return Transition.getNextState(player, returnToGoal)

def findGoalboxCorner(player):
    if player.firstFrame():
        player.brain.tracker.repeatHeadMove(FIXED_PITCH_SLOW_GOALIE_PAN)
        player.brain.nav.stop()

    return Transition.getNextState(player, findGoalboxCorner)

def centerAtGoalBasedOnCorners(player):
    if player.firstFrame():
        centerAtGoalBasedOnCorners.home = RelRobotLocation(-10.0, 0.0, 0.0)
        player.brain.nav.goTo(centerAtGoalBasedOnCorners.home,
                              nav.GENERAL_AREA,
                              nav.FAST_SPEED)

    for corner in player.brain.vision.fieldLines.corners:
        # if it is possible that this is the desired corner
        if(centerAtGoalBasedOnCorners.cornerID in corner.possibilities):
            if(centerAtGoalBasedOnCorners.cornerID == IDs.YELLOW_GOAL_LEFT_L
               and corner.visualOrientation < 0 and
               player.brain.vision.fieldEdge.centerDist > 110.0):
                centerAtGoalBasedOnCorners.cornerDirection = corner.bearing
                heading = corner.getRobotGlobalHeadingIfFieldAngleIs(90)
                relX = corner.getRobotRelXIfFieldAngleIs(90)
                relY = corner.getRobotRelYIfFieldAngleIs(90)
            elif(centerAtGoalBasedOnCorners.cornerID ==
                 IDs.YELLOW_GOAL_RIGHT_L and corner.visualOrientation > 0 and
                 player.brain.vision.fieldEdge.centerDist > 110.0):
                centerAtGoalBasedOnCorners.cornerDirection = corner.bearing
                heading = corner.getRobotGlobalHeadingIfFieldAngleIs(0)
                relX = corner.getRobotRelXIfFieldAngleIs(0)
                relY = corner.getRobotRelYIfFieldAngleIs(0)
            else:
                continue

            centerAtGoalBasedOnCorners.home.relH = -heading
            centerAtGoalBasedOnCorners.home.relX = -(GOALBOX_DEPTH + relX)
            if centerAtGoalBasedOnCorners.cornerID == IDs.YELLOW_GOAL_LEFT_L:
                centerAtGoalBasedOnCorners.home.relY = -(GOALBOX_WIDTH/2.0 +
                                                         relY)
            else:
                centerAtGoalBasedOnCorners.home.relY = (GOALBOX_WIDTH/2.0 -
                                                        relY)

            lookTo = RelLocation(-relX, -relY)

            player.brain.tracker.lookToAngle(centerAtGoalBasedOnCorners.cornerDirection)

            break

    # corrections to make nav STOP!
    if fabs(centerAtGoalBasedOnCorners.home.relH) < 5:
        centerAtGoalBasedOnCorners.home.relH = 0

    if fabs(centerAtGoalBasedOnCorners.home.relX) < 10:
        centerAtGoalBasedOnCorners.home.relX = 0

    if fabs(centerAtGoalBasedOnCorners.home.relY) < 10:
        centerAtGoalBasedOnCorners.home.relY = 0

    return Transition.getNextState(player, centerAtGoalBasedOnCorners)

def repositionAfterWhiff(player):
    if player.firstFrame():
        # We need to be able to reset ODO
        #player.brain.motion.resetOdometry()
        if player.brain.ball.rel_y < 0.0:
            player.kick = kicks.RIGHT_STRAIGHT_KICK
        else:
            player.kick = kicks.LEFT_STRAIGHT_KICK

        kickPose = player.kick.getPosition()
        repositionAfterWhiff.ballDest = RelRobotLocation(player.brain.ball.rel_x -
                                                         kickPose[0],
                                                         player.brain.ball.rel_y -
                                                         kickPose[1],
                                                         0.0)
        player.brain.nav.goTo(repositionAfterWhiff.ballDest,
                              nav.CLOSE_ENOUGH,
                              nav.FAST_SPEED)

    # if it took more than 5 seconds, forget it
    if player.counter > 150:
        returnToGoal.kickPose.relX += player.brain.interface.odometry.x
        returnToGoal.kickPose.relY += player.brain.interface.odometry.y
        returnToGoal.kickPose.relH += player.brain.interface.odometry.h

        return player.goLater('returnToGoal')

    kickPose = player.kick.getPosition()
    repositionAfterWhiff.ballDest.relX = (player.brain.ball.rel_x -
                                          kickPose[0])
    repositionAfterWhiff.ballDest.relY = (player.brain.ball.rel_y -
                                          kickPose[1])

    return Transition.getNextState(player, repositionAfterWhiff)
