from . import NavConstants as constants
from . import NavHelper as helper
from . import WalkHelper as walker
from . import NavTransitions as navTrans

DEBUG = False

def playbookWalk(nav):
    """positions us in any state using position from playbook"""
    if nav.firstFrame():
        nav.omniWalkToCount = 0
        nav.playbookAtPositionCount = 0

    my = nav.brain.my
    dest = nav.brain.play.getPosition()

    if (navTrans.atDestinationCloser(my, dest) and
        navTrans.atHeading(my, dest.h)):
            nav.playbookAtPositionCount += 1
            if nav.playbookAtPositionCount > constants.FRAMES_THRESHOLD_TO_POSITION_PLAYBOOK:
                return nav.goNow('playbookAtPosition')
    else:
        if not nav.playbookAtPositionCount == 0:
            nav.playbookAtPositionCount -= 1

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
        nav.playbookAtPositionCount = 0

    my = nav.brain.my
    dest = nav.brain.play.getPosition()

    if (navTrans.atDestinationCloser(my, dest) and
        navTrans.atHeading(my, dest.h)):
        nav.playbookAtPositionCount += 1
        if nav.playbookAtPositionCount > constants.FRAMES_THRESHOLD_TO_POSITION_PLAYBOOK:
            return nav.goNow('playbookAtPosition')
    else:
        nav.playbookAtPositionCount = 0

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
        nav.brain.speech.say("At Position")
        helper.setSpeed(nav, 0, 0, 0)
        nav.startOmniCount = 0

    my = nav.brain.my
    dest = nav.brain.play.getPosition()

    if navTrans.atDestinationCloser(my, dest) and navTrans.atHeading(my, dest.h):
        nav.startOmniCount = 0
        return nav.stay()

    else:
        nav.startOmniCount += 1
        if nav.startOmniCount > constants.FRAMES_THRESHOLD_TO_POSITION_OMNI:
            return nav.goLater('playbookOmni')
        else:
            return nav.stay()
