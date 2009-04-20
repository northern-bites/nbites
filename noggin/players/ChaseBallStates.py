import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import math
from ..util import MyMath as MyMath

SPIN_SPEED = 15
Y_SPEED = 2
X_SPEED = 0
FRAMES_OFF_THRESH = 5
FRAMES_ON_THRESH = 2
BALL_BEARING_THRESH = 15
GIVE_UP_THRESH = 36


def scanFindBall(player):
    '''
    State to move the head to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we spin to keep looking
    '''
    if player.firstFrame() or  player.brain.ball.framesOff > FRAMES_OFF_THRESH:
        player.printf("First frame or havent seen the ball in abit")
        player.brain.tracker.switchTo('scanBall')
        player.stopWalking()
    if player.brain.ball.on:
        player.brain.tracker.trackBall()

    player.printf("ball.frames on" +str(player.brain.ball.framesOn) +
                  " dist:" + str(player.brain.ball.dist))

    if player.brain.ball.framesOn > FRAMES_ON_THRESH:
        return player.goNow('rotAlignOnBall')

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.SCAN_BALL):
        return player.goNow('spinFindBall')
    return player.stay()

def spinFindBall(player):
    '''
    State to spin to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we go to a garbage state
    '''
    if player.firstFrame():
        player.setSpeed(X_SPEED, Y_SPEED, SPIN_SPEED)

    if player.brain.ball.on:
        player.brain.tracker.trackBall()
    if player.brain.ball.framesOn > FRAMES_ON_THRESH:
        return player.goNow('rotAlignOnBall')
    if player.stateTime >= GIVE_UP_THRESH:
        player.goLater('cantFindBall')
    return player.stay()

def cantFindBall(player):
    ''' Garbage state when we can't see the ball'''
    if player.firstFrame():
        player.printf('Cant find ball')
    return player.stay()

def rotAlignOnBall(player):
    '''Rotate to align with the ball. When we get close, we will approach it '''
    turnRate = MyMath.clip(player.brain.ball.bearing*0.5,-10.,10.)
    player.printf( "Ball bearing i "+ str(player.brain.ball.bearing)+
                   " turning at "+str(turnRate))
    player.setSpeed(x=0,y=0,theta=turnRate)
    if player.brain.ball.on and player.brain.ball.bearing < BALL_BEARING_THRESH :
        return player.goNow('approachBall')
    if player.brain.ball.framesOff > FRAMES_OFF_THRESH:
        return player.goNow('scanFindBall')
    return player.stay()

def approachBall(player):
    ''' Once we are alligned with the ball, approach it'''
    if player.firstFrame():
        player.stopWalking()
        player.printf("Approaching ball")
    if player.brain.ball.on:
        bearing = player.brain.ball.bearing
        if bearing < BALL_BEARING_THRESH:
            targetX = math.cos(math.radians(bearing))*player.brain.ball.dist
            targetY = math.sin(math.radians(bearing))*player.brain.ball.dist
            player.printf("bearing is"+str(bearing)+" target X/Y  is "+str(targetX)+","+str(targetY),"blue")
            maxTarget = max(abs(targetX),abs(targetY))


            sX = targetX/maxTarget*4.0
            sY = targetY/maxTarget*1.5
            player.printf("Walk vector is "+str(sX)+","+str(sY))
            player.setSpeed(sX,sY,0)
        else:
            return player.goNow('rotAlignOnBall')
    elif player.brain.ball.framesOff > FRAMES_OFF_THRESH:
        return player.goNow('scanFindBall')
    return player.stay()

def alignOnBallClose(player):
    if player.firstFrame():
        player.stopWalking()
    return player.stay()
