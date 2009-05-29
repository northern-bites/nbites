import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves

TRACKER_FRAMES_ON_TRACK_THRESH = 1 #num frms after which to switch to scanfindbl

def scanBall(tracker):


    if tracker.target == tracker.brain.ball and \
            tracker.target.framesOn >= TRACKER_FRAMES_ON_TRACK_THRESH:
        return tracker.goNow('ballTracking')
#     if tracker.firstFrame():
#         # move head to beginning (0.0,SCAN_BALL start head yaw)
#         # Move the head at 4 deg /sec
#         moveTime = (abs(tracker.brain.sensors.motionAngles[0]) / 23.0)
#         tracker.execute((((0.0,HeadMoves.SCAN_BALL[0][0][1]),moveTime,1),))

    if not tracker.brain.motion.isHeadActive():
        tracker.execute(HeadMoves.SCAN_BALL)

    return tracker.stay()

def scanning(tracker):
    if (tracker.firstFrame() or not
        tracker.brain.motion.isHeadActive()):
        tracker.execute(tracker.currentHeadScan)
    return tracker.stay()

def locPans(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        tracker.execute(HeadMoves.QUICK_PANS)
    return tracker.stay()

def panLeftOnce(tracker):
    if tracker.firstFrame():
        tracker.execute(HeadMoves.PAN_LEFT)

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater(tracker.lastDiffState)

    return tracker.stay()

def panRightOnce(tracker):
    if tracker.firstFrame():
        tracker.execute(HeadMoves.PAN_RIGHT)

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater(tracker.lastDiffState)

    return tracker.stay()

def postScan(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        tracker.execute(HeadMoves.POST_SCAN)
    return tracker.stay()

def activeLocScan(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        tracker.execute(HeadMoves.QUICK_PANS)
    if tracker.target.on:
        return tracker.goLater('activeTracking')
    return tracker.stay()
