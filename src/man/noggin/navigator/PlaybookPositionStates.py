from . import NavConstants as constants
from . import NavHelper as helper
from ..playbook.PBConstants import GOALIE
from math import fabs

DEBUG = False

def playbookWalk(nav):
    """positions us in ready state"""
    if nav.firstFrame():
        nav.spinToPointCount = 0
        nav.omniWalkToCount = 0

    my = nav.brain.my
    dest = nav.dest

    dest.h = my.getTargetHeading(dest)

    walkX, walkY, walkTheta = helper.getWalkStraightParam(my, dest)
    helper.setSpeed(nav, walkX, walkY, walkTheta)

    # this order is important! the other way he will attempt to spin and walk
    # to a position very close behind him
    if helper.useFinalHeading(nav.brain, dest):
        nav.omniWalkToCount += 1
        if nav.omniWalkToCount > constants.FRAMES_THRESHOLD_TO_POSITION_OMNI:
            return nav.goLater('playbookOmni')
    else:
        nav.omniWalkToCount = 0

    bearingDiff = fabs(my.getRelativeBearing(dest))
    if bearingDiff > constants.HEADING_THRESHOLD_TO_SPIN:
        nav.spinToPointCount += 1
        if nav.spinToPointCount > constants.FRAMES_THRESHOLD_TO_SPIN:
            return nav.goLater('playbookSpin')
    else:
        nav.spinToPointCount = 0

    return nav.stay()

def playbookOmni(nav):
    if nav.firstFrame():
        nav.stopOmniCount = 0
        nav.spinToPointCount = 0

    my = nav.brain.my
    dest = nav.dest

    walkX, walkY, walkTheta = helper.getOmniWalkParam(my, dest)
    helper.setSpeed(nav, walkX, walkY, walkTheta)

    if nav.brain.play.isRole(GOALIE):
        if helper.atDestinationGoalie(my, dest) and helper.atHeading(my, dest.h):
            return nav.goNow('stop')
    else:
        if helper.atDestinationCloser(my, dest) and helper.atHeading(my, dest.h):
            return nav.goNow('stop')

    if helper.useFinalHeading(nav.brain, dest):
        headingDiff = fabs(my.getRelativeBearing(dest))
        if headingDiff > constants.HEADING_THRESHOLD_TO_SPIN:
            nav.spinToPointCount += 1
            if nav.spinToPointCount > constants.FRAMES_THRESHOLD_TO_SPIN:
                return nav.goLater('playbookFinalSpin')
            else:
                nav.spinToPointCount = 0

    else:
        nav.stopOmniCount += 1
        if nav.stopOmniCount > constants.FRAMES_THRESHOLD_TO_POSITION_PLAYBOOK:
            return nav.goLater('playbookWalk')

    return nav.stay()

def playbookSpin(nav):
    if nav.firstFrame():
        nav.walkOmniCount = 0
        nav.spinToPointCount = 0
        nav.walkToPointCount = 0

    my = nav.brain.my
    dest = nav.dest

    dest.h = my.getTargetHeading(dest)

    walkX, walkY, walkTheta = helper.getSpinOnlyParam(my, dest)
    helper.setSpeed(nav, walkX, walkY, walkTheta)

    if helper.useFinalHeading(nav.brain, dest):
        nav.walkOmniCount += 1
        if nav.walkOmniCount > constants.FRAMES_THRESHOLD_TO_POSITION_OMNI:
            return nav.goLater('playbookOmni')
    else:
        nav.walkOmniCount = 0

    headingDiff = fabs(my.getRelativeBearing(dest))
    if headingDiff < constants.HEADING_THRESHOLD_TO_SPIN:
        nav.walkToPointCount += 1
        if nav.walkToPointCount > constants.FRAMES_THRESHOLD_TO_SPIN:
            return nav.goLater('playbookWalk')
    else:
        nav.walkToPointCount = 0

    return nav.stay()

def playbookFinalSpin(nav):
    if nav.firstFrame():
        nav.walkOmniCount = 0
        nav.walkToPointCount = 0

    my = nav.brain.my
    dest = nav.dest

    walkX, walkY, walkTheta = helper.getSpinOnlyParam(my, dest)
    helper.setSpeed(nav, walkX, walkY, walkTheta)

    if not helper.atDestinationGoalie(my, dest):
        if helper.useFinalHeading(nav.brain, dest):
            nav.walkOmniCount += 1
            if nav.walkOmniCount > constants.FRAMES_THRESHOLD_TO_POSITION_OMNI:
                return nav.goLater('playbookOmni')
        else:
            nav.walkOmniCount = 0
            nav.walkToPointCount += 1
            if nav.walkToPointCount > constants.FRAMES_THRESHOLD_TO_SPIN:
                return nav.goLater('playbookWalk')
            else:
                nav.walkToPointCount = 0
    else:
        if helper.atHeading(my, dest.h):
            return nav.goNow('stop')

    return nav.stay()
