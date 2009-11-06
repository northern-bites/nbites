
from . import TrackingConstants as constants
from . import TrackingStates
from . import PanningStates
from .util import FSA
import man.motion as motion
from man.motion import MotionConstants
import util.MyMath as MyMath
from man.motion import StiffnessModes
from math import (fabs, atan, sqrt)

class HeadTracking(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(TrackingStates)
        self.addStates(PanningStates)

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

    def ballTrackingHelper(self,args):
        self.target = None
        self.gain = 1.0
        self.angleX = None
        self.angleY = None
        if len(args) == 4:
            (self.target,self.gain,self.angleX,self.angleY) = args
        elif len(args) ==2:
            (self.target,self.gain) = args
        elif len(args) == 1:
            self.target = args[0]
        else:
            raise ValueError("Num params are wrong: besure to"+\
                             " specifiy both angleX and angleY"+str(len(args)))

    def isTracking(self):
        return self.currentState == constants.TRACKING

    def stopHeadMoves(self):
        self.switchTo('stop')

    def trackBall(self):
        self.target = self.brain.ball
        self.gain = 1.0
        if (not self.currentState == 'tracking') and\
                (not self.currentState == 'scanBall'):
           self.switchTo('ballTracking')

    def locPans(self):
        self.activeLocOn = False
        self.stopHeadMoves()
        self.switchTo('locPans')

    def scanBall(self):
        self.activeLocOn = False
        self.stopHeadMoves()
        self.switchTo('ballScan')

    def execute(self,sweetMove):
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

    def activeLoc(self):
        self.target = self.brain.ball
        self.gain = 1.0
        if( not self.activeLocOn ):
            self.switchTo('activeTracking')

    def trackObject(self):
        """
        Method to actually perform the tracking.
        Should only be called explicitly from state
        methods in TrackingStates.py
        """
        #if self.firstFrame():
         #   self.brain.motion.stopHeadMoves()
        (changeX,changeY) = (0.,0.)
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

        newYaw = MyMath.clip(newYaw,-80.,80.)

        maxSpeed = 2.0
        headMove = motion.SetHeadCommand(newYaw,newPitch,
                                         maxSpeed, maxSpeed)
        self.brain.motion.setHead(headMove)

    def startScan(self, newScan):
        if newScan != self.currentHeadScan:
            self.currentHeadScan = newScan
            self.switchTo('scanning')

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

    def lookToDir(self, direction):
        if self.currentState is not 'look' or \
                self.lookDirection != direction:
            self.lookDirection = direction
            self.switchTo('look')

    def lookToLandmark(self, landmark):
        self.lookToPoint(landmark.x, landmark.y, 0)

    def lookToPoint(self, visGoalX, visGoalY, visGoalH):
        relX = 0 #calculate myX relative to VisionGoalX
        relY = 0 #calculate myY relative to VisionGoalY
        relH = 0 #calculate myH relative to VisionGoalH
        self.lookToRelativePoint(relX, relY, relH)

    def lookToRelativePoint(self, relVisGoalX, relVisGoalY, relVisGoalH):
        goalYaw = self.calculateGoalYaw(relVisGoalX, relVisGoalY)
        goalPitch = self.calculateGoalPitch(relVisGoalX, relVisGoalY,
                                            relVisGoalH)
        headMove = motion.SetHeadCommand(goalYaw, goalPitch, 2.0, 2.0)
        self.brain.motion.setHead(headMove)

    def calculateGoalYaw(relX, relY):
        localRelY = relY
        if localRelY <= 0:
            localRelY = .001
        goalYaw = atan(relX/localRelY)
        return goalYaw

    def calculateGoalPitch(relX, relY, relH):
        dist = sqrt(relX*relX, relY*relY)
        pitch = atan(relH/dist)
        return pitch
