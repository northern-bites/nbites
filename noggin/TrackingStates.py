
from man import motion
from man.motion import MotionConstants
import util.MyMath as MyMath

DEBUG = False
TRACKER_FRAMES_OFF_REFIND_THRESH = 7 #num frms after which to switch to scanfindbl

def stopped(tracker):
    '''default state where the tracker does nothing'''
    return tracker.stay()

def stop(tracker):
    ''' stop all head moves '''
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater('stopped')

    return tracker.stay()

def ballTracking(tracker): #Super state which handles following/refinding the ball
    if tracker.target.framesOff <= TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goNow('tracking')
    else:
        return tracker.goNow('scanBall')

def tracking(tracker):
    ''' state askes it's parent (the tracker) for an object or angles to track
    while the object is on screen, or angles are passed, we track it.
    Otherwise, we continually write the current values into motion via setHeads.

    If a sweet move is begun while we are tracking, the current setup is to let
    the sweet move conclude and then resume tracking afterward.'''

    if tracker.firstFrame():
        #supersede anything in the motion queue:
        tracker.brain.motion.stopHeadMoves()
        if DEBUG: tracker.printf("stopping head moves, first frame",'cyan')

    (changeX,changeY) = (0.,0.)
    #Find the target's angular distance from the center of the screen
    #if we have an object, track that
    if tracker.target != None:

        if tracker.target.on:
            changeX = tracker.target.angleX
            changeY = tracker.target.angleY #the pitch is pos = down
        else:
            if DEBUG : tracker.printf("Missing object this frame",'cyan')
            if tracker.target == tracker.brain.ball and \
                    tracker.target.framesOff > TRACKER_FRAMES_OFF_REFIND_THRESH:
                return tracker.goLater(tracker.lastDiffState)
            return tracker.stay()
    #otherwise, if we have angles, track by those
    elif tracker.angleX != None and tracker.angleY !=None:
        changeX = tracker.angleX
        changeY = tracker.angleY #the pitch is pos = down
    else:
        #by default, the change is none
        tracker.player.printf( "No object")
        return tracker.stay()

    def getGain(angleToCover):
        ''' choose our gain by default on how far we need to go'''
        #should this be based on distance?
        angleToCover = abs(angleToCover)
        if angleToCover > 8:
            return 0.3*tracker.gain
        if angleToCover > 4:
            return .2*tracker.gain
        if angleToCover > .5:
            return .1*tracker.gain
        return 0.

    xGain = getGain(changeX)
    yGain = getGain(changeY)

    curPitch = tracker.brain.sensors.motionAngles[MotionConstants.HeadPitch]
    curYaw = tracker.brain.sensors.motionAngles[MotionConstants.HeadYaw]

    if DEBUG:
        print "curHead (%g,%g), gain (%g,%g) change (%g,%g)" %\
            (curYaw,curPitch,xGain,yGain,changeX,changeY)

    maxChange = 13.0

    #Warning- no gain is applied currently!
    safeChangeX = MyMath.clip(changeX,-maxChange,maxChange )
    safeChangeY = MyMath.clip(changeY,-maxChange,maxChange )

    newYaw = curYaw + safeChangeX/3
    newPitch = curPitch - safeChangeY/3

    if newYaw < 0:
        newYaw = max(newYaw,-80.0)
    else:
        newYaw = min(newYaw,80.0)

    if DEBUG:
        print "target: x %g, y%g" % (newYaw,newPitch)

    maxSpeed = 2.0
    #motion.stopHeadMoves()
    #headMove = motion.HeadJointCommand(.15,(newYaw,newPitch),1)
    headMove = motion.SetHeadCommand(newYaw,newPitch,
                                     maxSpeed, maxSpeed)
    tracker.brain.motion.setHead(headMove)

    return tracker.stay()


def activeTracking(tracker):
    ball = tracker.brain.ball

    if ball.framesOn > 30 and tracker.framesSinceActiveLoc > 45: #ball vel?
        tracker.framesSinceActiveLoc = 0
        return tracker.goNow('panLeftOnce')

    return tracker.goNow('tracking')


