import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.MotionConstants as MotionConstants

def gameInitial(player):
    return player.goLater('determineDistances')

def nothing(player):
    """
    Do nothing
    """
    return player.stay()

def stopHead(player):
    if player.firstFrame():
        player.brain.motion.stopHeadMoves()
    return player.goLater('nothing')

def determineDistances(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('locPans')
    return player.stay()

def done(player):
    ''' garbage state'''
    return player.stay()
