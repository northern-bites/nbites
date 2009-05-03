
from . import TrackingStates
from . import PanningStates
from .util import FSA
import man.motion as motion
from man.motion import MotionConstants
import util.MyMath as MyMath

#constants need to go in another file.
#maybe auto generated on boot?
TRACKING = 'tracking'

LOC_PANS = 'locPans'

PAN_LEFT_ONCE = 'panLeftOnce'

class HeadTracking(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(TrackingStates)
        self.addStates(PanningStates)

        self.currentState = 'stopped'
        self.setPrintFunction(self.brain.out.printf)
        self.setPrintStateChanges(True)
        self.stateChangeColor = 'yellow'
        self.setName('headTracking')
        self.activePanDir = False
        self.currentHeadScan = None

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
        return self.currentState == TRACKING

    def stopHeadMoves(self):
        self.switchTo('stop')

    def trackBall(self):
        self.target = self.brain.ball
        self.gain = 1.0
        if(self.currentState != 'tracking' or self.currentState != 'scanBall'):
           self.switchTo('ballTracking')

    def locPans(self):
        self.stopHeadMoves()
        self.switchTo('locPans')

    def execute(self,sweetMove):
        for position in sweetMove:
            if len(position) == 6:
                move = motion.BodyJointCommand(position[4], #time
                                               position[0], #larm
                                               position[1], #lleg
                                               position[2], #rleg
                                               position[3], #rarm
                                               position[5], #interpolation type
                                               )
                self.brain.motion.enqueue(move)

            elif len(position) == 3:
                move = motion.HeadJointCommand(position[1],#time
                                               position[0],#head pos
                                               position[2],#interpolation type
                                                   )
                self.brain.motion.enqueue(move)

            else:
                print "What kind of sweet ass-Move is this?"

    def activeLoc(self):
        self.target = self.brain.ball
        self.gain = 1.0
        if(self.currentState != 'panLeftOnce' or self.currentState != 'panRightOnce' or
           self.currentState != 'activeTracking'):
           self.switchTo('activeTracking')

    def trackObject(self):
        """
        Method to actually perform the tracking.  Should only be called explicitly from state
        methods in TrackingStates.py
        """
        if self.firstFrame():
            #supersede anything in the motion queue:
            self.brain.motion.stopHeadMoves()

        (changeX,changeY) = (0.,0.)
        # Find the target's angular distance from the center of the screen
        # if we have an object, track that
        if self.target != None:

            if self.target.on:
                changeX = self.target.angleX
                changeY = self.target.angleY #the pitch is pos = down

        #otherwise, if we have angles, track by those
        elif self.angleX != None and self.angleY !=None:
            changeX = self.angleX
            changeY = self.angleY #the pitch is pos = down
        else:
            # by default, the change is none
            self.player.printf( "No object")
            return

        def getGain(angleToCover):
            ''' choose our gain by default on how far we need to go'''
            #should this be based on distance?
            angleToCover = abs(angleToCover)
            if angleToCover > 8:
                return 0.3*self.gain
            if angleToCover > 4:
                return .2*self.gain
            if angleToCover > .5:
                return .1*self.gain
            return 0.

        xGain = getGain(changeX)
        yGain = getGain(changeY)

        curPitch = self.brain.sensors.motionAngles[MotionConstants.HeadPitch]
        curYaw = self.brain.sensors.motionAngles[MotionConstants.HeadYaw]

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

        maxSpeed = 2.0
        headMove = motion.SetHeadCommand(newYaw,newPitch,
                                         maxSpeed, maxSpeed)
        self.brain.motion.setHead(headMove)

    def startScan(self, newScan):
        if newScan != self.currentHeadScan:
            self.currentHeadScan = newScan
            self.switchTo('scanning')
