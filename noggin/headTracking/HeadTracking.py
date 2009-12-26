from . import TrackingConstants as constants
from . import TrackingStates
from . import PanningStates
from ..util import FSA
from . import ActiveLookStates
import man.motion as motion
from man.motion import MotionConstants
import ..util.MyMath as MyMath
from man.motion import StiffnessModes
from math import (fabs)

class HeadTracking(FSA.FSA):
    """FSA to control actions performed by head"""
    def __init__(self, brain):
        FSA.FSA.__init__(self, brain)
        self.brain = brain
        self.addStates(TrackingStates)
        self.addStates(PanningStates)
        self.addStates(ActiveLookStates)

        self.currentState = 'stopped'
        self.setPrintFunction(self.brain.out.printf)
        self.setPrintStateChanges(False)
        self.stateChangeColor = 'yellow'
        self.setName('headTracking')

        self.currentHeadScan = None

        self.activePanDir = False
        self.activeLocOn = False
        self.activePanOut = False
        self.activePanUp = False
        self.preActivePanHeads = None

        self.lookDirection = None


    def execute(self, sweetMove):
        """performs a sweetmove"""
        for position in sweetMove:
            if len(position) == 7:
                move = motion.BodyJointCommand(position[4], #time
                                               position[0], #larm
                                               position[1], #lleg
                                               position[2], #rleg
                                               position[3], #rarm
                                               position[6], # Chain Stiffnesses
                                               position[5], #interpolation type
                                               )


            elif len(position) == 4:
                move = motion.HeadJointCommand(position[1] ,# time
                                               position[0], # head pos
                                               position[3], # chain stiffnesses
                                               position[2], # interpolation type
                                                   )

            elif len(position) == 5:
                move = motion.BodyJointCommand(position[2], # time
                                               position[0], # chainID
                                               position[1], # chain angles
                                               position[4], # chain stiffnesses
                                               position[3], # interpolation type
                                               )

            else:
                self.printf("What kind of sweet ass-Move is this?")

            self.brain.motion.enqueue(move)


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
        if self.target and \
                self.target.on:
            changeX = self.target.angleX
            changeY = self.target.angleY #the pitch is pos = down
        else:
            # by default, the change is none
            #self.printf( "No object")
            return

        motionAngles = self.brain.sensors.motionAngles
        curPitch = motionAngles[MotionConstants.HeadPitch]
        curYaw = motionAngles[MotionConstants.HeadYaw]

        maxChange = 13.0

        #Warning- no gain is applied currently!
        safeChangeX = MyMath.clip(changeX,-maxChange,maxChange )
        safeChangeY = MyMath.clip(changeY,-maxChange,maxChange )

        newYaw = curYaw + safeChangeX/3
        newPitch = curPitch - safeChangeY/3

        newYaw = MyMath.clip(newYaw,-80., 80.)

        maxSpeed = 2.0
        headMove = motion.SetHeadCommand(newYaw,newPitch,
                                         maxSpeed, maxSpeed)
        self.brain.motion.setHead(headMove)

    def panTo(self, heads):
        """
        Pan heads at appropriate speed to given heads
        """
        motionAngles = self.brain.sensors.motionAngles

        headPitch = motionAngles[MotionConstants.HeadPitch]
        headYaw = motionAngles[MotionConstants.HeadYaw]

        yawDiff = fabs(heads[0] - headYaw)
        pitchDiff = fabs(heads[1] - headPitch)


        maxDiff = max(pitchDiff, yawDiff)
        panTime = maxDiff/constants.MAX_PAN_SPEED
        self.execute( ((heads, panTime, 0,
                        StiffnessModes.LOW_HEAD_STIFFNESSES),) )


    def getCameraHeight(self):
        """gets the height of the (hopefully)lower camera in cm"""
        pose = self.brain.vision.pose
        cameraInWorldFrameZ = pose.cameraInWorldFrameZ
        comHeight = pose.bodyCenterHeight
        lensHeight = cameraInWorldFrameZ + comHeight
        lensHeightInCM = lensHeight/10.
        return lensHeightInCM
