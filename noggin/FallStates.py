
from man.motion import SweetMoves as SweetMoves
import man.motion as motion

"""
Fall Protection and Recovery States
"""
def fallen(guard):
    if guard.firstFrame():
        guard.brain.roboguardian.enableFallProtection(False)
        guard.brain.tracker.stopHeadMoves()
        guard.brain.motion.resetWalk()
        x = motion.StiffnessCommand(1.0)
        guard.brain.motion.sendStiffness(x)

    """
    Activates when robot has fallen. Deactivates player
    and puts standup in motion
    """
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
        headscmd = motion.HeadJointCommand(.5,(0.,0.),1)
        guard.brain.motion.enqueue(headscmd)

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

    return guard.goLater('standing')

def standFromFront(guard):
    if guard.firstFrame():
        guard.brain.player.executeMove(SweetMoves.STAND_UP_FRONT)

    return guard.goLater('standing')

def standing(guard):

    if guard.brain.motion.isBodyActive():
        return guard.stay()
    else :
        guard.doneStandingCount += 1

    if guard.doneStandingCount > guard.DONE_STANDING_THRESH:
        guard.doneStandingCount
        return guard.goLater('doneStanding')
    return guard.stay()

def doneStanding(guard):
    """
    Does clean up after standing up.
    """
    if guard.firstFrame():
        x = motion.StiffnessCommand(.85)
        guard.brain.motion.sendStiffness(x)

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


