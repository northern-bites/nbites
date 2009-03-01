import man.motion.SweetMoves as SweetMoves

def scanBall(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        print "Enqueing head motion"
        tracker.execute(SweetMoves.SCAN_BALL)

    return tracker.stay()

def locPans(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        print "Enqueing head motion"
        tracker.execute(SweetMoves.LOC_PANS)
    return tracker.stay()

def panLeftOnce(tracker):
    if tracker.firstFrame():
        tracker.execute(SweetMoves.PAN_LEFT)

    if not tracker.brain.motion.isHeadActive():
        if tracker.lastDiffState == 'tracking':
            return tracker.goNow('tracking')

        return tracker.goLater('nothing')

    return tracker.stay()

def panRightOnce(tracker):
    if tracker.firstFrame():
        tracker.execute(SweetMoves.PAN_RIGHT)

    if not tracker.brain.motion.isHeadActive():
        if tracker.lastDiffState == 'tracking':
            return tracker.goNow('tracking')

        return tracker.goLater('nothing')

    return tracker.stay()
