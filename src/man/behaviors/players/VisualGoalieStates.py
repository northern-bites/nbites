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

@superState('gameControllerResponder')
def backUpForDangerousBall(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.nav.goTo(RelRobotLocation(-10, 0, 0))

    return Transition.getNextState(player, backUpForDangerousBall)

# clearIt->kickBall->didIKickIt->returnToGoal
@superState('gameControllerResponder')
def clearIt(player):
    if player.firstFrame():
        GoalieStates.watchWithLineChecks.wentToClearIt = True
        GoalieStates.watchWithLineChecks.correctFacing = False
        player.brain.tracker.trackBall()
        clearIt.startingBallDist = player.brain.ball.distance
        clearIt.startingBallX = player.brain.ball.x
        if clearIt.dangerousSide == -1:
            if player.brain.ball.rel_y < 0.0:
                print "I'm kicking right!"
                player.side = RIGHT
                player.kick = kicks.RIGHT_SHORT_STRAIGHT_KICK
            else:
                player.side = LEFT
                player.kick = kicks.LEFT_SHORT_STRAIGHT_KICK
        elif clearIt.dangerousSide == RIGHT:
            print "I'm doing a side kick!"
            player.side = RIGHT
            player.kick = kicks.RIGHT_SIDE_KICK
        else:
            print "I'm doing a left side kick!"
            player.side = LEFT
            player.kick = kicks.LEFT_SIDE_KICK

        kickPose = player.kick.getPosition()
        clearIt.ballDest = RelRobotLocation(player.brain.ball.rel_x -
                                            kickPose[0],
                                            player.brain.ball.rel_y -
                                            kickPose[1],
                                            0.0)

        # reset odometry
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)
        clearIt.odoDelay = True
        clearIt.closeEnuf = False

        print ("Kickpose: ", kickPose[0], kickPose[1])
        print ("Dest: ", clearIt.ballDest.relX, clearIt.ballDest.relY, clearIt.ballDest.relH)
        print ("Ball: ", player.brain.ball.rel_x, player.brain.ball.rel_y)
        return Transition.getNextState(player, clearIt)

    if clearIt.odoDelay:
        clearIt.odoDelay = False
        kickPose = player.kick.getPosition()
        # player.brain.nav.destinationWalkTo(clearIt.ballDest, nav.QUICK_SPEED)
        clearIt.ballDest = RelRobotLocation(player.brain.ball.rel_x -
                                            kickPose[0], 0.0,
                                            # player.brain.ball.rel_y -
                                            # kickPose[1],
                                            0.0)

        player.brain.nav.goTo(clearIt.ballDest,
                              nav.CLOSE_ENOUGH,
                              nav.QUICK_SPEED,
                              adaptive = False)

    kickPose = player.kick.getPosition()

    if (player.brain.ball.rel_x < 30.0) and not clearIt.closeEnuf:
        print "I'm moving my y now"
        print("ball x", player.brain.ball.rel_x)
        clearIt.closeEnuf = True
        clearIt.ballDest.relH = 0.0
        player.brain.nav.goTo(clearIt.ballDest,
                              (3.0, 3.0, 5),
                              nav.MEDIUM_SPEED,
                              adaptive = False)

    elif fabs(player.brain.ball.bearing_deg) > 20:
        clearIt.ballDest.relH = player.brain.ball.bearing_deg

    clearIt.ballDest.relY = player.brain.ball.rel_y - kickPose[1]
    clearIt.ballDest.relX = player.brain.ball.rel_x - kickPose[0]

    # clearIt.ballDest.relH = 0.0
    # player.brain.nav.updateDest(clearIt.ballDest)

    # print("Ball stationary:", player.brain.ball.is_stationary)
    # print("x", player.brain.ball.x, "y:", player.brain.ball.y)

    if fabs(player.brain.ball.mov_vel_x) > 2.0:
        print("I think ball is moving", player.brain.ball.mov_vel_x)
        print("y", player.brain.ball.mov_vel_y)
    if player.brain.ball.mov_vel_x < -4.0:
        print("I think the ball is coming towards me!")
        print("I think ball is moving", player.brain.ball.mov_vel_x)
        print("y", player.brain.ball.mov_vel_y)
    if player.brain.ball.mov_vel_x > 4.0:
        print("I think ball moving away!")
        return player.goLater('returnUsingLoc')

    # or math.fabs(player.brain.ball.x - clearIt.startingBallX) > 15.0:
    #     print("I think the ball has moved away I should stop chasing")

    # print("Ball vel:", player.brain.ball.mov_vel_x)


    return Transition.getNextState(player, clearIt)





@superState('gameControllerResponder')
def didIKickIt(player):
    if player.firstFrame():
        player.brain.nav.stop()
    return Transition.getNextState(player, didIKickIt)

@superState('gameControllerResponder')
def spinToFaceBall(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        # print("ball at ", player.brain.ball.bearing_deg)
        # facingDest = RelRobotLocation(0.0, 0.0, 0.0)

        # facingDest.relH = player.brain.ball.bearing_deg
        # GoalieStates.spinBack.toAngle = player.brain.ball.bearing_deg

        # player.brain.nav.walkTo(facingDest)

    # if player.counter > 250:
    #     return player.goLater('clearIt')

    # return Transition.getNextState(player, spinToFaceBall)

@superState('gameControllerResponder')
def clearBall(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        clearBall.startDist = player.brain.ball.distance

    if player.brain.ball.distance < constants.POSITION_FOR_KICK_DIST:
        # print "Now positioning for kick"
        return player.goNow('positionForGoalieKick')
    elif player.brain.ball.distance < constants.SLOW_DOWN_DIST:
        # print "Slowing down"
        player.brain.nav.chaseBall(nav.MEDIUM_SPEED, fast = True)
    else:
        # print "approaching ball"
        player.brain.nav.chaseBall(nav.FAST_SPEED, fast = True)

    return Transition.getNextState(player, clearBall)

@superState('gameControllerResponder')
def positionForGoalieKick(player):
    if player.firstFrame():
        player.brain.tracker.lookStraightThenTrack()
        if clearIt.ballSide == RIGHT:
            player.kick = kicks.RIGHT_SHORT_STRAIGHT_KICK
        else:
            player.kick = kicks.LEFT_SHORT_STRAIGHT_KICK
        ball = player.brain.ball
        positionForGoalieKick.kickPose = RelRobotLocation(ball.rel_x - player.kick.setupX,
                                    ball.rel_y - player.kick.setupY,
                                    0)
        positionForGoalieKick.speed = nav.CAREFUL_SPEED

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
        return player.goNow('kickBall')

    return Transition.getNextState(player, positionForGoalieKick)

@superState('gameControllerResponder')
def waitToFaceField(player):
    if player.firstFrame():
        player.brain.tracker.lookToAngle(0)

    return Transition.getNextState(player, waitToFaceField)

@superState('gameControllerResponder')
def returnToGoal(player):
    if player.firstFrame():
        player.brain.tracker.trackBall
        if player.lastDiffState == 'didIKickIt' or player.lastDiffState == 'gamePlaying'\
        or player.lastDiffState == 'spinBack':
            correctedDest =(RelRobotLocation(0.0, 0.0, 0.0 ) -
                            returnToGoal.kickPose)
            correctedDest.relH = -returnToGoal.kickPose.relH
            print ("first returnToGoal.kickPose: ", returnToGoal.kickPose.relX, returnToGoal.kickPose.relY, returnToGoal.kickPose.relH)
        else:
            correctedDest = (RelRobotLocation(0.0, 0.0, 0.0) -
                             RelRobotLocation(player.brain.interface.odometry.x,
                                              player.brain.interface.odometry.y,
                                              0.0))

        print ("first correctedDest: ", correctedDest.relX, correctedDest.relY, correctedDest.relH)

        if fabs(correctedDest.relX) < 5:
            correctedDest.relX = 0.0
        if fabs(correctedDest.relY) < 5:
            correctedDest.relY = 0.0
        if fabs(correctedDest.relH) < 5:
            correctedDest.relH = 0.0

        print "I'm returning to goal now!"
        print ("my correctedDest: ", correctedDest.relX, correctedDest.relY, correctedDest.relH)
        print ("My odometry: ", player.brain.interface.odometry.x, player.brain.interface.odometry.y, player.brain.interface.odometry.h )
        correctedDest.relY = 0
        player.brain.nav.walkTo(correctedDest)

    return Transition.getNextState(player, returnToGoal)

# Very hacky, I am on a lot of shoulder medication
# Ideally let a robot find its way back to the goalbox after falling
# or getting lost outside
# Robot spins until it finds a line, then walks towards it
@superState('gameControllerResponder')
def findMyWayBackPtI(player):
    if player.firstFrame():
        player.homeDirections = []
        player.brain.nav.stop()
        findMyWayBackPtI.counter = 0
        player.brain.tracker.lookToAngle(0.0)
    findMyWayBackPtI.counter += 1
    if findMyWayBackPtI.counter > 200:
            return player.goLater('watchWithLineChecks')
    if player.brain.nav.isStopped():
        #TESTING CHANGE
        # if clearIt.ballSide == RIGHT:
        #     player.setWalk(0, 0, -20.0)
        # else:
        player.setWalk(0, 0, 20.0)

    return Transition.getNextState(player, findMyWayBackPtI)

@superState('gameControllerResponder')
def findMyWayBackPtII(player):
    if player.firstFrame():
        findMyWayBackPtII.counter = 0
        player.brain.tracker.lookToAngle(0.0)
        dest = GoalieStates.average(player.homeDirections)
        print "I'm walking back now!"
        player.brain.nav.walkTo(dest)
        player.homeDirections = []
        #Testingchange
        # if clearIt.ballSide == RIGHT:
        #     player.homeDirections += [RelRobotLocation(0.0, 0.0, 175.0)]
        # else:
            # player.homeDirections += [RelRobotLocation(0.0, 0.0, -175.0)]
    findMyWayBackPtII.counter += 1
    # if findMyWayBackPtII.counter > 200:
    #         return player.goLater('watchWithLineChecks')

    return Transition.getNextState(player, findMyWayBackPtII)



@superState('gameControllerResponder')
def repositionAfterWhiff(player):
    if player.firstFrame():
        # reset odometry
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)

        if player.kick in [kicks.RIGHT_SIDE_KICK, kicks.LEFT_SIDE_KICK]:
            pass
        elif player.brain.ball.rel_y < 0.0:
            player.kick = kicks.RIGHT_SHORT_STRAIGHT_KICK
        else:
            player.kick = kicks.LEFT_SHORT_STRAIGHT_KICK

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
