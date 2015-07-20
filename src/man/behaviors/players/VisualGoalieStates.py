from ..headTracker.HeadMoves import (FIXED_PITCH_LEFT_SIDE_PAN,
                                      FIXED_PITCH_RIGHT_SIDE_PAN,
                                      FIXED_PITCH_PAN,
                                      FIXED_PITCH_SLOW_GOALIE_PAN)
#from vision import certainty
from ..navigator import Navigator as nav
from ..util import *
#import goalie
from GoalieConstants import RIGHT, LEFT, UNKNOWN
import GoalieConstants as constants
import GoalieTransitions
from objects import RelRobotLocation, RelLocation, Location, RobotLocation
from noggin_constants import (LINE_CROSS_OFFSET, GOALBOX_DEPTH, GOALBOX_WIDTH,
                              FIELD_WHITE_LEFT_SIDELINE_X, CENTER_FIELD_Y,
                              HEADING_LEFT)

#from vision import cornerID as IDs
from math import fabs, degrees, radians, sin, cos
from ..kickDecider import kicks
import GoalieStates as GoalieStates
import noggin_constants as Constants

@superState('gameControllerResponder')
def walkToGoal(player):
    """
    Has the goalie walk in the general direction of the goal.
    """
    if player.firstFrame():
        player.brain.tracker.repeatBasicPan()
        player.returningFromPenalty = False
        player.brain.nav.goTo(RobotLocation(FIELD_WHITE_LEFT_SIDELINE_X,
                                       CENTER_FIELD_Y, 0.0))

    return Transition.getNextState(player, walkToGoal)

#TODO make reactive to ball...
@superState('gameControllerResponder')
def checkSafePlacement(player):
    if player.firstFrame():
        print("First frame of check safe placement")
        player.corners = []
        checkSafePlacement.lastLook = constants.RIGHT
        checkSafePlacement.looking = False
        checkSafePlacement.turnCount = 0
        player.goodRightCornerObservation = False
        player.goodLeftCornerObservation = False

    if not checkSafePlacement.looking and checkSafePlacement.turnCount < 2:
        checkSafePlacement.looking = True
        if checkSafePlacement.lastLook is constants.RIGHT:
            player.brain.tracker.lookToAngle(constants.EXPECTED_LEFT_CORNER_BEARING_FROM_CENTER)
            checkSafePlacement.lastLook = constants.LEFT
        elif checkSafePlacement.lastLook is constants.LEFT:
            player.brain.tracker.lookToAngle(0)
            checkSafePlacement.lastLook = constants.UNKNOWN
        else:
            print("horizon:", player.brain.vision.horizon_dist)
            checkSafePlacement.turnCount += 1
            player.brain.tracker.lookToAngle(constants.EXPECTED_RIGHT_CORNER_BEARING_FROM_CENTER)
            checkSafePlacement.lastLook = constants.RIGHT
    elif checkSafePlacement.turnCount >= 2:
        player.brain.tracker.lookToAngle(0)
    if player.brain.tracker.isStopped():
        checkSafePlacement.looking = False

    # if player.counter > 50:

        #TESTINGCHANGE
    if player.counter > 140 and not player.justDived:
        print("Took too long, assume wrong")
        return player.goLater('watchWithLineChecks')
    elif player.counter > 130 and player.justDived:
        player.justDived = False
        return player.goLater('returnUsingLoc')

        # return player.goLater('watchWithLineChecks')

    return Transition.getNextState(player, checkSafePlacement)

@superState('gameControllerResponder')
def didIKickIt(player):
    if player.firstFrame():
        player.brain.nav.stop()
    return Transition.getNextState(player, didIKickIt)

@superState('gameControllerResponder')
def clearBall(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()

    if player.brain.ball.distance < constants.POSITION_FOR_KICK_DIST:
        # print "Now positioning for kick"
        return player.goNow('positionForGoalieKick')
    elif player.brain.ball.distance < constants.SLOW_DOWN_DIST:
        # print "Slowing down"
        player.brain.nav.chaseBall(nav.MEDIUM_SPEED, fast = True)
    else:
        # print "approaching ball"
        player.brain.nav.chaseBall(nav.FAST_SPEED, fast = True)

    # if player.counter % 2 == 0:
    #     nball = player.brain.naiveBall
    #     ball = player.brain.ball
    #     print "================================="
    #     print("yintercept:", nball.yintercept)
    #     print("Ball dist:", ball.distance)
    #     print("ball.vis.frames_on", ball.vis.frames_on)
    #     print("nb xvel:", nball.x_vel)
    #     print("ball mov vel:", ball.mov_vel_x)

    return Transition.getNextState(player, clearBall)

@superState('gameControllerResponder')
def positionForGoalieKick(player):
    if player.firstFrame():
        player.brain.tracker.lookStraightThenTrack()
        if clearBall.ballSide == RIGHT:
            player.kick = kicks.RIGHT_SHORT_STRAIGHT_KICK
        else:
            player.kick = kicks.LEFT_SHORT_STRAIGHT_KICK
        ball = player.brain.ball
        positionForGoalieKick.kickPose = RelRobotLocation(ball.rel_x - player.kick.setupX,
                                    ball.rel_y - player.kick.setupY,
                                    0)
        print("Kickpose:", positionForGoalieKick.kickPose.relX, positionForGoalieKick.kickPose.relY)
        positionForGoalieKick.speed = nav.GRADUAL_SPEED

        player.brain.nav.goTo(positionForGoalieKick.kickPose,
                                            speed = positionForGoalieKick.speed,
                                            precision = nav.CLOSE_ENOUGH)
    ball = player.brain.ball
    positionForGoalieKick.kickPose = RelRobotLocation(ball.rel_x - player.kick.setupX,
                                    ball.rel_y - player.kick.setupY,
                                    0)
    player.brain.nav.updateDest(positionForGoalieKick.kickPose)

    if GoalieTransitions.ballReadyToKick(player, positionForGoalieKick.kickPose):
        player.brain.nav.stand()
        print("Kickpose:", positionForGoalieKick.kickPose.relX, positionForGoalieKick.kickPose.relY)
        return player.goNow('kickBall')

    return Transition.getNextState(player, positionForGoalieKick)

@superState('gameControllerResponder')
def waitToFaceField(player):
    if player.firstFrame():
        player.brain.tracker.lookToAngle(0)

    return Transition.getNextState(player, waitToFaceField)

@superState('gameControllerResponder')
def repositionAfterWhiff(player):
    if player.firstFrame():
        # reset odometry
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)

        # if player.kick in [kicks.RIGHT_SIDE_KICK, kicks.LEFT_SIDE_KICK]:
        #     pass
        # elif player.brain.ball.rel_y < 0.0:
        #     player.kick = kicks.RIGHT_SHORT_STRAIGHT_KICK
        # else:
        #     player.kick = kicks.LEFT_SHORT_STRAIGHT_KICK

        kickPose = player.kick.getPosition()
        repositionAfterWhiff.ballDest = RelRobotLocation(player.brain.ball.rel_x -
                                                         kickPose[0],
                                                         player.brain.ball.rel_y -
                                                         kickPose[1],
                                                         0.0)
        player.brain.nav.goTo(repositionAfterWhiff.ballDest,
                              nav.CLOSE_ENOUGH,
                              nav.GRADUAL_SPEED)

    # if it took more than 5 seconds, forget it
    if player.counter > 350:
        return player.goLater('returnUsingLoc')

    kickPose = player.kick.getPosition()
    repositionAfterWhiff.ballDest.relX = (player.brain.ball.rel_x -
                                          kickPose[0])
    repositionAfterWhiff.ballDest.relY = (player.brain.ball.rel_y -
                                          kickPose[1])

    return Transition.getNextState(player, repositionAfterWhiff)
