''' States for finding our way on the field '''

from man import motion
from man.motion import MotionConstants

def nothing(nav):
    return nav.stay()

### Walk Straight States ###
def walkStraightTo(nav):
    nav.brain.motion.setWalkConfig(*MotionConstants.WALK_STRAIGHT_CONFIGS[0])
    nav.brain.motion.setWalkExtraConfig(*MotionConstants.WALK_STRAIGHT_CONFIGS[1])

    straight = motion.WalkStraight(nav.distToDest,30)
    nav.brain.motion.setNextWalkCommand(straight)

def walkStraightForever(nav):
    if nav.firstFrame() or not nav.brain.motion.isWalkActive():
        nav.brain.motion.setWalkConfig(*MotionConstants.WALK_STRAIGHT_CONFIGS[0])
        nav.brain.motion.setWalkExtraConfig(*MotionConstants.WALK_STRAIGHT_CONFIGS[1])
        straight = motion.WalkStraight(700.0, #800cm is diag of field-ish
                                       MotionConstants.CYCLES_PER_STEP)
        nav.brain.motion.setNextWalkCommand(straight)
    return nav.stay()


def walkStraight(nav):
    if nav.arrived():
        if nav.shouldFinalTurnRight():
            return nav.goNow('finalTurnRight')
        elif nav.shouldFinalTurnLeft():
            return nav.goNow('finalTurnLeft')
        else:
            return nav.goNow('stop')
    elif nav.shouldInitialTurnLeft():
        return nav.goNow('initalTurnLeft')
    elif nav.shouldInitialTurnRight():
        return nav.goNow('initalTurnRight')

    if nav.firstFrame() or not nav.brain.motion.isWalkActive():
        nav.brain.motion.setWalkConfig(*MotionConstants.WALK_STRAIGHT_CONFIGS[0])
        nav.brain.motion.setWalkExtraConfig(*MotionConstants.WALK_STRAIGHT_CONFIGS[1])
        #requeue another walk because I haven't arrived or I am just starting
        nav.brain.motion.stopBodyMoves()
        straight = motion.WalkStraight(nav.distToDest,
                                       MotionConstants.CYCLES_PER_STEP)
        nav.brain.motion.setNextWalkCommand(straight)
        #What happens on cpp motion end when we stop body moves AND call a new walk in the same frame
        pass


    return nav.goLater('walkStraight')


### Turning States ###
def turn(nav):
    if nav.firstFrame():
        if nav.degreesToTurn>35:
            nav.brain.motion.setWalkConfig(*MotionConstants.WALK_TURN_BIG_CONFIGS[0])
            nav.brain.motion.setWalkExtraConfig(*MotionConstants.WALK_TURN_BIG_CONFIGS[1])

        else:
            nav.brain.motion.setWalkConfig(*MotionConstants.WALK_TURN_SMALL_CONFIGS[0])
            nav.brain.motion.setWalkExtraConfig(*MotionConstants.WALK_TURN_SMALL_CONFIGS[1])

        turnMove = motion.WalkTurn(nav.degreesToTurn,30)
        nav.brain.motion.setNextWalkCommand(turnMove)

    elif not nav.brain.motion.isWalkActive():
        nav.goLater('stopped')

    return nav.stay()

def turnLeftForever(nav):
    if nav.firstFrame() or not nav.brain.motion.isWalkActive():
        nav.brain.motion.setWalkConfig(*MotionConstants.WALK_TURN_BIG_CONFIGS[0])
        nav.brain.motion.setWalkExtraConfig(*MotionConstants.WALK_TURN_BIG_CONFIGS[1])
        turn = motion.WalkTurn(360.0, 
                               MotionConstants.CYCLES_PER_STEP)
        nav.brain.motion.setNextWalkCommand(turn)

    return nav.stay()

def turnRightForever(nav):
    if nav.firstFrame() or not nav.brain.motion.isWalkActive():
        nav.brain.motion.setWalkConfig(*MotionConstants.WALK_TURN_BIG_CONFIGS[0])
        nav.brain.motion.setWalkExtraConfig(*MotionConstants.WALK_TURN_BIG_CONFIGS[1])

        turn = motion.WalkTurn(-360.0,
                               MotionConstants.CYCLES_PER_STEP)
        nav.brain.motion.setNextWalkCommand(turn)

    return nav.stay()

# -jgm still needs to set walk configs
def initialTurnLeft(nav):
    # Am I where I want to be?
    if nav.arrived():
        if nav.shouldFinalTurnRight():
            return nav.goNow('finalTurnRight')
        elif nav.shouldFinalTurnLeft():
            return nav.goNow('finalTurnLeft')
        else:
            return nav.goNow('stop')

    # If I should switch directions
    elif nav.shouldInitialTurnRight():
        return nav.goNow('initialTurnRight')

    # Am I facing my destination
    elif nav.shouldWalkStraight():
        return nav.goNow('walkStraight')

    # Should I queue another turn?
    if nav.firstFrame() or not nav.brain.motion.isWalkActive():
        nav.brain.motion.stopBodyMoves()
        turn = motion.WalkTurn(nav.bearingToDest,
                               MotionConstants.CYCLES_PER_STEP)
        nav.brain.motion.setNextWalkCommand(walk)
# -jgm still needs to set walk configs
def initialTurnRight(nav):
    # Am I where I want to be?
    if nav.arrived():
        if nav.shouldFinalTurnRight():
            return nav.goNow('finalTurnRight')
        elif nav.shouldFinalTurnLeft():
            return nav.goNow('finalTurnLeft')
        else:
            return nav.goNow('stop')

    # If I should switch directions
    elif nav.shouldInitialTurnLeft():
        return nav.goNow('initialTurnLeft')

    # Am I facing my destination
    elif nav.shouldWalkStraight():
        return nav.goNow('walkStraight')

    # Should I queue another turn?
    if nav.firstFrame() or not nav.brain.motion.isWalkActive():
        nav.brain.motion.stopBodyMoves()
        turn = motion.WalkTurn(nav.bearingToDest,
                               MotionConstants.CYCLES_PER_STEP)
        nav.brain.motion.setNextWalkCommand(walk)

# -jgm still needs to set walk configs
def initialTurnRight(nav):
    # Am I where I want to be?
    if nav.arrived():
        if nav.shouldFinalTurnRight():
            return nav.goNow('finalTurnRight')
        elif nav.shouldFinalTurnLeft():
            return nav.goNow('finalTurnLeft')
        else:
            return nav.goNow('stop')

    # If I should switch directions
    elif nav.shouldInitialTurnLeft():
        return nav.goNow('initialTurnLeft')

    # Am I facing my destination
    elif nav.shouldWalkStraight():
        return nav.goNow('walkStraight')

    # Should I queue another turn?
    if nav.firstFrame() or not nav.brain.motion.isWalkActive():
        nav.brain.motion.stopBodyMoves()
        turn = motion.WalkTurn(nav.bearingToDest,
                               MotionConstants.CYCLES_PER_STEP)
        nav.brain.motion.setNextWalkCommand(walk)
# -jgm still needs to set walk configs
def finalTurnRight(nav):
    # Am I where I want to be?
    if not nav.arrived():
        if nav.shouldInitialTurnLeft():
            return nav.goNow('initialTurnLeft')
        elif nav.shouldInitialTurnRight():
            return nav.goNow('initialTurnLeft')
        else:
            return nav.goNow('walkStraight')
    else:
        #should switch directions?
        if nav.shouldFinalTurnLeft():
            return nav.goNow('finalTurnLeft')
        elif nav.shouldStop:
            return nav.goNow('stop')

    # Should I queue another turn?
    if nav.firstFrame() or not nav.brain.motion.isWalkActive():
        nav.brain.motion.stopBodyMoves()
        turn = motion.WalkTurn(nav.bearingToDestHeading,
                               MotionConstants.CYCLES_PER_STEP)
        nav.brain.motion.setNextWalkCommand(walk) 

# -jgm still needs to set walk configs
def finalTurnLeft(nav):
    # Am I where I want to be?
    if not nav.arrived():
        if nav.shouldInitialTurnLeft():
            return nav.goNow('initialTurnLeft')
        elif nav.shouldInitialTurnRight():
            return nav.goNow('initialTurnLeft')
        else:
            return nav.goNow('walkStraight')
    else:
        #should switch directions?
        if nav.shouldFinalTurnRight():
            return nav.goNow('finalTurnRigt')
        elif nav.shouldStop:
            return nav.goNow('stop')

    # Should I queue another turn?
    if nav.firstFrame() or not nav.brain.motion.isWalkActive():
        nav.brain.motion.stopBodyMoves()
        turn = motion.WalkTurn(nav.bearingToDestHeading,
                               MotionConstants.CYCLES_PER_STEP)
        nav.brain.motion.setNextWalkCommand(walk) 

### Walk Sideways ##
def walkSidewaysTo(nav):

    nav.brain.motion.setWalkConfig(*MotionConstants.WALK_SIDEWAYS_CONFIGS[0])
    nav.brain.motion.setWalkExtraConfig(*MotionConstants.WALK_SIDEWAYS_CONFIGS[1])
    walkSide = motion.WalkSideways(nav.distToDest,30)

### Stopping States ###

def stop(nav):
    '''
    Wait until the walk is finished.
    '''
    if nav.firstFrame():
        nav.brain.motion.stopBodyMoves()

    if not nav.brain.motion.isWalkActive():
        return nav.goNow('stopped')

    return nav.stay()
    

def stopped(nav):
    return nav.stay()
