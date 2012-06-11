from man.motion import HeadMoves

def stopped(tracker):
    '''Default state where the tracker does nothing.'''
    tracker.activeLocOn = False
    return tracker.stay()

def stop(tracker):
    '''Stop all head moves.'''
    if tracker.firstFrame():
        tracker.activeLocOn = False
        tracker.brain.motion.stopHeadMoves()

    if not tracker.brain.motion.isHeadActive():
        return tracker.goNow('stopped')

    return tracker.stay()

def neutralHead(tracker):
    '''Moves head to neutral position.'''
    if tracker.firstFrame():
        tracker.activeLocOn = False
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.executeHeadMove(HeadMoves.NEUT_HEADS)

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater('stopped')

    return tracker.stay()

def doHeadMove(tracker):
    '''Executes the currently set headMove, then stops.'''
    if tracker.firstFrame():
        tracker.activeLocOn = False
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.executeHeadMove(tracker.headMove)

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater('stopped')

    return tracker.stay()

def repeatHeadMove(tracker):
    '''Executes the currently set headMove, then repeats it.'''
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.startingPan(tracker.firstHeadMove)

    if not tracker.brain.motion.isHeadActive():
        tracker.helper.executeHeadMove(tracker.headMove)

    return tracker.stay()
