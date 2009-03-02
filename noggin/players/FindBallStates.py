import man.motion as motion
import man.motion.SweetMoves as SweetMoves

def scanFindBall(player):
    if player.firstFrame() or  player.brain.ball.framesOff >2:
        player.printf("First frame or havent seen the ball in abit")
        player.brain.tracker.switchTo('scanBall')

    if player.brain.ball.on:
        player.brain.tracker.trackBall()

    player.printf("ball.frames on" + player.brain.ball.framesOn + " dist:" + player.brain.ball.dist)

    if player.brain.ball.framesOn > 2:
        return player.goNow('rotAlignOnBall')

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.SCAN_BALL):
        return player.goNow('spinFindBall')
    return player.stay()

def spinFindBall(player):
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
    if player.firstFrame():
        player.printf('Cant find ball')
    return player.stay()

def rotAlignOnBall(player):
    turnRate = player.clip(player.brain.ball.bearing*0.5,-25.,25.)
    player.printf( "Ball bearing i "+ str(player.brain.ball.bearing)+
                   " turning at "+str(turnRate))
    player.setSpeed(x=0,y=0,theta=turnRate)
    if player.brain.ball.on and player.brain.ball.bearing < 15 :
        return player.goNow('approachBall')
    return player.stay()

def approachBall(player):
    if player.firstFrame():
        player.setSpeed(0,0,0)
        player.printf("Approaching ball")
    return player.stay()
