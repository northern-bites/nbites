from . import TrackingStates
from . import PanningStates
from . import ActiveLookStates
from . import BasicStates
from . import HeadTrackingHelper as helper
from ..util import FSA
from ..util import MyMath
from man.motion import MotionConstants# ** # debugging code

class HeadTracking(FSA.FSA):
    """FSA to control actions performed by head"""
    def __init__(self, brain):
        FSA.FSA.__init__(self, brain)
        self.brain = brain
        self.addStates(TrackingStates)
        self.addStates(PanningStates)
        self.addStates(ActiveLookStates)
        self.addStates(BasicStates)

        self.currentState = 'stopped'
        self.setPrintFunction(self.brain.out.printf)
        self.setPrintStateChanges(True)
        self.stateChangeColor = 'yellow'
        self.setName('headTracking')
        self.decisionState = 'stopped'

        self.currentHeadScan = None
        self.headMove = None

        self.activePanDir = False
        self.activeLocOn = False
        self.activePanOut = False
        self.activePanUp = False
        self.isPreKickScanning = False
        self.preActivePanHeads = None
        self.locObjectList = []
        self.locObjectList.extend(self.brain.myFieldObjects)
        self.helper = helper.HeadTrackingHelper(self)

        self.lookDirection = None
        self.target = self.brain.ball #default
        # target should either be ball or instance of FieldObject

# ** # old method
    def stopHeadMoves(self):
        """stop all head moves. In TrackingStates.py"""
        self.switchTo('stop')

# ** # old method
    def setNeutralHead(self):
        """executes sweet move to move head to neutral position.
        Does not call stop head moves. In TrackingStates.py"""
        self.switchTo('neutralHead')

# ** # old method - kept unchanged
    def performHeadMove(self, headMove):
        self.headMove = headMove
        self.switchTo('doHeadMove')

# ** # old method (main input method)
    def trackBall(self):
        """automatically tracks the ball. scans for the ball if not in view"""
        self.target = self.brain.ball
        self.gain = 1.0
        if ( (not self.currentState == 'tracking')
            and (not self.currentState == 'scanBall') ):
            self.switchTo('ballTracking')

# ** # old method (main input method)
    def trackBallSpin(self):
        """automatically tracks the ball. spins if not in view"""
        self.target = self.brain.ball
        self.gain = 1.0
        if ( (not self.currentState == 'tracking')
            and (not self.currentState == 'spinScanBall') ):
            self.switchTo('ballSpinTracking')

# ** # new method (main input method)
    def spinFindBall(self):
        """Assumes robot is spinning and looking for ball."""
        """Scans along side robot is spinning to."""
        self.switchTo('spinningPan')

# ** # old method
    def locPans(self):
        """repeatedly performs quick pan"""
        self.activeLocOn = False
        self.switchTo('locPans')

# ** # old method (main input method)
    #def activeLoc(self):
        #"""tracks the ball but periodically looks away"""
        #self.target = self.brain.ball
        #self.gain = 1.0
        #if (not self.activeLocOn):
        #    self.switchTo('activeTracking')

# ** # old method
    def preKickScan(self):
        """Scans up very quickly to see  """
        if not self.isPreKickScanning:
            self.isPreKickScanning = True
            self.switchTo('scanQuickUp')

# ** # old method
    def kickDecideScan(self):
        self.lastDiffState = 'stop'
        if self.currentState != 'trianglePan':
            self.switchTo('trianglePan')

# ** # old method
    def startScan(self,  newScan):
        """repeatedly performs passed in scan"""
        if newScan != self.currentHeadScan:
            self.currentHeadScan = newScan
            self.switchTo('scanning')

# ** # old method
    def lookToDir(self,  direction):
        """performs scripted look: down, up, right, or left"""
        if self.currentState is not 'look' or \
                self.lookDirection != direction:
            self.lookDirection = direction
            self.switchTo('look')

# ** # old method (main input method)
    def trackTarget(self, target):
        """automatically tracks landmark, scans for landmark if not in view
        only works if target has attribute locDist, framesOn, framesOff,x,y"""
        self.target = target
        self.target.height = 0
        self.gain = 1.0
        if (not self.currentState == 'targetTracking' and
            not self.currentState == 'lookToTarget' and
            not self.currentState == 'scanForTarget' ):
            self.switchTo('targetTracking')

# ** # old method
    def lookToTarget(self, target):
        """looks toward our best guess of landmark position based on loc"""
        self.target.height = 0
        self.switchTo('lookToPoint')

# ** # old method
#    def lookToPoint(self, goalX=0, goalY=0, goalHeight=0):
#        """continuously looks toward our best guess of goal based on loc"""
#        self.target.x = goalX
#        self.target.y = goalY
#        self.target.height = goalHeight
#        #allows us to update target values but not confuse FSA
#        if not self.currentState == 'lookToPoint':
#            self.switchTo('lookToPoint')

    # ** # new method - helper method
    def lookToPoint(self, goalX=0, goalY=0, goalZ=0):
        """
        Look at given relative coordinates.
        """
        self.target.x = goalX
        self.target.y = goalY
        self.target.height = goalZ
        self.helper.lookToPoint(self.target)

# ** # debugging method
    def nudge(self):
        self.nudged = True

# ** # debugging method
    def lookToAngles(self,pitch,yaw):
        self.helper.lookToAngles(yaw,pitch)

# ** # debugging method
    def printAngles(self):
        motionAngles = self.brain.sensors.motionAngles
        print MyMath.radians(motionAngles[MotionConstants.HeadYaw]),MyMath.radians(motionAngles[MotionConstants.HeadPitch])

# ** # old method
    def bounceHead(self):
        self.switchTo('bounceUp')

    # PRIMARY INPUT METHODS FOR TRACKING FSA:

# ** # new method
    def readyLoc(self):
        self.target = None
        self.decisionState = 'trackLandmarks'
        self.switchTo('trackLandmarks')

# ** # new method
    def trackBall(self):
        self.target = self.brain.ball
        self.switchTo('trackingBall')

# ** # new method
    def passiveLoc(self):
        self.target = None
        self.decisionState = 'passiveLoc'
        self.switchTo('passiveLoc')

# ** # new method
    def activeLoc(self):
        self.target = None
        self.decisionState = 'activeLoc'
        self.switchTo('activeLoc')
