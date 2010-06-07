from . import NavConstants as constants
from . import NavHelper as helper
from . import WalkHelper as walker
from . import NavTransitions as navTrans

DEBUG = False

def playbookWalk(nav):
    """positions us in any state using position from playbook"""
    if nav.firstFrame():
        nav.omniWalkToCount = 0

    my = nav.brain.my
    dest = nav.brain.play.getPosition()

    # TODO: buffer this switch
    if navTrans.atDestinationCloser(my, dest) and navTrans.atHeading(my, dest.h):
        return nav.goNow('playbookAtPosition')

    dest.h = my.headingTo(dest)

    walkX, walkY, walkTheta = walker.getWalkSpinParam(my, dest)
    helper.setSpeed(nav, walkX, walkY, walkTheta)

    if navTrans.useFinalHeading(nav.brain, dest):
        nav.omniWalkToCount += 1
        if nav.omniWalkToCount > constants.FRAMES_THRESHOLD_TO_POSITION_OMNI:
            return nav.goLater('playbookOmni')
    else:
        nav.omniWalkToCount = 0

    if navTrans.shouldAvoidObstacle(nav):
        return nav.goLater('avoidObstacle')

    return nav.stay()

def playbookOmni(nav):
    if nav.firstFrame():
        nav.stopOmniCount = 0

    my = nav.brain.my
    dest = nav.brain.play.getPosition()

    # TODO: buffer this switch
    if navTrans.atDestinationCloser(my, dest) and navTrans.atHeading(my, dest.h):
        return nav.goNow('playbookAtPosition')

    walkX, walkY, walkTheta = walker.getOmniWalkParam(my, dest)
    helper.setSpeed(nav, walkX, walkY, walkTheta)

    if not navTrans.useFinalHeading(nav.brain, dest):
        nav.stopOmniCount += 1
        if nav.stopOmniCount > constants.FRAMES_THRESHOLD_TO_POSITION_PLAYBOOK:
            return nav.goLater('playbookWalk')
    else:
        nav.stopOmniCount = 0

    if navTrans.shouldAvoidObstacle(nav):
        return nav.goLater('avoidObstacle')

    return nav.stay()

def playbookAtPosition(nav):
    if nav.firstFrame():
        helper.setSpeed(nav, 0, 0, 0)

    my = nav.brain.my
    dest = nav.brain.play.getPosition()

    # TODO: buffer this switch
    if navTrans.atDestination(my, dest) and navTrans.atHeading(my, dest.h):
        return nav.stay()

    else:
        return nav.goLater('playbookOmni')
