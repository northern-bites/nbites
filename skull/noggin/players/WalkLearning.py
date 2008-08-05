
RESET_POS = (130,300,0) #x,y,h
STOP_DIST = 100.
ADJUST_ANGLE = 5.0

def nothing(player):
    """
    Do nothing
    """
    player.brain.motion.setDefaultPosition()
    #player.brain.motion.setMotion(30,0,0)
    
    if player.counter == 12:
        return player.goLater('reset')
    return player.stay()

def walk(player):
    
    if player.firstFrame():
        player.brain.motion.setMotion(30,0,0)
    
    if player.object.on:
        player.brain.tracker.switchTo('tracking',player.object)
    
    if player.object.dist <= STOP_DIST:
        #stop walking
        player.brain.motion.setMotion(0,0,0)
        return player.goLater('done')

        
    return player.stay()


def reset(player):    
    (x,y,h) = RESET_POS
    pNum = player.brain.nao.webots.getPlayerNumber()

    player.brain.nao.webots.moveNode(pNum,x,y,h)
    player.brain.nao.webots.moveNode(8,20,20,0)
    
    player.printf("Resting position to (%g,%g,%g)" % RESET_POS)

    return player.goLater('idle')

def idle(player):
    """
    State to wait after reset before walking.
    """
    if player.counter == 4:
        return player.goLater('walk')
    return player.stay()

def done(player):
    """
    Blackhole state.
    """
    return player.stay()


