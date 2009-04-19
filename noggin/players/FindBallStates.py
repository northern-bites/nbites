import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import math
from ..util import MyMath as MyMath

def scanFindBall(player):
    '''
    State to move the head to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we spin to keep looking
    '''
    if player.firstFrame() or  player.brain.ball.framesOff >2:
        player.printf("First frame or havent seen the ball in abit")
        player.brain.tracker.switchTo('scanBall')
        player.setSpeed(0,0,0)
    if player.brain.ball.on:
        player.brain.tracker.trackBall()

    player.printf("ball.frames on" +str(player.brain.ball.framesOn) +
                  " dist:" + str(player.brain.ball.dist))

    if player.brain.ball.framesOn > 2:
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
        player.setSpeed(0,0,10)

    if player.brain.ball.on:
        player.brain.tracker.trackBall()
    if player.brain.ball.framesOn > 2:
        return player.goNow('rotAlignOnBall')
    if player.stateTime >= 36:
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
    if player.brain.ball.on and player.brain.ball.bearing < 15 :
        return player.goNow('approachBall')
    if player.brain.ball.framesOff > 5:
        return player.goNow('scanFindBall')
    return player.stay()

def approachBall(player):
    ''' Once we are alligned with the ball, approach it'''
    if player.firstFrame():
        player.setSpeed(0,0,0)
        player.printf("Approaching ball")
    if player.brain.ball.on:
        bearing = player.brain.ball.bearing
        if bearing < 20:
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
    elif player.brain.ball.framesOff >2:
        return player.goNow('scanFindBall')
    return player.stay()
