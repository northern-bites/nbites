import man.motion.SweetMoves as SweetMoves

TRACKER_FRAMES_ON_TRACK_THRESH = 1 #num frms after which to switch to scanfindbl

def scanBall(tracker):


    if tracker.target == tracker.brain.ball and \
            tracker.target.framesOn >= TRACKER_FRAMES_ON_TRACK_THRESH:
        return tracker.goNow('ballTracking')
    if tracker.firstFrame():
        # move head to beginning (0.0,SCAN_BALL start head yaw)
        # Move the head at 4 deg /sec
        moveTime = (abs(tracker.brain.sensors.motionAngles[0]) / 23.0)
        tracker.execute((((0.0,SweetMoves.SCAN_BALL[0][0][1]),moveTime,1),))

    if not tracker.brain.motion.isHeadActive():
        #print "Enqueing head motion"
        tracker.execute(SweetMoves.SCAN_BALL)

    return tracker.stay()

def locPans(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        #print "Enqueing head motion"
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
