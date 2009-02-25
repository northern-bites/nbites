import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.MotionConstants as MotionConstants

def gameInitial(player):
    return player.goLater('trackBall')

def nothing(player):
    """
    Do nothing
    """

#    if player.counter > 10:
#        return player.goLater('test')

    
    return player.stay()

def test(player):
    if player.firstFrame():
        print "Would be sending a head command"
        hc = motion.HeadJointCommand(5.0,(0.,0.),1)
        player.brain.motion.enqueue(hc)
    if player.counter == 2:
        return player.goLater("stopHead")
    return player.stay()

def stopHead(player):
    if player.firstFrame():
        player.brain.motion.stopHeadMoves()

    return player.goLater('nothing')

def trackBall(player):
    if player.brain.ball.on:
        player.brain.tracker.trackBall()
    else:
        player.brain.tracker.switchTo('scan')
    return player.stay()

def printing(player):
    TO_DEG = 180./math.pi
    #player.printf("bearing:   "+str(player.brain.ball.bearing*TO_DEG))
    #player.printf("elevation: "+str(player.brain.ball.elevation*TO_DEG))
    player.printf("angleX:   "+str(player.brain.ball.angleX))
    player.printf("angleY: "+str(player.brain.ball.angleY))

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
