import math
import man.noggin.util.MyMath as MyMath
import ChaseBallConstants as constants
import ChaseBallTransitions as transitions

import man.motion.SweetMoves as SweetMoves

def chase(player):
    if player.brain.ball.on and constants.USE_LOC_CHASE:
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goNow('positionOnBall')
    elif transitions.shouldScanFindBall(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goNow('scanFindBall')
    elif transitions.shouldApproachBall(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goNow('approachBall')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goNow('turnToBallFar')
    elif transitions.shouldSpinFindBall(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goNow('spinFindBall')
    else :
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goNow('scanFindBall')

def scanFindBall(player):
    '''
    State to move the head to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we spin to keep looking
    '''
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    if player.brain.ball.on:
        player.brain.tracker.trackBall()
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goNow('positionOnBall')

    if transitions.shouldTurnToBall_FoundBall(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goNow('turnToBallFar')
    elif transitions.shouldSpinFindBall(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
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
        player.setSpeed(0,
                        0,
                        constants.SPIN_SPEED)
        player.brain.tracker.trackBall()

#     if not player.brain.motion.isHeadActive():
#         player.executeMove(SweetMoves.FIND_BALL_HEADS_LEFT)


    if player.brain.ball.on:
        player.brain.tracker.trackBall()
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goNow('positionOnBall')

    if transitions.shouldTurnToBall_FoundBall(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goNow('turnToBallFar')
#     elif transitions.shouldCantFindBall(player):
#         player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
#                       + ", " + str(player.brain.ball.locDist) + ")")
#         return player.goLater('cantFindBall')

    return player.stay()

def cantFindBall(player):
    ''' Garbage state when we can't see the ball'''
    if player.firstFrame():
        player.printf('Cant find ball')
    return player.stay()

def turnToBallFar(player):
    ''' Rotate to align with the ball. When we get close, we will approach it '''
    ball = player.brain.ball
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    turnRate = MyMath.clip(ball.locBearing*constants.BALL_SPIN_GAIN,
                           -constants.BALL_SPIN_SPEED,
                           constants.BALL_SPIN_SPEED)

#     player.printf("Ball bearing and dist are (" + str(ball.locBearing)
#                   + ", " + str(ball.locDist) + ")")

#     player.printf("Turn rate is " + str(turnRate))


    if transitions.shouldPositionForKick(player):
        return player.goNow('positionForKick')
    elif transitions.shouldApproachBall(player):
        return player.goNow('approachBall')
    elif transitions.shouldScanFindBall(player):
        return player.goNow('scanFindBall')

    elif player.currentSpinDir != MyMath.sign(turnRate):
        player.stopWalking()
        player.currentSpinDir = MyMath.sign(turnRate)
    elif player.stoppedWalk and ball.on and player.brain.nav.isStopped():
        player.currentSpinDir = MyMath.sign(turnRate)
        player.brain.CoA.setRobotTurnGait(player.brain.motion)

        player.setSpeed(x=0,y=0,theta=turnRate)

    return player.stay()

def approachBall(player):
    '''
    Once we are alligned with the ball, approach it
    '''
    if player.firstFrame():
        player.stopWalking()

    ball = player.brain.ball
    if ball.on and player.brain.nav.isStopped():
        player.brain.CoA.setRobotGait(player.brain.motion)
        targetX = math.cos(math.radians(ball.locBearing))*ball.locDist
        targetY = math.sin(math.radians(ball.locBearing))*ball.locDist
        maxTarget = max(abs(targetX),abs(targetY))

#         sX = MyMath.clip((targetX/maxTarget)*constants.APPROACH_X_GAIN,
#                          constants.MIN_X_SPEED,
#                          constants.MAX_X_SPEED)
#         sY = MyMath.clip((targetY/maxTarget)*constants.APPROACH_Y_GAIN,
#                          constants.MIN_Y_SPEED,
#                          constants.MAX_Y_SPEED)
#         sY = 0
        sX = MyMath.clip(ball.locDist*constants.APPROACH_X_GAIN,
            constants.MIN_X_SPEED,
            constants.MAX_X_SPEED)
        player.setSpeed(sX,0,0)

    if transitions.shouldPositionForKick(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goLater('positionForKick')
    if transitions.shouldTurnToBall_ApproachBall(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goLater('turnToBallFar')
    elif transitions.shouldScanFindBall(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goNow('scanFindBall')

    return player.stay()

def positionForKick(player):
    if player.firstFrame():
        player.stopWalking()

    ball = player.brain.ball
    if ball.on and player.brain.nav.isStopped():
        player.brain.CoA.setRobotTurnGait(player.brain.motion)
        # Put yourself with your left foot
        # in front of the ball
        #targetX = (math.cos(math.radians(ball.locBearing))*ball.locDist)*constants.POS_KICK_TARGET_X_GAIN
        targetX = player.brain.ball.locRelX*constants.POS_KICK_TARGET_X_GAIN
#         targetY = (math.sin(math.radians(ball.locBearing))*ball.locDist +
#                    constants.BALL_KICK_LEFT_Y_R )*constants.POS_KICK_TARGET_Y_GAIN
        targetY = (player.brain.ball.locRelY +
                   constants.BALL_KICK_LEFT_Y_R )*constants.POS_KICK_TARGET_Y_GAIN

        maxTarget = max(abs(targetX),abs(targetY))

#         sX = MyMath.clip((targetX/maxTarget)*constants.APPROACH_CLOSE_X_GAIN,
#                          constants.MIN_X_SPEED,
#                          constants.MAX_X_SPEED)
        sX = 0
        sY = MyMath.clip((targetY)*constants.APPROACH_CLOSE_Y_GAIN,
                         constants.MIN_Y_SPEED,
                         constants.MAX_Y_SPEED)
#         if sX > sY:
#             sX = 0
#         else:
#             sY = 0
        player.printf("positionForKick:idealVector is (%g,%g) "%
                      (sX,sY))
        player.setSpeed(sX,sY,0)

    if transitions.shouldKick(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goNow('decideKick')
    if transitions.shouldApproachForKick(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goLater('approachBall')
    if transitions.shouldScanFindBall(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goLater('scanFindBall')

    return player.stay()

def turnForKick(player):
    if player.firstFrame():
        player.stopWalking()

    ball = player.brain.ball
    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    if ball.on and player.brain.nav.isStopped():

        desiredTurn = ball.locBearing*0.8

        turnRate = MyMath.clip(desiredTurn,
                               -constants.BALL_SPIN_SPEED,
                               constants.BALL_SPIN_SPEED)

        player.setSpeed(0,0,turnRate)

    if transitions.shouldKick(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goLater('decideKick')
    if transitions.shouldPositionForKick(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goLater('positionForKick')
    if transitions.shouldScanFindBall(player):
        player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                      + ", " + str(player.brain.ball.locDist) + ")")
        return player.goLater('scanFindBall')


    return player.stay()

def decideKick(player):
    """
    Decides if we should kick.
    """
    if player.firstFrame():
        player.stopWalking()

    if (player.brain.myGoalLeftPost.framesOff > 10 and
        player.brain.myGoalRightPost.framesOff > 10):
        return player.goLater('kickBall')
    return player.goLater('positionForKick')

def kickBall(player):
    print "ball at (relx,rely) = (", player.brain.ball.locRelX, " , ", player.brain.ball.locRelY, ")"
    if player.firstFrame():
        player.stopWalking()
    if player.counter == 2:
        player.executeMove(SweetMoves.NEW_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.KICK_STRAIGHT_LEFT_FAR):
        player.setSpeed(0,0,0)
        if transitions.shouldScanFindBall(player):
            player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                          + ", " + str(player.brain.ball.locDist) + ")")
            return player.goLater('scanFindBall')
        elif transitions.shouldApproachBall(player):
            player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                          + ", " + str(player.brain.ball.locDist) + ")")
            return player.goLater('approachBall')
        elif transitions.shouldPositionForKick(player):
            player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                          + ", " + str(player.brain.ball.locDist) + ")")
            return player.goLater('positionForKick')
#         if transitions.shouldTurnForKick(player):
#             player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
#                           + ", " + str(player.brain.ball.locDist) + ")")
#             return player.goLater('turnForKick')
        elif transitions.shouldTurnToBall_ApproachBall(player):
            player.printf("Ball bearing and dist are (" + str(player.brain.ball.locBearing)
                          + ", " + str(player.brain.ball.locDist) + ")")
            return player.goLater('turnToBallFar')

    return player.stay()

def done(player):
    return player.stay()
