import math

import motion as motionCore
import motion.MotionConstants as MotionConstants

DEBUG = True


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
    motion = tracker.brain.motion

    if tracker.firstFrame():
        #supersede anything in the motion queue:
        motion.stopHeadMoves()
        print "stopping head moves, first frame"

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
    
    safeChangeX = xGain*changeX #clip(xGain*changeX,-maxChange,maxChange )
    safeChangeY = yGain*changeY #clip(yGain*changeY,-maxChange,maxChange )

    newYaw = curYaw + safeChangeX
    newPitch = curPitch - safeChangeY

    if newYaw < 0:
        newYaw = max(newYaw,-80.0)
    else:
        newYaw = min(newYaw,80.0)

    if DEBUG:
        print "target: x %g, y%g" % (newYaw,newPitch)

    motion.stopHeadMoves()
    headMove = motionCore.HeadJointCommand(.15,[newYaw,newPitch],0)
    
    motion.enqueue(headMove)

    return tracker.stay()


def activeTracking(tracker):
    ball = tracker.brain.ball

    if ball.framesOn > 30 and tracker.framesSinceActiveLoc > 45: #ball vel?
        tracker.framesSinceActiveLoc = 0
        return tracker.goNow('panLeftOnce')
    
    return tracker.goNow('tracking')

def scan(tracker):
    scan1 = motionCore.HeadJointCommand(3., [ 65.0, 30.0], 0)
    scan2 =  motionCore.HeadJointCommand(2., [65.,10.], 0)
    scan3 = motionCore.HeadJointCommand(5.0, [-65.,10.],0)
    scan4 = motionCore.HeadJointCommand(2., [-65.0, 30.0], 0)
    scan5 = motionCore.HeadJointCommand(3., [ 0.0, 30.0], 0)

    headScan = motionCore.HeadScanCommand([scan5, scan1, scan2, scan3,scan4], True)
    tracker.brain.motion.enqueue(headScan)
    return tracker.stay()
