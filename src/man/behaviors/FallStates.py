import SweetMoves as SweetMoves

"""
Fall Protection and Recovery States
Also detects if the robot has been picked up & stops walk engine
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
        guard.brain.tracker.setNeutralHead()

    if not guard.executeStandup:
        guard.printf("FallController: not executing a standup routine")
        guard.standupMoveTime = 150 # keep stiffness off for 5s
        return guard.goLater('standing')

    # If on back
    if ( inertial.angleY < 0 ):
        return guard.goLater('standFromBack')

    # If on stomach
    return guard.goLater('standFromFront')

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
        guard.brain.player.stand()

    # Swtich player out of safe mode. Put in the Game Controller
    # state so that if game conditions have changed, it doens't do
    # what it was doing.
    guard.brain.player.switchTo(guard.brain.gameController.currentState)
    return guard.goLater('notFallen')

def feetOffGround(guard):
    """
    Shuts off walk engine while the robot is off the ground
    """
    if guard.firstFrame():
        guard.brain.tracker.stopHeadMoves()
        guard.brain.motion.resetWalk()
        guard.brain.motion.resetScripted()
        guard.brain.motion.stopHeadMoves()
        guard.brain.player.stopWalking()
        guard.brain.nav.stop()

    if (guard.brain.roboguardian.isFeetOnGround()):
        guard.brain.player.switchTo(guard.brain.gameController.currentState)
        return guard.goNow('notFallen')

    return guard.stay()

def notFallen(guard):
    """
    Does nothing, also the target state for feet on ground
    """
    if guard.firstFrame():
        guard.standingUp = False
        guard.brain.roboguardian.enableFallProtection(True)

    return guard.stay()

def off(guard):
    if guard.firstFrame():
        guard.brain.roboguardian.enableFallProtection(False)

    return guard.stay()

def on(guard):
    if guard.firstFrame():
        guard.standingUp = False
        guard.brain.roboguardian.enableFallProtection(True)

    return guard.stay()
