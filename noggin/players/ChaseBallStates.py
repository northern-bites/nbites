import math
import man.noggin.util.MyMath as MyMath
import ChaseBallConstants as constants
import ChaseBallTransitions as transitions

import man.motion.SweetMoves as SweetMoves

def chase(player):
    if player.brain.ball.on and constants.USE_LOC_CHASE:
        return player.goNow('positionOnBall')
    elif transitions.shouldScanFindBall(player):
        return player.goNow('scanFindBall')
    elif transitions.shouldSpinFindBall(player):
        return player.goNow('spinFindBall')
    elif transitions.shouldApproachBall(player):
        return player.goNow('approachBall')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goNow('turnToBallFar')
    elif transitions.shouldSpinFindBall(player):
        return player.goNow('spinFindBall')
    elif transitions.shouldTurnToBallClose(player):
        return player.goNow('turnToBallClose')
    else :
        return player.goNow('scanFindBall')

def scanFindBall(player):
    '''
    State to move the head to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we spin to keep looking
    '''
    if player.firstFrame():
        player.stopWalking()

    if player.brain.ball.framesOff > constants.FRAMES_OFF_THRESH:
        player.brain.tracker.trackBall()

    if player.brain.ball.on:
        player.brain.tracker.trackBall()
#        return player.goNow('positionOnBall')

    if transitions.shouldTurnToBall_FoundBall(player):
        return player.goNow('turnToBallFar')

    if transitions.shouldSpinFindBall(player):
        return player.goNow('spinFindBall')

    return player.stay()

def spinFindBall(player):
    '''
    State to spin to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we go to a garbage state
    '''
    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    if player.firstFrame() and player.stoppedWalk:
        player.setSpeed(constants.X_SPEED_TO_BALL,
                        constants.Y_SPIN_SPEED,
                        constants.SPIN_SPEED)
        player.brain.tracker.stopHeadMoves()

    if not player.brain.motion.isHeadActive():
        player.executeMove(SweetMoves.FIND_BALL_HEADS_LEFT)


    if player.brain.ball.on:
        player.brain.tracker.trackBall()
#        return player.goNow('positionOnBall')
    
    if transitions.shouldTurnToBall_FoundBall(player):
        return player.goNow('turnToBallFar')
    if transitions.shouldCantFindBall(player):
        return player.goLater('cantFindBall')

    return player.stay()

def cantFindBall(player):
    ''' Garbage state when we can't see the ball'''
    if player.firstFrame():
        player.printf('Cant find ball')
    return player.stay()

def turnToBallFar(player):
    '''Rotate to align with the ball. When we get close, we will approach it '''
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    ball = player.brain.ball
    if player.stoppedWalk and ball.on:
        turnRate = MyMath.clip(ball.bearing*0.3,
                               -constants.BALL_SPIN_SPEED,
                               constants.BALL_SPIN_SPEED)
        player.setSpeed(x=0,y=0,theta=turnRate)

    if transitions.shouldApproachBall(player):
        return player.goLater('approachBall')
    if transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')

    return player.stay()

def approachBall(player):
    ''' Once we are alligned with the ball, approach it'''
    if player.firstFrame():
        player.stopWalking()

    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    ball = player.brain.ball
    if ball.on and player.stoppedWalk:
        targetX = math.cos(math.radians(ball.bearing))*ball.dist
        targetY = math.sin(math.radians(ball.bearing))*ball.dist
        maxTarget = max(abs(targetX),abs(targetY))

        sX = (targetX/maxTarget)*constants.APPROACH_X_GAIN
        sY = (targetY/maxTarget)*constants.APPROACH_Y_GAIN
        player.setSpeed(sX,sY,0)

    if transitions.shouldTurnToBallClose(player):
        return player.goLater('turnToBallClose')
    if transitions.shouldApproachBallClose(player):
        return player.goLater('approachBallClose')
    if transitions.shouldTurnToBall_ApproachBall(player):
        return player.goLater('turnToBallFar')
    if transitions.shouldScanFindBall(player):
        return player.goNow('scanFindBall')

    return player.stay()

def turnToBallClose(player):
    if player.firstFrame():
        player.stopWalking()

    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    ball = player.brain.ball
    if ball.on and player.stoppedWalk:

        turnRate = MyMath.clip(ball.bearing*0.8,
                               -constants.BALL_SPIN_SPEED,
                               constants.BALL_SPIN_SPEED)

        player.setSpeed(0,
                        MyMath.getSign(turnRate)*constants.Y_SPIN_SPEED,
                        turnRate)

    if transitions.shouldPositionForKick(player):
        return player.goLater('positionForKick')
    if transitions.shouldTurnForKick(player):
        return player.goLater('turnForKick')
    if transitions.shouldApproachBallClose(player):
        return player.goLater('approachBallClose')
    if transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')

    return player.stay()

def approachBallClose(player):
    if player.firstFrame():
        player.stopWalking()

    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    ball = player.brain.ball
    if ball.on and player.stoppedWalk:
        bearing = player.brain.ball.bearing

        targetX = math.cos(math.radians(ball.bearing))*ball.dist
        targetY = math.sin(math.radians(ball.bearing))*ball.dist

        maxTarget = max(abs(targetX),abs(targetY))

        sX = (targetX/maxTarget)*constants.APPROACH_CLOSE_X_GAIN
        sY = (targetY/maxTarget)*constants.APPROACH_CLOSE_Y_GAIN

        player.setSpeed(sX,sY,0)

    if transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    if transitions.shouldTurnToBallClose(player):
        return player.goLater('turnToBallClose')
    if transitions.shouldPositionForKick(player):
        return player.goLater('positionForKick')
    if transitions.shouldTurnForKick(player):
        return player.goLater('turnForKick')

    return player.stay()

def positionForKick(player):
    if player.firstFrame():
        player.stopWalking()

    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    ball = player.brain.ball
    if ball.on and player.stoppedWalk:

        # Put yourself with your left foot
        # in front of the ball
         targetX = (math.cos(math.radians(ball.bearing))*ball.dist)*constants.POS_KICK_TARGET_X_GAIN
         targetY = (math.sin(math.radians(ball.bearing))*ball.dist +
                    constants.BALL_KICK_LEFT_Y_R )*constants.POS_KICK_TARGET_Y_GAIN

         maxTarget = max(abs(targetX),abs(targetY))

         sX = (targetX/maxTarget)*constants.APPROACH_CLOSE_X_GAIN
         sY = (targetY/maxTarget)*constants.APPROACH_CLOSE_Y_GAIN
         player.setSpeed(sX,sY,0)

    if transitions.shouldKick(player):
        return player.goNow('kickBall')
    if transitions.shouldTurnForKick(player):
        return player.goLater('turnForKick')
    if transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')

    return player.stay()

def turnForKick(player):
    if player.firstFrame():
        player.stopWalking()

    ball = player.brain.ball
    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    if ball.on and player.stoppedWalk:

        desiredTurn = ball.bearing*0.8

        turnRate = MyMath.clip(desiredTurn,
                               -constants.BALL_SPIN_SPEED,
                               constants.BALL_SPIN_SPEED)

        player.setSpeed(0,0,turnRate)

    if transitions.shouldKick(player):
        return player.goLater('kickBall')
    if transitions.shouldPositionForKick(player):
        return player.goLater('positionForKick')
    if transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')


    return player.stay()

def kickBall(player):
    print "ball at (relx,rely) = (", player.brain.ball.locRelX, " , ", player.brain.ball.locRelY, ")"
    if player.firstFrame():
        player.stopWalking()
    if player.counter == 2:
        player.executeMove(SweetMoves.KICK_STRAIGHT_LEFT_FAR)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.KICK_STRAIGHT_LEFT_FAR):
        if transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')
        if transitions.shouldApproachBall(player):
            return player.goLater('approachBall')
        if transitions.shouldTurnToBallClose(player):
            return player.goLater('turnToBallClose')
        if transitions.shouldPositionForKick(player):
            return player.goLater('positionForKick')
        if transitions.shouldTurnForKick(player):
            return player.goLater('turnForKick')
        if transitions.shouldTurnToBall_ApproachBall(player):
            return player.goLater('turnToBallFar')

    return player.stay()

def done(player):
    return player.stay()
