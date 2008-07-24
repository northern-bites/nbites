

def nothing(player):
    """
    Do nothing
    """
    if player.firstFrame():
        player.brain.motion.setDefaultPosition()
        

    if player.counter > 10:
        return player.goLater('rotFindBall')

    
    return player.stay()

def printing(player):
    player.printf(str(player.brain.ball.bearing))

    return player.stay()


def panLeft(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('panLeftOnce')

    if player.counter > 30:
        return player.goLater('panRight')

    return player.stay()

def panRight(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('panRightOnce')

    if player.counter > 30:
        return player.goLater('panLeft')

    return player.stay()
    

def locPans(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('locPans')
    return player.stay()


def tracker(player):
    """
    BROKEN:
    if player.firstFrame():
        pass
    if player.brain.ball.on:
        player.brain.tracker.switchTo('tracking',player.brain.ball)

    return player.stay()

    """
    if player.firstFrame():
        player.bearingCounter = 0
    if player.brain.ball.on:
        player.brain.tracker.switchTo('tracking',player.brain.ball)
        
        bearing = player.brain.ball.bearing + \
               player.brain.motion.getHeadYaw()
        
        if abs(bearing) > 10.0:
            player.bearingCounter +=1

        if player.bearingCounter > 5:
            return player.goNow('alignOnBall')

        player.brain.motion.setMotion(0,0,0)
        return player.stay()
    elif player.brain.ball.framesOff >4:
        return player.goNow('findBall')

    return player.stay()
    
def alignOnBall(player):
    if player.brain.ball.on:
        bearing = player.brain.ball.bearing + \
               player.brain.motion.getHeadYaw()

        if abs(bearing) > 10.0:
            player.brain.motion.setMotion(0,0,bearing/2)
        else:
            return player.goLater('tracker')
    elif player.brain.ball.framesOff > 4:
        return player.goNow('findBall')

    return player.stay()

def findBall(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('locPans')#locPans()
    if player.brain.ball.on:
        return player.goNow('tracker')

    if player.counter > 36: #wait 3 secs
        return player.goNow('rotFindBall')
    return player.stay()


def rotFindBall(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('locPans')#locPans()
        player.brain.motion.setMotion(0,0,20)
    if player.brain.ball.on:
        return player.goNow('tracker')
    
    
    
    return player.stay()

def approachBall(player):
    if player.brain.ball.dist > 100:
        player.brain.motion.setMotion(0,0,20)
        return player.stay()
    else:
        player.brain.motion.setMotion(0,0,0)
        return player.goLater('tracker')

def turn(player):
    player.brain.motion.setMotion(0,0,25)
    return player.stay()

def done(player):
    ''' garbage state'''
    return player.stay()
