from . import TrackingConstants as constants

def goalieActiveLook(tracker):
    '''goalie looks at ball, then own left post, opp left post, opp right post,
    own right post and repeats'''
    #field cross and goalbox corners prbly more useful
    STARE_TIME = 30
    if tracker.firstFrame():
        tracker.lookToLandmark(tracker.brain.ball)
    elif STARE_TIME < (tracker.counter % STARE_TIME*5) < STARE_TIME*2:
        tracker.lookToLandmark(tracker.brain.myGoalLeftPost)
    elif (tracker.counter % STARE_TIME*5) < STARE_TIME*3:
        tracker.lookToLandmark(tracker.brain.oppGoalLeftPost)
    elif (tracker.counter % STARE_TIME*5) < STARE_TIME*4:
        tracker.lookToLandmark(tracker.brain.oppGoalRightPost)
    elif (tracker.counter % STARE_TIME*5) < STARE_TIME*5:
        tracker.lookToLandmark(tracker.brain.myGoalRightPost)
    else:
        tracker.lookToLandmark(tracker.brain.ball)

    return tracker.stay()

def activeLookBGLP(tracker):
    #need to do every frame
    bglp = tracker.brain.bglp
    if bglp.on:
        tracker.trackLandmark(bglp)
    else:
        tracker.lookToLandmark(bglp)

def landmarkScan(tracker):
    if tracker.firstFrame():
        b = tracker.brain
        landmarks = [ b.myGoalLeftPost, b.oppGoalLeftPost,
                      b.oppGoalRightPost, b.myGoalRightPost ]
        i = iter(landmarks)
        print landmarks

    try:
        print tracker.counter % constants.LOOK_TO_TIME_TO_FIND
        if ((tracker.counter % constants.LOOK_TO_TIME_TO_FIND) == 0):
            print "getting next landmark"
            p = i.next()

    except StopIteration:
        return tracker.goNow('stop')
    #is switching us out of this state
    #tracker.trackLandmark(p)

    return tracker.stay()

def lookToPoint(self, tracker):
    """look to an absolute position on the field"""
    lensHeightInCM = self.tracker.helper.getCameraHeight()
    my = self.tracker.brain.my
    relX = tracker.visGoalX - my.x
    relY = tracker.visGoalY - my.y
        #relH is relative to camera height. negative is normal
    relHeight = tracker.visGoalHeight - (lensHeightInCM)
    headMove = self.tracker.Motion.CoordHeadCommand(relX, relY,
                                                    relHeight, my.h)
    self.tracker.brain.motion.coordHead(headMove)

    return tracker.stay()

