from . import NavConstants as constants
from . import NavHelper as helper
from ..playbook.PBConstants import GOALIE

DEBUG = False

def playbookWalk(nav):
    """positions us in ready state"""
    if nav.firstFrame():
        nav.spinToPointCount = 0
        nav.omniWalkToCount = 0

    my = nav.brain.my
    dest = nav.brain.play.getPosition()

    if nav.brain.play.isRole(GOALIE):
        if helper.atDestinationGoalie(my, dest) and helper.atHeading(my, dest.h):
            return nav.goNow('stop')
    else:
        if helper.atDestinationCloser(my, dest) and helper.atHeading(my, dest.h):
            return nav.goNow('stop')

    dest.h = my.headingTo(dest)

    walkX, walkY, walkTheta = helper.getWalkSpinParam(my, dest)
    helper.setSpeed(nav, walkX, walkY, walkTheta)

    # this order is important! the other way he will attempt to spin and walk
    # to a position very close behind him
    if helper.useFinalHeading(nav.brain, dest):
        nav.omniWalkToCount += 1
        if nav.omniWalkToCount > constants.FRAMES_THRESHOLD_TO_POSITION_OMNI:
            return nav.goLater('playbookOmni')
    else:
        nav.omniWalkToCount = 0

    return nav.stay()

def playbookOmni(nav):
    if nav.firstFrame():
        nav.stopOmniCount = 0
        nav.spinToPointCount = 0

    my = nav.brain.my
    dest = nav.brain.play.getPosition()

    if nav.brain.play.isRole(GOALIE):
        if helper.atDestinationGoalie(my, dest) and helper.atHeading(my, dest.h):
            return nav.goNow('stop')
    else:
        if helper.atDestinationCloser(my, dest) and helper.atHeading(my, dest.h):
            return nav.goNow('stop')

    walkX, walkY, walkTheta = helper.getOmniWalkParam(my, dest)
    helper.setSpeed(nav, walkX, walkY, walkTheta)

    if not helper.useFinalHeading(nav.brain, dest):
        nav.stopOmniCount += 1
        if nav.stopOmniCount > constants.FRAMES_THRESHOLD_TO_POSITION_PLAYBOOK:
            return nav.goLater('playbookWalk')
    else:
        nav.stopOmniCount = 0

    return nav.stay()
