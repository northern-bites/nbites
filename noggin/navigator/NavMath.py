from math import fabs
from . import NavConstants as constants
from ..util import MyMath

def atDestination(nav):
    """
    Returns true if we are at an (x, y) close enough to the one we want
    """
#         self.printf("X diff is " + str(self.brain.my.x - self.destX))
#         self.printf("Y diff is " + str(self.brain.my.y - self.destY))
    return (fabs(nav.brain.my.x - nav.destX) < constants.CLOSE_ENOUGH_XY
            and fabs(nav.brain.my.y - nav.destY) < constants.CLOSE_ENOUGH_XY)

def atDestinationCloser(nav):
    """
    Returns true if we are at an (x, y) close enough to the one we want
    """
#         self.printf("X diff is " + str(self.brain.my.x - self.destX))
#         self.printf("Y diff is " + str(self.brain.my.y - self.destY))
    return (fabs(nav.brain.my.x - nav.destX) < constants.CLOSER_XY
            and fabs(nav.brain.my.y - nav.destY) < constants.CLOSER_XY)

def atDestinationGoalie(nav):
    return (fabs(nav.brain.my.x - nav.destX) < constants.GOALIE_CLOSE_X
            and fabs(nav.brain.my.y - nav.destY) < constants.GOALIE_CLOSE_Y)

def atHeadingGoTo(nav, targetHeading):
    hDiff = fabs(MyMath.sub180Angle(nav.brain.my.h - targetHeading))
    #self.printf("H diff is " + str(hDiff))
    return hDiff < constants.AT_HEADING_GOTO_DEG

def atHeading(nav, targetHeading = None):
    """
    Returns true if we are at a heading close enough to what we want
    """
    if targetHeading is None:
        targetHeading = nav.destH
    hDiff = fabs(MyMath.sub180Angle(nav.brain.my.h - targetHeading))
    #self.printf("H diff is " + str(hDiff))
    return hDiff < constants.CLOSE_ENOUGH_H and \
           nav.brain.my.uncertH < constants.LOC_IS_ACTIVE_H

def notAtHeading(nav, targetHeading= None):
    if targetHeading is None:
        targetHeading = nav.destH
    hDiff = fabs(MyMath.sub180Angle(nav.brain.my.h - targetHeading))
    #self.printf("H diff is " + str(hDiff))
    return hDiff > constants.ALMOST_CLOSE_ENOUGH_H and \
            nav.brain.my.uncertH < constants.LOC_IS_ACTIVE_H

def getRotScale(headingDiff):
    absHDiff = fabs(headingDiff)
    if absHDiff < constants.HEADING_NEAR_THRESH:
        return constants.HEADING_NEAR_SCALE
    elif absHDiff < constants.HEADING_MEDIUM_THRESH:
        return constants.HEADING_MEDIUM_SCALE
    else:
        return constants.HEADING_FAR_SCALE
