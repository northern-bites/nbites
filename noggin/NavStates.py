
''' States for finding our way on the field '''

from man import motion
from man.motion import MotionConstants

def nothing(nav):
    return nav.stay()

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
