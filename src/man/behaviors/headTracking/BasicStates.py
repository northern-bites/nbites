import HeadMoves

def stopped(tracker):
    '''Default state where the tracker does nothing.'''
    tracker.activeLocOn = False
    return tracker.stay()

def stop(tracker):
    '''Stop all head moves.'''
    if tracker.firstFrame():
        tracker.activeLocOn = False
        request = tracker.brain.interface.motionRequest
        request.type = request.STOP_HEAD
        request.processed_by_motion = False

    if not tracker.brain.interface.motionStatus.head_is_active:
        return tracker.goNow('stopped')

    return tracker.stay()

def neutralHead(tracker):
    '''Moves head to neutral position.'''
    if tracker.firstFrame():
        tracker.activeLocOn = False
        request = tracker.brain.interface.motionRequest
        request.type = request.STOP_HEAD
        request.processed_by_motion = False
        tracker.helper.executeHeadMove(HeadMoves.NEUT_HEADS)

    if not tracker.brain.interface.motionStatus.head_is_active:
        return tracker.goLater('stopped')

    return tracker.stay()

def doHeadMove(tracker):
    '''Executes the currently set headMove, then stops.'''
    if tracker.firstFrame():
        tracker.activeLocOn = False
        request = tracker.brain.interface.motionRequest
        request.type = request.STOP_HEAD
        request.processed_by_motion = False
        tracker.helper.executeHeadMove(tracker.headMove)

    if not tracker.brain.interface.motionStatus.head_is_active:
        return tracker.goLater('stopped')

    return tracker.stay()

def repeatHeadMove(tracker):
    '''Executes the currently set headMove, then repeats it.'''
    if tracker.firstFrame():
        request = tracker.brain.interface.motionRequest
        request.type = request.STOP_HEAD
        request.processed_by_motion = False
        tracker.helper.startingPan(tracker.headMove)

    if not tracker.brain.interface.motionStatus.head_is_active:
        tracker.helper.executeHeadMove(tracker.headMove)

    return tracker.stay()

def penalizeHeads(tracker):
    '''Penalizes the heads.'''
    if tracker.firstFrame():
        request = tracker.brain.interface.motionRequest
        request.type = request.STOP_HEAD
        request.processed_by_motion = False
        tracker.helper.executeHeadMove(HeadMoves.PENALIZED_HEADS)

    if not tracker.brain.interface.motionStatus.head_is_active:
        return tracker.goLater('stopped')

    return tracker.stay()
