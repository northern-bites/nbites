from . import NavConstants as constants
from math import fabs
from man.noggin.util import MyMath
from ..players import ChaseBallConstants

def getOmniWalkParam(my, dest):
    # we use distance and bearing to get relX, relY which we already have
    # for the ball. be nice not to recalculate it.
    relX, relY = 0, 0

    if hasattr(dest, "relX") and \
            hasattr(dest, "relY") and \
            hasattr(dest, "relH"):
        relX = dest.relX
        relY = dest.relY
        relH = dest.relH

    else:
        bearingDeg = my.getRelativeBearing(dest)
        distToDest = my.distTo(dest)
        relX = MyMath.getRelativeX(distToDest, bearingDeg)
        relY = MyMath.getRelativeY(distToDest, bearingDeg)
        relH = MyMath.sub180Angle(dest.h - my.h)

    # calculate forward speed
    forwardGain = constants.OMNI_GOTO_X_GAIN * relX
    sX = constants.OMNI_GOTO_FORWARD_SPEED * forwardGain
    sX = MyMath.clip(sX,
                     constants.OMNI_MIN_X_SPEED,
                     constants.OMNI_MAX_X_SPEED)
    if fabs(sX) < constants.OMNI_MIN_X_MAGNITUDE:
        sX = 0

    # calculate sideways speed
    strafeGain = constants.OMNI_GOTO_Y_GAIN * relY
    sY = constants.OMNI_GOTO_STRAFE_SPEED  * strafeGain
    sY = MyMath.clip(sY,
                     constants.OMNI_MIN_Y_SPEED,
                     constants.OMNI_MAX_Y_SPEED,)
    if fabs(sY) < constants.OMNI_MIN_Y_MAGNITUDE:
        sY = 0

    # calculate spin speed
    spinGain = constants.GOTO_SPIN_GAIN
    sTheta = MyMath.sign(relH) * getRotScale(relH) * \
        constants.OMNI_MAX_SPIN_SPEED * spinGain

    # sTheta = MyMath.clip(sTheta,
    #                      constants.OMNI_MIN_SPIN_SPEED,
    #                      constants.OMNI_MAX_SPIN_SPEED)

    if fabs(sTheta) < constants.OMNI_MIN_SPIN_MAGNITUDE:
        sTheta = 0.0

    return (sX, sY, sTheta)

def getWalkSpinParam(my, dest):

    relX = 0
    bearingDeg = my.getRelativeBearing(dest)

    if hasattr(dest, "relX"):
        relX = dest.relX
    else:
        distToDest = my.distTo(dest)
        relX = MyMath.getRelativeX(distToDest, bearingDeg)

    # calculate ideal max forward speed
    sX = constants.GOTO_FORWARD_SPEED * relX

    # calculate ideal max spin speed
    sTheta = (MyMath.sign(bearingDeg) * getRotScale(bearingDeg) *
              constants.OMNI_MAX_SPIN_SPEED)

    ## if any are below min thresholds, set to 0
    if fabs(sX) < constants.OMNI_MIN_X_MAGNITUDE:
        sX = 0

    if fabs(sTheta) < constants.MIN_SPIN_SPEED:
        sTheta = 0.0

    absSTheta = fabs(sTheta)

    if absSTheta == 0:
        sX = MyMath.clip(sX, constants.WALK_TO_MIN_X_SPEED,
                         constants.WALK_TO_MAX_X_SPEED)

    elif absSTheta <= (constants.HEADING_MEDIUM_SCALE *
                       constants.OMNI_MAX_SPIN_SPEED): #18
        sX = MyMath.clip(sX, -9, 9)

    else: #if we make getRotScale finer grained we could
        sX = 0

    return (sX, 0, sTheta)

def getWalkStraightParam(my, dest):

    distToDest = my.distTo(dest)

    if distToDest < ChaseBallConstants.APPROACH_WITH_GAIN_DIST:
        gain = constants.GOTO_FORWARD_GAIN * distToDest
    else:
        gain = 1.0

    sX = MyMath.clip(constants.GOTO_FORWARD_SPEED*gain,
                     constants.WALK_TO_MIN_X_SPEED,
                     constants.WALK_TO_MAX_X_SPEED)

    bearingDeg= my.getRelativeBearing(dest)

    sTheta = MyMath.clip(MyMath.sign(bearingDeg) *
                         constants.GOTO_STRAIGHT_SPIN_SPEED *
                         getRotScale(bearingDeg),
                         -constants.GOTO_STRAIGHT_SPIN_SPEED,
                         constants.GOTO_STRAIGHT_SPIN_SPEED )

    if fabs(sTheta) < constants.MIN_SPIN_MAGNITUDE_WALK:
        sTheta = 0

    sY = 0

    return (sX, sY, sTheta)

def getSpinOnlyParam(my, dest):
    # Determine the speed to turn
    # see if getRotScale can go faster

    headingDiff = my.getRelativeBearing(dest)
    sTheta = MyMath.sign(headingDiff) * constants.GOTO_SPIN_SPEED * \
             getRotScale(headingDiff)

    sX, sY = 0, 0
    return (sX, sY, sTheta)

def getRotScale(headingDiff):
    absHDiff = fabs(headingDiff)
    if absHDiff < constants.HEADING_NEAR_THRESH:
        return constants.HEADING_NEAR_SCALE
    elif absHDiff < constants.HEADING_MEDIUM_THRESH:
        return constants.HEADING_MEDIUM_SCALE
    else:
        return constants.HEADING_FAR_SCALE
