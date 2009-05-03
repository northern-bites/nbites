import man.noggin.util.MyMath as MyMath
import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
import man.motion.SweetMoves as SweetMoves
from math import fabs
import man.motion.StiffnessModes as StiffnessModes

def chase(player):
    if player.brain.ball.on and constants.USE_LOC_CHASE:
        return player.goNow('positionOnBall')
    elif transitions.shouldScanFindBall(player):
        return player.goNow('scanFindBall')
    elif transitions.shouldApproachBall(player):
        return player.goNow('approachBall')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goNow('turnToBallFar')
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
        return player.goLater('turnToBallFar')
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

    if player.brain.nav.isStopped():
        if player.currentGait != constants.FAST_GAIT:
            player.brain.CoA.setRobotGait(player.brain.motion)
            player.currentGait = constants.FAST_GAIT

        player.setSpeed(0, 0, constants.FIND_BALL_SPIN_SPEED)

#     if not player.brain.motion.isHeadActive():
#         player.executeMove(SweetMoves.FIND_BALL_HEADS_LEFT)

    if player.brain.ball.on and constants.USE_LOC_CHASE:
        player.brain.tracker.trackBall()
        return player.goLater('positionOnBall')

    if transitions.shouldTurnToBall_FoundBall(player):
        return player.goLater('turnToBallFar')

    return player.stay()

def turnToBallFar(player):
    ''' Rotate to align with the ball. When we get close, we will approach it '''
    ball = player.brain.ball
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()
        player.currentChaseWalkX = 0
        player.currentChaseWalkY = 0
        player.currentChaseWalkTheta = 0

    turnRate = MyMath.clip(ball.locBearing*constants.BALL_SPIN_GAIN,
                           -constants.BALL_SPIN_SPEED,
                           constants.BALL_SPIN_SPEED)

    if transitions.shouldPositionForKick(player):
        return player.goLater('positionForKick')
    elif transitions.shouldApproachBall(player):
        return player.goLater('approachBall')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')

    elif MyMath.sign(player.currentChaseWalkTheta) != MyMath.sign(turnRate):
        player.currentChaseWalkTheta = turnRate
        player.stopWalking()
    elif ball.on and player.brain.nav.isStopped():
        player.currentChaseWalkTheta = turnRate
        if player.currentGait != constants.NORMAL_GAIT:
            player.brain.CoA.setRobotTurnGait(player.brain.motion)
            player.currentGait = constants.NORMAL_GAIT
        player.setSpeed(x=0,y=0,theta=turnRate)

    return player.stay()

def approachBall(player):
    '''
    Once we are alligned with the ball, approach it
    '''
    if player.firstFrame():
        player.stopWalking()
        player.currentChaseWalkX = 0
        player.currentChaseWalkY = 0
        player.currentChaseWalkTheta = 0

    ball = player.brain.ball
    sX = MyMath.clip(ball.locDist*constants.APPROACH_X_GAIN,
                     constants.MIN_X_SPEED,
                     constants.MAX_X_SPEED)

    if ball.on and player.brain.nav.isStopped():
        if player.currentGait != constants.FAST_GAIT:
            player.brain.CoA.setRobotGait(player.brain.motion)
            player.currentGait = constants.FAST_GAIT
        player.currentChaseWalkX = sX
        player.setSpeed(sX,0,0)

    elif transitions.shouldPositionForKick(player):
        return player.goLater('positionForKick')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goLater('turnToBallFar')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')

    return player.stay()

def positionForKick(player):
    if player.firstFrame():
        player.stopWalking()
        player.currentChaseWalkX = 0
        player.currentChaseWalkY = 0
        player.currentChaseWalkTheta = 0

    ball = player.brain.ball
    targetY = (ball.locRelY - constants.BALL_KICK_LEFT_Y_R )
    sY = MyMath.clip((targetY),
                     constants.MIN_Y_SPEED,
                     constants.MAX_Y_SPEED)

    if transitions.shouldKick(player):
        return player.goLater('waitBeforeKick')
    elif transitions.shouldApproachForKick(player):
        return player.goLater('approachBall')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    elif ball.on and player.brain.nav.isStopped():

        player.currentChaseWalkY = sY
        if player.currentGait != constants.NORMAL_GAIT:
            player.brain.CoA.setRobotTurnGait(player.brain.motion)
            player.currentGait = constants.NORMAL_GAIT
        player.setSpeed(0,sY,0)
    elif (MyMath.sign(player.currentChaseWalkY) != MyMath.sign(sY) or
          fabs(sY - player.currentChaseWalkY) > constants.CHASE_Y_EPSILON):

        player.currentChaseWalkY = sY
        player.stopWalking()

    return player.stay()

def waitBeforeKick(player):
    if player.firstFrame():
        player.stopWalking()
        player.currentChaseWalkX = 0
        player.currentChaseWalkY = 0
        player.currentChaseWalkTheta = 0

    if not player.brain.nav.isStopped():
        return player.stay()
    elif transitions.shouldApproachForKick(player):
        player.brain.tracker.trackBall()
        return player.goLater('approachBall')
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
    Decides if we should kick.
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
        player.executeMove(SweetMoves.KICK_SCAN)
        player.oppGoalLeftPostBearings = []
        player.oppGoalRightPostBearings = []
        player.myGoalLeftPostBearings = []
        player.myGoalRightPostBearings = []

    if (player.stateTime < SweetMoves.getMoveTime(SweetMoves.KICK_SCAN)):
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
            if player.brain.my.h > 0:
                return player.goLater('kickBallRight')
            else:
                return player.goLater('kickBallLeft')
#             # Goal in front
#             if myRightPostBearing > myLeftPostBearing > 0:
#                 # kick right
#                 return player.goLater('kickBallRight')
#             else:
#                 # kick left
#                 return player.goLater('kickBallLeft')
#         elif oppLeftPostBearing is not None and oppRightPostBearing is not None:
#             if oppLeftPostBearing < 0 and oppRightPostBearing > 0:
#                 # kick straight
#                 return player.goLater('kickBallStraight')
#             else:
#                 if fabs(oppLeftPostBearing) > fabs(oppRightPostBearing):
#                     # kick left
#                     return player.goLater('kickBallLeft')
#                 else:
#                     # kick right
#                     return player.goLater('kickBallRight')

        elif myLeftPostBearing is not None:
            if player.brain.my.h > 0:
                return player.goLater('kickBallRight')
            else:
                return player.goLater('kickBallLeft')

#             if myLeftPostBearing > 0:
#                 return player.goLater('kickBallRight')
#             else:
#                 return player.goLater('kickBallLeft')
        elif myRightPostBearing is not None:
            if player.brain.my.h > 0:
                return player.goLater('kickBallRight')
            else:
                return player.goLater('kickBallLeft')

#             if myRightPostBearing > 0:
#                 return player.goLater('kickBallLeft')
#             else:
#                 return player.goLater('kickBallRight')

        # don't do anything
        return player.goLater('ignoreOwnGoal')
    elif player.sawOppGoal:
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
            if player.brain.my.h < -35:
                return player.goLater('kickBallLeft')
            elif player.brain.my.h > 35:
                return player.goLater('kickBallRight')
            else:
                return player.goLater('kickBallStraight')
        # kick straight
        return player.goLater('kickBallStraight')
    else:
        # use localization for kick
        return player.goLater('kickBallStraight')

def kickBallStraight(player):
    if player.firstFrame():
        player.executeStiffness(StiffnessModes.LEFT_FAR_KICK_STIFFNESS)
        player.printf("We should kick straight!", 'cyan')
    if player.counter == 2:
        player.executeMove(SweetMoves.LEFT_FAR_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.LEFT_FAR_KICK):
        # trick the robot into standing up instead of leaning to the side
        player.executeStiffness(StiffnessModes.LOOSE_ARMS_STIFFNESSES)
        player.setSpeed(0,0,0)
        if transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')
        elif transitions.shouldApproachBall(player):
            return player.goLater('approachBall')
        elif transitions.shouldPositionForKick(player):
            return player.goLater('positionForKick')
        elif transitions.shouldTurnToBall_ApproachBall(player):
            return player.goLater('turnToBallFar')

    return player.stay()

def kickBallLeft(player):
    if player.firstFrame():
        player.executeStiffness(StiffnessModes.LEFT_FAR_KICK_STIFFNESS)
        player.printf("We should kick left!", 'cyan')
    if player.counter == 2:
        player.executeMove(SweetMoves.LEFT_FAR_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.LEFT_FAR_KICK):
        # trick the robot into standing up instead of leaning to the side
        player.executeStiffness(StiffnessModes.LOOSE_ARMS_STIFFNESSES)
        player.setSpeed(0,0,0)
        if transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')
        elif transitions.shouldApproachBall(player):
            return player.goLater('approachBall')
        elif transitions.shouldPositionForKick(player):
            return player.goLater('positionForKick')
        elif transitions.shouldTurnToBall_ApproachBall(player):
            return player.goLater('turnToBallFar')

    return player.stay()

def kickBallRight(player):
    if player.firstFrame():
        player.executeStiffness(StiffnessModes.LEFT_FAR_KICK_STIFFNESS)
        player.printf("We should kick right!", 'cyan')
    if player.counter == 2:
        player.executeMove(SweetMoves.LEFT_FAR_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.LEFT_FAR_KICK):
        # trick the robot into standing up instead of leaning to the side
        player.executeStiffness(StiffnessModes.LOOSE_ARMS_STIFFNESSES)
        player.setSpeed(0,0,0)
        if transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')
        elif transitions.shouldApproachBall(player):
            return player.goLater('approachBall')
        elif transitions.shouldPositionForKick(player):
            return player.goLater('positionForKick')
        elif transitions.shouldTurnToBall_ApproachBall(player):
            return player.goLater('turnToBallFar')

    return player.stay()

def done(player):
    return player.stay()

def ignoreOwnGoal(player):
    if transitions.shouldSpinFindBall(player):
        return player.goNow('spinFindBall')

    return player.stay()
