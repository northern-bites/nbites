import math
import man.noggin.util.MyMath as MyMath
import ChaseBallConstants as constants
import ChaseBallTransitions as transitions

import man.motion.SweetMoves as SweetMoves

def scanFindBall(player):
    '''
    State to move the head to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we spin to keep looking
    '''
    if player.firstFrame() or  \
            player.brain.ball.framesOff > constants.FRAMES_OFF_THRESH:

        player.brain.tracker.switchTo('scanBall')
        player.stopWalking()
    if player.brain.ball.on:
        player.brain.tracker.trackBall()

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
    if player.firstFrame():
        player.setSpeed(constants.X_SPEED_TO_BALL,
                        constants.Y_SPIN_SPEED,
                        constants.SPIN_SPEED)

    if player.brain.ball.on:
        player.brain.tracker.trackBall()

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

    turnRate = MyMath.clip(player.brain.ball.bearing*0.3,
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

    ball = player.brain.ball
    if ball.on:
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


    ball = player.brain.ball
    if ball.on:
        turnRate = MyMath.clip(ball.bearing*0.3,
                               -constants.BALL_SPIN_SPEED,
                               constants.BALL_SPIN_SPEED)

        player.setSpeed(0,constants.Y_SPIN_SPEED,turnRate)

    if transitions.shouldApproachBallClose(player):
        return player.goLater('approachBallClose')
    if transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')

    return player.stay()

def approachBallClose(player):
    if player.firstFrame():
        player.stopWalking()

    if player.brain.ball.on:
        bearing = player.brain.ball.bearing
        ball = player.brain.ball
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

    return player.stay()

def positionForKick(player):
    if player.firstFrame():
        player.stopWalking()

    ball = player.brain.ball
    if ball.on:
        # Put yourself with your left foot
        # in front of the ball
         targetX = math.cos(math.radians(ball.bearing))*ball.dist
         targetY = math.sin(math.radians(ball.bearing))*ball.dist

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
    if ball.on:
        desiredTurn = ball.bearing - \
            (constants.BALL_KICK_BEARING_THRESH_UPPER - \
                 constants.BALL_KICK_BEARING_THRESH_LOWER )
        turnRate = MyMath.clip(desiredTurn*0.4,
                               -constants.BALL_SPIN_SPEED,
                               constants.BALL_SPIN_SPEED)

        player.setSpeed(0,0,turnRate)

    if transitions.shouldKick(player):
        return player.goNow('kickBall')
    if transitions.shouldTurnForKick(player):
        return player.goLater('turnForKick')
    if transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')


    return player.stay()

def kickBall(player):
    if player.firstFrame():
        player.stopWalking()
        return player.stay()
    player.executeMove(SweetMoves.KICK_STRAIGHT)

    return player.goLater('done')

def done(player):
    return player.stay()
