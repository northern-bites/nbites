import man.noggin.util.MyMath as MyMath
import ChaseBallConstants as constants
import ChaseBallTransitions as transitions

import man.motion.SweetMoves as SweetMoves

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
    else :
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
    targetY = (player.brain.ball.locRelY +
               constants.BALL_KICK_LEFT_Y_R )*constants.POS_KICK_TARGET_Y_GAIN
    sY = MyMath.clip((targetY)*constants.APPROACH_CLOSE_Y_GAIN,
                     constants.MIN_Y_SPEED,
                     constants.MAX_Y_SPEED)
    player.printf("My current sY is " + str(player.currentChaseWalkY), 'cyan')
    player.printf("My new sY is " + str(sY), 'cyan')

    if transitions.shouldKick(player):
        return player.goLater('decideKick')
    elif transitions.shouldApproachForKick(player):
        return player.goLater('approachBall')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    elif MyMath.sign(player.currentChaseWalkY) != MyMath.sign(sY):
        player.currentChaseWalkY = sY
        player.stopWalking()
    elif ball.on and player.brain.nav.isStopped():
        player.currentChaseWalkY = sY
        if player.currentGait != constants.NORMAL_GAIT:
            player.brain.CoA.setRobotTurnGait(player.brain.motion)
        player.setSpeed(0,sY,0)

    return player.stay()

def decideKick(player):
    """
    Decides if we should kick.
    """
    if player.firstFrame():
        player.stopWalking()
        player.currentChaseWalkX = 0
        player.currentChaseWalkY = 0
        player.currentChaseWalkTheta = 0

    if (player.brain.myGoalLeftPost.framesOff > 10 and
        player.brain.myGoalRightPost.framesOff > 10):
        return player.goLater('kickBall')
    return player.goLater('positionForKick')

def kickBall(player):
    if player.firstFrame():
        player.stopWalking()
    if player.counter == 2:
        player.executeMove(SweetMoves.NEW_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.NEW_KICK):
        # trick the robot into standing up instead of leaning to the side
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
