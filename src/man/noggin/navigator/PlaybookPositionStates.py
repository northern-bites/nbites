from . import NavConstants as constants
from . import NavHelper as helper
from . import WalkHelper as walker
from . import NavTransitions as navTrans
from ..playbook import PBConstants
from man.noggin.util import MyMath

DEBUG = False

def playbookWalk(nav):
    """positions us in any state using position from playbook"""
    if nav.firstFrame():
        nav.omniWalkToCount = 0
        nav.playbookAtPositionCount = 0

    my = nav.brain.my
    dest = nav.brain.play.getPosition()

    relX = 0
    if hasattr(dest, "relX"):
        relX = dest.relX
    else:
        bearingDeg = my.getRelativeBearing(dest)
        distToDest = my.distTo(dest)
        relX = MyMath.getRelativeX(distToDest, bearingDeg)

    if (navTrans.atDestinationCloser(my, dest) and
        navTrans.atHeading(my, dest.h)):
            nav.playbookAtPositionCount += 1
            if nav.playbookAtPositionCount > \
                    constants.FRAMES_THRESHOLD_TO_POSITION_PLAYBOOK:
                return nav.goNow('playbookAtPosition')
    else:
        if not nav.playbookAtPositionCount == 0:
            nav.playbookAtPositionCount -= 1

    dest.h = my.headingTo(dest)

    if (nav.brain.play.isRole(PBConstants.GOALIE) and
        not nav.brain.play.isSubRole(PBConstants.READY_GOALIE)):
        if (relX < 0):
            return nav.goNow('playbookGoalieWalk')
        else:
            walkX, walkY, walkTheta = walker.getWalkSpinParam(my, dest)
            helper.setSpeed(nav, walkX, walkY, walkTheta)

    else:
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

    relX = 0
    if hasattr(dest, "relX"):
        relX = dest.relX
    else:
        bearingDeg = my.getRelativeBearing(dest)
        distToDest = my.distTo(dest)
        relX = MyMath.getRelativeX(distToDest, bearingDeg)

    if (navTrans.atDestinationCloser(my, dest) and
        navTrans.atHeading(my, dest.h)):
        nav.playbookAtPositionCount += 1
        if nav.playbookAtPositionCount > \
                constants.FRAMES_THRESHOLD_TO_POSITION_PLAYBOOK:
            return nav.goNow('playbookAtPosition')
    else:
        if not nav.playbookAtPositionCount == 0:
            nav.playbookAtPositionCount -= 1

    walkX, walkY, walkTheta = walker.getOmniWalkParam(my, dest)
    helper.setSpeed(nav, walkX, walkY, walkTheta)

    if not navTrans.useFinalHeading(nav.brain, dest):
        nav.stopOmniCount += 1

    if (nav.stopOmniCount > \
            constants.FRAMES_THRESHOLD_TO_POSITION_PLAYBOOK):
        if (nav.brain.play.isRole(PBConstants.GOALIE) and
            not nav.brain.play.isSubRole(PBConstants.READY_GOALIE)):
            if (relX < 0):
                return nav.goLater('playbookGoalieWalk')
            else:
                print relX
                return nav.goLater('playbookWalk')
        else:
            return nav.goLater('playbookWalk')
    else:
        nav.stopOmniCount = 0

    if navTrans.shouldAvoidObstacle(nav):
        return nav.goLater('avoidObstacle')

    return nav.stay()

def playbookGoalieWalk(nav):
    if nav.firstFrame():
        nav.stopOmniCount = 0
        nav.playbookAtPositionCount = 0

    my = nav.brain.my
    dest = nav.brain.play.getPosition()

    relX = 0
    if hasattr(dest, "relX"):
        relX = dest.relX
    else:
        bearingDeg = my.getRelativeBearing(dest)
        distToDest = my.distTo(dest)
        relX = MyMath.getRelativeX(distToDest, bearingDeg)

    if (navTrans.atDestinationCloser(my, dest) and
        navTrans.atHeading(my, dest.h)):
        nav.playbookAtPositionCount += 1
        if nav.playbookAtPositionCount > \
                constants.FRAMES_THRESHOLD_TO_POSITION_PLAYBOOK:
            return nav.goNow('playbookAtPosition')
    else:
        if not nav.playbookAtPositionCount == 0:
            nav.playbookAtPositionCount -= 1

    dest.h = my.headingTo(dest)

    if (nav.brain.play.isRole(PBConstants.GOALIE) and
        not nav.brain.play.isSubRole(PBConstants.READY_GOALIE)):
        if (relX > 0):
            return nav.goNow('playbookWalk')
        else:
            walkX, walkY, walkTheta = walker.getWalkBackParam(my, dest)
            helper.setSpeed(nav, walkX, walkY, walkTheta)
    else:
        return nav.goNow('playbookWalk')

    if navTrans.useFinalHeading(nav.brain, dest):
        nav.omniWalkToCount += 1
        if nav.omniWalkToCount > constants.FRAMES_THRESHOLD_TO_POSITION_OMNI:
            return nav.goLater('playbookOmni')
    else:
        nav.omniWalkToCount = 0

    if navTrans.shouldAvoidObstacle(nav):
        return nav.goLater('avoidObstacle')

    return nav.stay()

def playbookAtPosition(nav):
    if nav.firstFrame():
        helper.setSpeed(nav, 0, 0, 0)
        nav.startOmniCount = 0

    my = nav.brain.my
    dest = nav.brain.play.getPosition()

    if (navTrans.atDestinationCloser(my, dest) and
        navTrans.atHeading(my, dest.h)):
        nav.startOmniCount = 0
        return nav.stay()

    else:
        nav.startOmniCount += 1
        if nav.startOmniCount > constants.FRAMES_THRESHOLD_TO_POSITION_OMNI:
            return nav.goLater('playbookOmni')
        else:
            return nav.stay()
