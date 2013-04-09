import HeadMoves

def stopped(tracker):
    '''Default state where the tracker does nothing.'''
    return tracker.stay()

def stop(tracker):
    '''Stop all head moves.'''
    if tracker.firstFrame():
        # Send the motion request message to stop
        request = tracker.brain.interface.motionRequest
        request.type = request.RequestType.STOP_HEAD
        request.processed_by_motion = False

    if not tracker.brain.motion.head_is_active:
        return tracker.goNow('stopped')

    return tracker.stay()

def neutralHead(tracker):
    '''Moves head to neutral position.'''
    if tracker.firstFrame():
        # Send the motion request message to stop
        request = tracker.brain.interface.motionRequest
        request.type = request.RequestType.STOP_HEAD
        request.processed_by_motion = False
        # Perform the headmove
        tracker.helper.executeHeadMove(HeadMoves.NEUT_HEADS)

    if not tracker.brain.motion.head_is_active:
        return tracker.goLater('stopped')

    return tracker.stay()

def doHeadMove(tracker):
    '''Executes the currently set headMove, then stops.'''
    # Send the motion request message to stop
    request = tracker.brain.interface.motionRequest
    request.type = request.RequestType.STOP_HEAD
    request.processed_by_motion = False
    # Perform the headmove
    tracker.helper.executeHeadMove(tracker.headMove)

    return tracker.goLater('doingHeadMove')

def doingHeadMove(tracker):
    '''Robot state while performing a headmove via doHeadMove state '''
    if not tracker.brain.motion.head_is_active:
        return tracker.goLater('stopped')

    return tracker.stay()

def repeatHeadMove(tracker):
    '''Executes the currently set headMove, then repeats it.'''
    if tracker.firstFrame():
        # Send the motion request message to stop
        request = tracker.brain.interface.motionRequest
        request.type = request.RequestType.STOP_HEAD
        request.processed_by_motion = False
        # Perform the headmove
        tracker.helper.startingPan(tracker.headMove)

    return tracker.goLater('repeatingHeadMove')

def repeatingHeadMove(tracker):
    '''Robot state while repeating a headmove via repeatHeadMove state'''

    if not tracker.brain.motion.head_is_active:
        tracker.helper.executeHeadMove(tracker.headMove)

    return tracker.stay()

def penalizeHeads(tracker):
    '''Penalizes the heads.'''
    if tracker.firstFrame():
        # Send the motion request to stop
        request = tracker.brain.interface.motionRequest
        request.type = request.RequestType.STOP_HEAD
        request.processed_by_motion = False
        # Perform the headmove
        tracker.helper.executeHeadMove(HeadMoves.PENALIZED_HEADS)

    if not tracker.brain.motion.head_is_active:
        return tracker.goLater('stopped')

    return tracker.stay()
