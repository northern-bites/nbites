import man.noggin.util.MyMath as MyMath
import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
from math import fabs
import man.motion.StiffnessModes as StiffnessModes

def chase(player):
    """
    Method to determine which chase state should be used.
    We dump the robot into this state when we our switching from something else.
    """
    if player.brain.ball.on and constants.USE_LOC_CHASE:
        return player.goNow('positionOnBall')
    elif transitions.shouldScanFindBall(player):
        return player.goNow('scanFindBall')
    elif transitions.shouldApproachBall(player):
        return player.goNow('approachBall')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goNow('turnToBall')
    elif transitions.shouldSpinFindBall(player):
        return player.goNow('spinFindBall')
    else:
        return player.goNow('scanFindBall')

def scanFindBall(player):
    '''
    State to move the head to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we spin to keep looking
    '''
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    if player.brain.ball.on and constants.USE_LOC_CHASE:
        player.brain.tracker.trackBall()
        return player.goLater('positionOnBall')
    elif transitions.shouldTurnToBall_FoundBall(player):
        return player.goLater('turnToBall')
    elif transitions.shouldSpinFindBall(player):
        return player.goLater('spinFindBall')

    return player.stay()

def spinFindBall(player):
    '''
    State to spin to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we go to a garbage state
    '''
    if player.firstFrame():
        player.brain.tracker.trackBall()
        # Stop walking if we need to switch gaits
        if player.currentGait != constants.FAST_GAIT:
            player.stopWalking()
            player.stoppedWalk = False
        else:
            player.stoppedWalk = True

    # Determine if we have stopped walking
    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    # Walk if we have stopped walking
    if player.stoppedWalk:
        # Switch to the fast gait for better spinning
        if player.currentGait != constants.FAST_GAIT:
            player.brain.CoA.setRobotGait(player.brain.motion)
            player.currentGait = constants.FAST_GAIT

        player.setSpeed(0, 0, constants.FIND_BALL_SPIN_SPEED)

    # Determine if we should leave this state
    if player.brain.ball.on and constants.USE_LOC_CHASE:
        player.brain.tracker.trackBall()
        return player.goLater('positionOnBall')
    elif transitions.shouldTurnToBall_FoundBall(player):
        return player.goLater('turnToBall')

    return player.stay()

def turnToBall(player):
    """
    Rotate to align with the ball. When we get close, we will approach it
    """
    ball = player.brain.ball

    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.currentChaseWalkX = 0
        player.currentChaseWalkY = 0
        player.currentChaseWalkTheta = 0
        # Stop walking if we need to switch gaits
        if player.currentGait != constants.FAST_GAIT:
            player.stopWalking()
            player.stoppedWalk = False
        else:
            player.stoppedWalk = True

    # Determine the speed to turn to the ball
    turnRate = MyMath.clip(ball.bearing*constants.BALL_SPIN_GAIN,
                           -constants.BALL_SPIN_SPEED,
                           constants.BALL_SPIN_SPEED)

    # Avoid spinning so slowly that we step in place
    if fabs(turnRate) < constants.MIN_BALL_SPIN_SPEED:
        turnRate = MyMath.sign(turnRate)*constants.MIN_BALL_SPIN_SPEED

    # Determine that we have stopped walking
    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    if transitions.shouldPositionForKick(player):
        return player.goLater('positionForKick')
    elif transitions.shouldApproachBall(player):
        return player.goLater('approachBall')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    elif ball.on and player.stoppedWalk:
        # Switch gaits if necessary
        if player.currentGait != constants.FAST_GAIT:
            player.brain.CoA.setRobotGait(player.brain.motion)
            player.currentGait = constants.FAST_GAIT

        player.currentChaseWalkTheta = turnRate
        player.setSpeed(x=0,y=0,theta=turnRate)

    return player.stay()

def approachBall(player):
    '''
    Once we are alligned with the ball, approach it
    '''
    if player.firstFrame():
        player.currentChaseWalkX = 0
        player.currentChaseWalkY = 0
        player.currentChaseWalkTheta = 0
        if player.currentGait != constants.FAST_GAIT:
            player.stopWalking()
            player.stoppedWalk = False
        else:
            player.stoppedWalk = True

    # Determine if we have stopped walking
    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    # Switch to other states if we should
    if transitions.shouldPositionForKick(player):
        return player.goLater('positionForKick')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goLater('turnToBall')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    elif transitions.shouldAvoidObstacle(player):
        return player.goLater('avoidObstacle')

    # Determine our speed for approaching the ball
    ball = player.brain.ball
    sX = MyMath.clip(ball.dist*constants.APPROACH_X_GAIN,
                     constants.MIN_X_SPEED,
                     constants.MAX_X_SPEED)

    # Determine the speed to turn to the ball
    sTheta = MyMath.clip(ball.bearing*constants.APPROACH_SPIN_GAIN,
                         -constants.APPROACH_SPIN_SPEED,
                         constants.APPROACH_SPIN_SPEED)
    # Avoid spinning so slowly that we step in place
    if fabs(sTheta) < constants.MIN_APPROACH_SPIN_SPEED:
        sTheta = 0.0

    # Set our walk towards the ball
    if ball.on and player.stoppedWalk:
        if player.currentGait != constants.FAST_GAIT:
            player.brain.CoA.setRobotGait(player.brain.motion)
            player.currentGait = constants.FAST_GAIT
        player.currentChaseWalkX = sX
        player.currentChaseWalkTheta = sTheta
        player.setSpeed(sX,0,sTheta)

    return player.stay()

def positionForKick(player):
    """
    State to align on the ball once we are near it
    Currently aligns the ball on the left foot
    """
    ball = player.brain.ball

    if player.firstFrame():
        player.currentChaseWalkX = 0
        player.currentChaseWalkY = 0
        player.currentChaseWalkTheta = 0

        # Stop if we need to switch gaits
        if player.currentGait != constants.NORMAL_GAIT:
            player.stoppedWalk = False
            player.stopWalking()
        else:
            player.stoppedWalk = True

    # Leave this state if necessary
    if transitions.shouldKick(player):
        return player.goLater('waitBeforeKick')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goLater('turnToBall')


    # Determine approach speed
    targetY = (ball.locRelY -
               (constants.BALL_KICK_LEFT_Y_L + constants.BALL_KICK_LEFT_Y_R) / 2.0 )
    sY = MyMath.clip(targetY,
                     constants.MIN_Y_SPEED,
                     constants.MAX_Y_SPEED)
    if fabs(sY) < constants.MIN_Y_MAGNITUDE:
        sY = 0.0

    if transitions.shouldApproachForKick(player):
        targetX = (ball.locRelX -
                   (constants.BALL_KICK_LEFT_X_CLOSE +
                    constants.BALL_KICK_LEFT_X_FAR) / 2.0)
        sX = MyMath.clip(ball.locRelX,
                         constants.MIN_X_SPEED,
                         constants.MAX_X_SPEED)
    else:
        sX = 0.0

#     player.printf("\tPosition for kick target_y is " +
#                   str(targetY), "cyan")
#     player.printf("\tPosition for kick sY is " + str(sY), "cyan")
#     player.printf("\tPosition for kick sX is " + str(sX), "cyan")
#     player.printf("\tBall dist is: " + str(ball.dist), "cyan")
#     player.printf("\tBall current y is: " +
#                   str(ball.locRelY) + " want between " +
#                   str(constants.BALL_KICK_LEFT_Y_L) +
#                   " and " +
#                   str(constants.BALL_KICK_LEFT_Y_R), "cyan")
#     player.printf("\tBall current x is: " +
#                   str(ball.locRelX) + " want between " +
#                   str(constants.BALL_KICK_LEFT_X_CLOSE) +
#                   " and " + str(constants.BALL_KICK_LEFT_X_FAR), "cyan")

    # Determine if we have stopped
    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    # Walk if we have stopped or have the correct gait already
    if ball.on and player.stoppedWalk:
        # Set the correct gait, to make us walk better
        if player.currentGait != constants.NORMAL_GAIT:
            player.brain.CoA.setRobotTurnGait(player.brain.motion)
            player.currentGait = constants.NORMAL_GAIT
        player.currentChaseWalkY = sY
        player.currentChaseWalkX = sX
        player.setSpeed(0,sY,sX)

    return player.stay()

def waitBeforeKick(player):
    """
    Stop before we kick to make sure we want to kick
    """
    if player.firstFrame():
        player.stopWalking()
        player.currentChaseWalkX = 0
        player.currentChaseWalkY = 0
        player.currentChaseWalkTheta = 0

    if not player.brain.nav.isStopped():
        return player.stay()
#     elif transitions.shouldApproachForKick(player):
#         player.brain.tracker.trackBall()
#         return player.goLater('approachBall')
    elif transitions.shouldScanFindBall(player):
        player.brain.tracker.trackBall()
        return player.goLater('scanFindBall')
#     elif transitions.shouldPositionForKick(player):
#         player.brain.tracker.trackBall()
#         return player.goLater('positionForKick')
    else:
        return player.goLater('decideKick')

def decideKick(player):
    """
    Decides which kick to use
    """
    if player.firstFrame():
        player.stopWalking()
        player.currentChaseWalkX = 0
        player.currentChaseWalkY = 0
        player.currentChaseWalkTheta = 0
        player.sawOwnGoal = False
        player.sawOppGoal = False
        player.brain.tracker.switchTo('stopped')
        player.brain.motion.stopHeadMoves()
        player.executeMove(HeadMoves.KICK_SCAN)
        player.oppGoalLeftPostBearings = []
        player.oppGoalRightPostBearings = []
        player.myGoalLeftPostBearings = []
        player.myGoalRightPostBearings = []

    if (player.stateTime < SweetMoves.getMoveTime(HeadMoves.KICK_SCAN)):
        if player.brain.myGoalLeftPost.on:
            player.sawOwnGoal = True
            player.myGoalLeftPostBearings.append(player.brain.myGoalLeftPost.bearing)
        if player.brain.myGoalRightPost.on:
            player.sawOwnGoal = True
            player.myGoalRightPostBearings.append(player.brain.myGoalRightPost.bearing)
        if player.brain.oppGoalLeftPost.on:
            player.sawOppGoal = True
            player.oppGoalLeftPostBearings.append(player.brain.oppGoalRightPost.bearing)
        if player.brain.oppGoalRightPost.on:
            player.sawOppGoal = True
            player.oppGoalRightPostBearings.append(player.brain.oppGoalRightPost.bearing)
        return player.stay()
    elif player.sawOwnGoal:
        player.brain.tracker.trackBall()
        if constants.SUPER_SAFE_KICKS:
            return player.goLater('ignoreOwnGoal')
        myLeftPostBearing = None
        myRightPostBearing = None
        oppLeftPostBearing = None
        oppRightPostBearing = None
        if len(player.myGoalLeftPostBearings) > 0:
            myLeftPostBearing = (sum(player.myGoalLeftPostBearings) /
                                 len(player.myGoalLeftPostBearings))
        if len(player.myGoalRightPostBearings) > 0:
            myRightPostBearing = (sum(player.myGoalRightPostBearings) /
                                  len(player.myGoalRightPostBearings))
        if len(player.oppGoalLeftPostBearings) > 0:
            oppLeftPostBearing = (sum(player.oppGoalLeftPostBearings) /
                                 len(player.oppGoalLeftPostBearings))
        if len(player.oppGoalRightPostBearings) > 0:
            oppRightPostBearing = (sum(player.oppGoalRightPostBearings) /
                                   len(player.oppGoalRightPostBearings))

        if myLeftPostBearing is not None:
            player.printf("My left post bearing is: " + str(myLeftPostBearing),'cyan')
        if myRightPostBearing is not None:
            player.printf("My right post bearing is: " + str(myRightPostBearing),'cyan')
        if oppLeftPostBearing is not None:
            player.printf("Opp left post bearing is: " + str(oppLeftPostBearing),'cyan')
        if oppRightPostBearing is not None:
            player.printf("Opp right post bearing is: " + str(oppRightPostBearing),'cyan')

        # We see both posts
        if myLeftPostBearing is not None and myRightPostBearing is not None:
#             if player.brain.my.h > 0:
#                 return player.goLater('kickBallRight')
#             else:
#                 return player.goLater('kickBallLeft')
            # Goal in front
            if myRightPostBearing > myLeftPostBearing > 0:
                # kick right
                return player.goLater('kickBallRight')
            else:
                # kick left
                return player.goLater('kickBallLeft')
        elif oppLeftPostBearing is not None and oppRightPostBearing is not None:
            if oppLeftPostBearing < 0 and oppRightPostBearing > 0:
                # kick straight
                return player.goLater('kickBallStraight')
            else:
                if fabs(oppLeftPostBearing) > fabs(oppRightPostBearing):
                    # kick left
                    return player.goLater('kickBallLeft')
                else:
                    # kick right
                    return player.goLater('kickBallRight')

        elif myLeftPostBearing is not None:
#             if player.brain.my.h > 0:
#                 return player.goLater('kickBallRight')
#             else:
#                 return player.goLater('kickBallLeft')

            if myLeftPostBearing > 0:
                return player.goLater('kickBallRight')
            else:
                return player.goLater('kickBallLeft')
        elif myRightPostBearing is not None:
            if player.brain.my.h > 0:
                return player.goLater('kickBallRight')
            else:
                return player.goLater('kickBallLeft')

            if myRightPostBearing > 0:
                return player.goLater('kickBallLeft')
            else:
                return player.goLater('kickBallRight')

        # don't do anything
        return player.goLater('ignoreOwnGoal')
    elif player.sawOppGoal:
        return player.goLater('kickBallStraight')
        myLeftPostBearing = None
        myRightPostBearing = None
        oppLeftPostBearing = None
        oppRightPostBearing = None
        player.brain.tracker.trackBall()
        if len(player.myGoalLeftPostBearings) > 0:
            myLeftPostBearing = (sum(player.myGoalLeftPostBearings) /
                                 len(player.myGoalLeftPostBearings))
        if len(player.myGoalRightPostBearings) > 0:
            myRightPostBearing = (sum(player.myGoalRightPostBearings) /
                                  len(player.myGoalRightPostBearings))
        if len(player.oppGoalLeftPostBearings) > 0:
            oppLeftPostBearing = (sum(player.oppGoalLeftPostBearings) /
                                 len(player.oppGoalLeftPostBearings))
        if len(player.oppGoalRightPostBearings) > 0:
            oppRightPostBearing = (sum(player.oppGoalRightPostBearings) /
                                   len(player.oppGoalRightPostBearings))
        if myLeftPostBearing is not None:
            player.printf("My left post bearing is: " + str(myLeftPostBearing),'cyan')
        if myRightPostBearing is not None:
            player.printf("My right post bearing is: " + str(myRightPostBearing),'cyan')
        if oppLeftPostBearing is not None:
            player.printf("Opp left post bearing is: " + str(oppLeftPostBearing),'cyan')
        if oppRightPostBearing is not None:
            player.printf("Opp right post bearing is: " + str(oppRightPostBearing),'cyan')

        if oppLeftPostBearing is not None and oppRightPostBearing is not None:
            if oppLeftPostBearing < 0 and oppRightPostBearing > 0:
                # kick straight
                return player.goLater('kickBallStraight')
            else:
                if player.brain.my.h > 0:
                    return player.goLater('kickBallRight')
                else:
                    return player.goLater('kickBallLeft')
        else:
            if player.brain.my.h < -65:
                return player.goLater('kickBallLeft')
            elif player.brain.my.h > 65:
                return player.goLater('kickBallRight')
            else:
                return player.goLater('kickBallStraight')
        # kick straight
        return player.goLater('kickBallStraight')
    else:
        # use localization for kick
        return player.goLater('kickBallStraight')

def kickBallStraight(player):
    """
    Kick the ball forward.  Currently uses the left foot
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.executeStiffness(StiffnessModes.LEFT_FAR_KICK_STIFFNESS)
        player.printf("We should kick straight!", 'cyan')
    if player.counter == 2:
        player.executeMove(SweetMoves.LEFT_FAR_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.LEFT_FAR_KICK):
        # trick the robot into standing up instead of leaning to the side
        player.executeStiffness(StiffnessModes.LOOSE_ARMS_STIFFNESSES)
        player.walkPose()

        if transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')
        elif transitions.shouldApproachBall(player):
            return player.goLater('approachBall')
        elif transitions.shouldPositionForKick(player):
            return player.goLater('positionForKick')
        elif transitions.shouldTurnToBall_ApproachBall(player):
            return player.goLater('turnToBall')

    return player.stay()

def kickBallLeft(player):
    """
    Strafe in order to line up to kick the ball to the right
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.setSpeed(0,1.5,0)
    elif player.counter == 10:
        player.stopWalking()
    elif player.brain.nav.isStopped():
        return player.goLater('kickBallLeftExecute')
    return player.stay()

def kickBallRight(player):
    """
    Kick the ball to the right, using the left foot
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.executeStiffness(StiffnessModes.LEFT_SIDE_KICK_STIFFNESSES)
        player.printf("We should kick right!", 'cyan')
    if player.counter == 2:
        # Left side kick, means the sideways kick with the left foot
        # Kicks the ball to the right
        player.executeMove(SweetMoves.LEFT_SIDE_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.LEFT_SIDE_KICK):
        # trick the robot into standing up instead of leaning to the side
        player.executeStiffness(StiffnessModes.LOOSE_ARMS_STIFFNESSES)
        player.walkPose()
        if transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')
        elif transitions.shouldApproachBall(player):
            return player.goLater('approachBall')
        elif transitions.shouldPositionForKick(player):
            return player.goLater('positionForKick')
        elif transitions.shouldTurnToBall_ApproachBall(player):
            return player.goLater('turnToBall')

    return player.stay()

def kickBallLeftExecute(player):
    """
    Kicks the ball to the left, using the right foot
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.executeStiffness(StiffnessModes.RIGHT_SIDE_KICK_STIFFNESSES)
        player.printf("We should kick left!", 'cyan')
    if player.counter == 2:
        # Right side kick, means the sideways kick with the right foot
        # Kicks the ball to the left
        player.executeMove(SweetMoves.RIGHT_SIDE_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.RIGHT_SIDE_KICK):
        # trick the robot into standing up instead of leaning to the side
        player.executeStiffness(StiffnessModes.LOOSE_ARMS_STIFFNESSES)
        player.walkPose()
        if transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')
        elif transitions.shouldApproachBall(player):
            return player.goLater('approachBall')
        elif transitions.shouldPositionForKick(player):
            return player.goLater('positionForKick')
        elif transitions.shouldTurnToBall_ApproachBall(player):
            return player.goLater('turnToBall')

    return player.stay()

def ignoreOwnGoal(player):
    """
    Method to intelligently ignore kicking into our own goal
    """
    if transitions.shouldSpinFindBall(player):
        return player.goNow('spinFindBall')

    return player.stay()

def avoidObstacle(player):
    """
    If we detect something in front of us, dodge it
    """
    if player.firstFrame():
        player.stopWalking()

    player.printf(player.brain.sonar)

    if not transitions.shouldAvoidObstacle(player):
        if player.brain.ball.on:
            return player.goLater("chase")
        else:
            return player.goLater("scanFindBall")
    return player.stay()
