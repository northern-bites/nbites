
import math

from man import motion 
from man.motion import MotionConstants

DEBUG = False


def nothing(tracker):
    '''default state where the tracker does nothing'''
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
    return tracker.stay()


def tracking(tracker):
    ''' state askes it's parent (the tracker) for an object or angles to track
    while the object is on screen, or angles are passed, we track it.
    Otherwise, we continually write the current values into motion via setHeads.

    If a sweet move is begun while we are tracking, the current setup is to let
    the sweet move conclude and then resume tracking afterward.'''
    tracker.brain.motion

    if tracker.firstFrame():
        #supersede anything in the motion queue:
        tracker.brain.motion.stopHeadMoves()
        if DEBUG: print "stopping head moves, first frame"

    (changeX,changeY) = (0.,0.)
    #Find the target's angular distance from the center of the screen
    #if we have an object, track that
    if tracker.target != None:

        if tracker.target.on:
            changeX = tracker.target.angleX
            changeY = tracker.target.angleY #the pitch is pos = down
        else:
            if DEBUG : print "Missing object this frame"
            return tracker.stay()
    #otherwise, if we have angles, track by those
    elif tracker.angleX != None and tracker.angleY !=None:
        changeX = tracker.angleX
        changeY = tracker.angleY #the pitch is pos = down
    else:
        #by default, the change is none
        tracker.player.printf( "No object")
        return tracker.stay()


    def clip(orig, minVal, maxVal):
        if orig > 0:
            return min(orig,maxVal)
        else:
            return max(orig,minVal)

    def getGain(angleToCover):
        ''' choose our gain by default on how far we need to go'''
        #should this be based on distance?
        angleToCover = abs(angleToCover)
        if angleToCover > 8:
            return 0.5*tracker.gain
        if angleToCover > 4:
            return .5*tracker.gain
        if angleToCover > .5:
            return .2*tracker.gain
        return 0.

    xGain = getGain(changeX)
    yGain = getGain(changeY)

    curPitch = tracker.brain.sensors.angles[MotionConstants.HeadPitch]
    curYaw = tracker.brain.sensors.angles[MotionConstants.HeadYaw]

    if DEBUG:
        print "curHead (%g,%g), gain (%g,%g) change (%g,%g)" %\
            (curYaw,curPitch,xGain,yGain,changeX,changeY)

    maxChange = 13.0

    #Warning- no gain is applied currently!
    safeChangeX = clip(changeX,-maxChange,maxChange )
    safeChangeY = clip(changeY,-maxChange,maxChange )

    newYaw = curYaw + safeChangeX
    newPitch = curPitch - safeChangeY

    if newYaw < 0:
        newYaw = max(newYaw,-80.0)
    else:
        newYaw = min(newYaw,80.0)

    if DEBUG:
        print "target: x %g, y%g" % (newYaw,newPitch)

    #motion.stopHeadMoves()
    #headMove = motion.HeadJointCommand(.15,(newYaw,newPitch),1)
    headMove = motion.SetHeadCommand(newYaw,newPitch)
    tracker.brain.motion.setHead(headMove)

    return tracker.stay()


def activeTracking(tracker):
    ball = tracker.brain.ball

    if ball.framesOn > 30 and tracker.framesSinceActiveLoc > 45: #ball vel?
        tracker.framesSinceActiveLoc = 0
        return tracker.goNow('panLeftOnce')
    
    return tracker.goNow('tracking')

def scan(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        print "Enqueing head motion"
        scan1 = motion.HeadJointCommand(3., ( 65.0, 20.0), 1)
        scan2 = motion.HeadJointCommand(2., (65.,-10.), 1)
        scan3 = motion.HeadJointCommand(5.0, (-65.,-10.),1)
        scan4 = motion.HeadJointCommand(2., (-65.0, 20.0), 1)
        scan5 = motion.HeadJointCommand(3., ( 0.0, 20.0), 1)

        tracker.brain.motion.enqueue(scan1)
        tracker.brain.motion.enqueue(scan2)
        tracker.brain.motion.enqueue(scan3)
        tracker.brain.motion.enqueue(scan4)
        tracker.brain.motion.enqueue(scan5)

    return tracker.stay()
