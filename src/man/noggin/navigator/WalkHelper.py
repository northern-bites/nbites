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
    hDiff = MyMath.sub180Angle(dest.h - my.h)

    if (fabs(hDiff) < 2.0):
        sTheta = 0.0
    else:
        sTheta = MyMath.sign(hDiff) * getRotScale(hDiff) * \
                 constants.OMNI_MAX_SPIN_SPEED * spinGain

        sTheta = MyMath.clip(sTheta,
                             constants.OMNI_MIN_SPIN_SPEED,
                             constants.OMNI_MAX_SPIN_SPEED)

    return (sX, sY, sTheta)

def getWalkSpinParam(my, dest):

    relX = 0
    bearingDeg = my.getRelativeBearing(dest)
    distToDest = my.distTo(dest)
    if hasattr(dest, "relX"):
        relX = dest.relX
    else:
        relX = MyMath.getRelativeX(distToDest, bearingDeg)

    # calculate ideal max forward speed
    sX = constants.GOTO_FORWARD_SPEED * MyMath.sign(relX)

    if (fabs(bearingDeg) < 2.0):
        sTheta = 0.0
    else:
        # calculate ideal max spin speed
        sTheta = (MyMath.sign(bearingDeg) * getRotScale(bearingDeg) *
                  constants.OMNI_MAX_SPIN_SPEED)

    absSTheta = fabs(sTheta)

    if  fabs(bearingDeg) > 20:
        sX = MyMath.clip(sX,
                         constants.OMNI_MIN_X_SPEED,
                         constants.OMNI_MAX_X_SPEED)
        sTheta = MyMath.sign(sTheta)* constants.OMNI_MAX_SPIN_SPEED

    elif fabs(bearingDeg)  > 35:
        sX = 0
        sTheta = constants.MAX_SPIN_SPEED * MyMath.sign(sTheta)

    gain = 1.0
    if distToDest < ChaseBallConstants.APPROACH_WITH_GAIN_DIST:
        gain = constants.GOTO_CLOSE_GAIN

    return (sX * gain, 0, sTheta * gain)

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

    if (fabs(bearingDeg) < 2.0):
        sTheta = 0.0
    else:
        sTheta = MyMath.clip(MyMath.sign(bearingDeg) *
                             constants.GOTO_STRAIGHT_SPIN_SPEED *
                             getRotScale(bearingDeg),
                             -constants.GOTO_STRAIGHT_SPIN_SPEED,
                             constants.GOTO_STRAIGHT_SPIN_SPEED )
    sY = 0

    return (sX, sY, sTheta)

def getSpinOnlyParam(my, dest):
    # Determine the speed to turn
    # see if getRotScale can go faster

    headingDiff = my.getRelativeBearing(dest)
    if (fabs(headingDiff) < 2.0):
        sTheta = 0.0
    else:
        sTheta = MyMath.sign(headingDiff) * constants.GOTO_SPIN_SPEED * \
                 getRotScale(headingDiff)

    sX, sY = 0, 0
    return (sX, sY, sTheta)

def getRotScale(headingDiff):
    absHDiff = fabs(headingDiff)

    return absHDiff / 90.0

def getCloseRotScale(headingDiff):
    absHDiff = fabs(headingDiff)

    return absHDiff / 50.0
