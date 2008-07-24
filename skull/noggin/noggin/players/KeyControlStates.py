

def nothing(player):
    """
    Do nothing
    """
    player.brain.motion.setDefaultPosition()
    
    
    if player.counter == 0:
        return player.goLater('walk')
    return player.stay()

def walk(player):
    
    lastKey = player.brain.lastCharPressed

    
    if lastKey == 'W':
        player.brain.motion.setMotion(30,0,0)
    elif lastKey == 'A':
        player.brain.motion.setMotion(0,10,0)
    elif lastKey == 'S':
        player.brain.motion.setMotion(-10,0,0)
    elif lastKey == 'D':
        player.brain.motion.setMotion(0,-10,0)
    else:
        player.brain.motion.setMotion(0,0,0)
    
    return player.stay()

def turn(player):
    player.brain.motion.setMotion(0,0,30)
    if player.counter == 250:
        return player.goLater('walk')
    return player.stay()

def webotsTester(player):
    pNum = player.brain.nao.webots.getPlayerNumber()
    pColor = player.brain.nao.webots.getTeamColor()
    (x,y,h) = (200,600,-20)

    player.brain.nao.webots.moveNode(pNum,x,y,h)
    player.printf("Player NUM "+str(pNum))
    player.printf("Player COLOR "+str(pColor))
    

    return player.goLater('done')

def done(player):
    return player.stay()


