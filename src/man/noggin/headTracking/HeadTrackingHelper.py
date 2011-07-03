from . import TrackingConstants as constants
import man.motion as motion
from man.motion import MotionConstants
from ..util import MyMath as MyMath
from man.motion import StiffnessModes
from math import (fabs, atan, radians, hypot)

class HeadTrackingHelper(object):
    def __init__(self, tracker):
        self.tracker = tracker

    def executeHeadMove(self, headMove):
        """performs a sweetmove"""
        for position in headMove:
            if len(position) == 4:
                move = motion.HeadJointCommand(position[1] ,# time
                                               position[0], # head pos
                                               position[3], # chain stiffnesses
                                               position[2], # interpolation type
                                               )
            else:
                self.printf("What kind of sweet ass-Move is this?")

            self.tracker.brain.motion.enqueue(move)


    def trackObject(self):
        """
        Method to actually perform the tracking.
        Should only be called explicitly from state
        methods in TrackingStates.py
        """
        #if self.firstFrame():
         #   self.brain.motion.stopHeadMoves()
        (changeX,changeY) = (0., 0.)
        # Find the target's angular distance from the center of the screen
        # if we have an object, track that
        if self.tracker.target and \
                self.tracker.target.vis.on:
            changeX = self.tracker.target.vis.angleX
            changeY = self.tracker.target.vis.angleY #the pitch is pos = down
        else:
            # by default, the change is none
            #self.printf( "No object")
            return

        motionAngles = self.tracker.brain.sensors.motionAngles
        curPitch = motionAngles[MotionConstants.HeadPitch]
        curYaw = motionAngles[MotionConstants.HeadYaw]

        maxChange = 13.0

        #Warning- no gain is applied currently!
        safeChangeX = MyMath.clip(changeX, -maxChange, maxChange )
        safeChangeY = MyMath.clip(changeY, -maxChange, maxChange )

        newYaw = curYaw + safeChangeX
        newPitch = curPitch - safeChangeY

        newYaw = MyMath.clip(newYaw, -80., 80.)

        maxSpeed = 2.0
        headMove = motion.SetHeadCommand(newYaw, newPitch,
                                         maxSpeed, maxSpeed)
        self.tracker.brain.motion.setHead(headMove)

    def panTo(self, heads):
        """
        Pan heads at appropriate speed to given heads
        """
        motionAngles = self.tracker.brain.sensors.motionAngles

        headPitch = motionAngles[MotionConstants.HeadPitch]
        headYaw = motionAngles[MotionConstants.HeadYaw]

        yawDiff = fabs(heads[0] - headYaw)
        pitchDiff = fabs(heads[1] - headPitch)


        maxDiff = max(pitchDiff, yawDiff)
        panTime = maxDiff/constants.MAX_PAN_SPEED
        self.executeHeadMove( ((heads, panTime, 0,
                                 StiffnessModes.LOW_HEAD_STIFFNESSES), ) )

    def lookToPoint(self, target):
        if hasattr(target, "height"):
            height = target.height
        else:
            height = 0
        headMove = motion.CoordHeadCommand(target.x, target.y, height)
        self.tracker.brain.motion.coordHead(headMove)

    def calcBearing(self, target):
        """returns the bearing to target in degrees. usable as headYaw"""
        my = self.tracker.brain.my

        return my.getRelativeBearing(target)

    def calcHeadPitch(self, target):
        """returns the pitch to target in degrees"""
        my = self.tracker.brain.my

        relX = target.x - my.x
        relY = target.y - my.y
        dist = hypot(relX, relY)

        lensHeightInCM = self.getCameraHeight()
        relHeight = lensHeightInCM - target.height

        #b/c we use lower angled camera we need to adjust by constant angle
        headPitch = atan(relHeight/dist) - CAMERA_ANGLE
        return headPitch

    def getCameraHeight(self):
        """gets the height of the lower camera in cm"""
        pose = self.tracker.brain.vision.pose

        cameraInWorldFrameZ = pose.cameraInWorldFrameZ
        comHeight = pose.bodyCenterHeight
        lensHeight = cameraInWorldFrameZ + comHeight
        lensHeightInCM = lensHeight/10.

        return lensHeightInCM
    """ already had to calculate bearing and groundDist to get xRelMe, yRelMe. those were stupid in the first place because they were used in CoordHeadCommand to calculate bearing again (doh!) with groundDist already calculated all that was needed was a single call to atan. """

CAMERA_ANGLE = 40.0 # from reddoc
