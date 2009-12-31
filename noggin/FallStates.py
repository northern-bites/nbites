
from man.motion import SweetMoves as SweetMoves

"""
Fall Protection and Recovery States
"""
def fallen(guard):
    """
    Activates when robot has fallen. Deactivates player
    and puts standup in motion
    """
    if guard.firstFrame():
        guard.brain.roboguardian.enableFallProtection(False)
        guard.brain.tracker.stopHeadMoves()
        guard.brain.motion.resetWalk()
        guard.brain.motion.resetScripted()
        guard.brain.motion.stopHeadMoves()
        guard.brain.player.gainsOn()

    # Put player into safe mode

    guard.brain.player.switchTo('fallen')
    return guard.goLater('standup')

def falling(guard):
    """
    Protect the robot when it is falling.
    """
    guard.brain.tracker.stopHeadMoves()
    return guard.goLater('notFallen')

def standup(guard):
    """
    Performs the appropriate standup routine
    """
    inertial = guard.brain.sensors.inertial
    #guard.printf("standup angleY is "+str(inertial.angleY))

    if guard.firstFrame():
        guard.brain.tracker.stopHeadMoves()
        guard.brain.tracker.setNeutralHead()

    # If on back, perform back stand up
    if ( inertial.angleY < -guard.FALLEN_THRESH ):
        return guard.goLater('standFromBack')

    # If on stomach, perform stand up from front
    elif ( inertial.angleY > guard.FALLEN_THRESH ):
        return guard.goLater('standFromFront')
    return guard.stay()

def standFromBack(guard):
    if guard.firstFrame():
        guard.brain.player.executeMove(SweetMoves.STAND_UP_BACK)
        guard.standupMoveTime = SweetMoves.getMoveTime(SweetMoves.STAND_UP_BACK)

    return guard.goLater('standing')

def standFromFront(guard):
    if guard.firstFrame():
        guard.brain.player.executeMove(SweetMoves.STAND_UP_FRONT)
        guard.standupMoveTime = SweetMoves.getMoveTime(SweetMoves.STAND_UP_FRONT)

    return guard.goLater('standing')

def standing(guard):
    if guard.stateTime <= guard.standupMoveTime:
        return guard.stay()

    return guard.goLater('doneStanding')

def doneStanding(guard):
    """
    Does clean up after standing up.
    """
    if guard.firstFrame():
        guard.brain.player.gainsOn()
        guard.brain.player.stopWalking()

    guard.brain.player.switchTo(guard.brain.gameController.currentState)
    return guard.goLater('notFallen')

def notFallen(guard):
    if guard.firstFrame():
        guard.standingUp = False
        guard.brain.roboguardian.enableFallProtection(True)
    """
    Does nothing
    """
    return guard.stay()


