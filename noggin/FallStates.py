
from man.motion import SweetMoves as SweetMoves
import man.motion as motion

"""
Fall Protection and Recovery States
"""
def fallen(guard):
    if guard.firstFrame():
        x = motion.StiffnessCommand(1.0)
        guard.brain.motion.sendStiffness(x)

    """
    Activates when robot has fallen. Deactivates player
    and puts standup in motion
    """
    # Put player into safe mode
    guard.brain.tracker.stopHeadMoves()
    guard.brain.player.switchTo('fallen')
    return guard.goNow('standup')

def falling(guard):
    """
    Protect the robot when it is falling.
    """
    guard.brain.tracker.stopHeadMoves()
    return guard.goNow('nothing')

def standup(guard):
    """
    Performs the appropriate standup routine
    """
    inertial = guard.brain.sensors.inertial
    # If on back, perform back stand up
    if ( inertial.angleY < -guard.FALLEN_THRESH ):
        return guard.goNow('standFromBack')

    # If on stomach, perform stand up from front
    elif ( inertial.angleY > guard.FALLEN_THRESH ):
        return guard.goNow('standFromFront')
    return guard.stay()

def standFromBack(guard):
    if guard.firstFrame():
        guard.brain.player.executeMove(SweetMoves.STAND_UP_BACK)

    if guard.brain.motion.isBodyActive():
        return guard.stay()
    return guard.goLater('doneStanding')

def standFromFront(guard):
    if guard.firstFrame():
        guard.brain.player.executeMove(SweetMoves.STAND_UP_FRONT)

    if guard.brain.motion.isBodyActive():
        return guard.stay()
    return guard.goLater('doneStanding')

def doneStanding(guard):
    """
    Does clean up after standing up.
    """
    x = motion.StiffnessCommand(.85)
    guard.brain.motion.sendStiffness(x)

    guard.standingUp = False
    guard.brain.player.switchTo(guard.brain.gameController.currentState)
    return guard.goNow('nothing')

def nothing(guard):
    """
    Does nothing
    """
    return guard.stay()


