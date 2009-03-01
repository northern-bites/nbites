import man.motion.SweetMoves as SweetMoves

def scanFindBall(player):
    if player.firstFrame() or  player.brain.ball.framesOff >2:
        print "First frame or havent seen the ball in abit"
        player.brain.tracker.switchTo('scanBall')

    if player.brain.ball.on:
        player.brain.tracker.trackBall()

    print "ball.frames on",player.brain.ball.framesOn," dist:",player.brain.ball.dist

    if player.brain.ball.framesOn > 2:
        return player.goNow('rotAlignOnBall')

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.SCAN_BALL):
        return player.goNow('spinFindBall')
    return player.stay()

def spinFindBall(player):
    return player.stay()

def rotAlignOnBall(player):
    print "Ball bearing i " , player.brain.ball.bearing
    return player.stay()
