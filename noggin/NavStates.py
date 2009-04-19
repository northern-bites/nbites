''' States for finding our way on the field '''

from .util import MyMath

GOTO_FORWARD_SPEED = 4
WAIT_BETWEEN_MOVES = 10
GOTO_SPIN_SPEED = 15
GOTO_SPIN_STRAFE = 2

# States for the standard spin - walk - spin go to
def spinToWalkHeading(nav):
    """
    Spin to the heading needed to walk to a specific point
    """
    if nav.firstFrame():
        nav.setSpeed(0,0,0)

    targetH = MyMath.getTargetHeading(nav.brain.my, nav.destX, nav.destY)

    if nav.counter > WAIT_BETWEEN_MOVES:
        spinDir = MyMath.getSpinDir(targetH)
        nav.setSpeed(0, spinDir * GOTO_SPIN_STRAFE, spinDir * GOTO_SPIN_SPEED)

    if nav.atHeading(targetH):
        return nav.goLater('walkToPoint')

    return nav.stay()

def walkToPoint(nav):
    """
    State to walk forward until localization thinks we are close to the point
    Stops if we get there
    If we no longer are heading towards it change to the spin state
    """
    if nav.firstFrame():
        nav.setSpeed(0,0,0)
    if nav.counter > WAIT_BETWEEN_MOVES:
        nav.setSpeed(GOTO_FORWARD_SPEED, 0, 0)

    if nav.atDestination():
        if nav.destH is None:
            return nav.goLater('stop')
        else:
            return nav.goLater('spinToFinalHeading')

    targetH = MyMath.getTargetHeading(nav.brain.my, nav.destX, nav.destY)

    if not nav.atHeading(targetH):
        return nav.goLater('spinToWalkHeading')

    return nav.stay()

def spinToFinalHeading(nav):
    """
    Spins until we are facing the final desired heading
    Stops when at heading
    """
    if nav.firstFrame():
        nav.setSpeed(0,0,0)

    if nav.counter > WAIT_BETWEEN_MOVES:
        nav.setSpeed(0, GOTO_SPIN_STRAFE, GOTO_SPIN_SPEED)

    if nav.atHeading():
        nav.goLater('stop')
    return nav.stay()

# State to be used with standard setSpeed movement
def walking(nav):
    """
    State to be used when setSpeed is called
    """
    if nav.firstFrame():
        nav.printf("New walk is (" + str(nav.walkX) + ", " +
                   str(nav.walkY) + ", " + str(nav.walkTheta) + ")")
        nav.setSpeed(nav.walkX, nav.walkY, nav.walkTheta)

    return nav.stay()

### Stopping States ###
def stop(nav):
    '''
    Wait until the walk is finished.
    '''
    if nav.firstFrame():
        nav.setSpeed(0,0,0)
    if not nav.brain.motion.isWalkActive():
        return nav.goNow('stopped')
    return nav.stay()

def stopped(nav):
    return nav.stay()
