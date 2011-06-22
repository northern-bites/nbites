from . import TrackingStates
from . import PanningStates
from . import ActiveLookStates
from . import BasicStates
from . import HeadTrackingHelper as helper
from ..util import FSA

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
        self.setPrintStateChanges(False)
        self.stateChangeColor = 'yellow'
        self.setName('headTracking')

        self.currentHeadScan = None
        self.headMove = None

        self.goalieActiveLoc = False

        self.activePanDir = False
        self.activeLocOn = False
        self.activePanOut = False
        self.activePanUp = False
        self.isPreKickScanning = False
        self.preActivePanHeads = None
        self.helper = helper.HeadTrackingHelper(self)

        self.lookDirection = None
        self.target = self.brain.ball #default

    def stopHeadMoves(self):
        """stop all head moves. In TrackingStates.py"""
        self.switchTo('stop')

    def isStopped(self):
        """Checks that all head moves have stopped"""
        return self.currentState == 'stopped'

    def setNeutralHead(self):
        """executes sweet move to move head to neutral position.
        Does not call stop head moves. In TrackingStates.py"""
        self.switchTo('neutralHead')

    def performHeadMove(self, headMove):
        self.headMove = headMove
        self.switchTo('doHeadMove')

    def trackBall(self):
        """automatically tracks the ball. scans for the ball if not in view"""
        self.target = self.brain.ball
        self.gain = 1.0
        if ( (not self.currentState == 'tracking')
            and (not self.currentState == 'scanBall') ):
            self.switchTo('ballTracking')

    def trackBallSpin(self):
        """automatically tracks the ball. spins if not in view"""
        self.target = self.brain.ball
        self.gain = 1.0
        if ( (not self.currentState == 'tracking')
            and (not self.currentState == 'spinScanBall') ):
            self.switchTo('ballSpinTracking')

    def locPans(self):
        """repeatedly performs quick pan"""
        self.activeLocOn = False
        self.switchTo('locPans')

    def activeLoc(self):
        """tracks the ball but periodically looks away"""
        self.target = self.brain.ball
        self.gain = 1.0
        self.goalieActiveLoc = False
        if (not self.activeLocOn):
            self.switchTo('activeTracking')

    # only need to set goalieActiveLoc in this
    # and activeLoc because they are the only states
    # that directly initially call activeTracking
    def activeLocGoaliePos(self):
        """looks at the ball for shorter amount of time that activeLoc"""
        self.target = self.brain.ball
        self.gain = 1.0
        self.goalieActiveLoc = True
        if (not self.activeLocOn):
            self.switchTo('activeTracking')

    def preKickScan(self):
        """Scans up very quickly to see  """
        if not self.isPreKickScanning:
            self.isPreKickScanning = True
            self.switchTo('scanQuickUp')

    def kickDecideScan(self):
        self.lastDiffState = 'stop'
        if self.currentState != 'trianglePan':
            self.switchTo('trianglePan')

    def startScan(self,  newScan):
        """repeatedly performs passed in scan"""
        if newScan != self.currentHeadScan:
            self.currentHeadScan = newScan
            self.switchTo('scanning')

    def lookToDir(self,  direction):
        """performs scripted look: down, up, right, or left"""
        if self.currentState is not 'look' or \
                self.lookDirection != direction:
            self.lookDirection = direction
            self.switchTo('look')

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

    def lookToTarget(self, target):
        """looks toward our best guess of landmark position based on loc"""
        self.target.height = 0
        self.switchTo('lookToPoint')

    def lookToPoint(self, goalX=0, goalY=0, goalHeight=0):
        """continuously looks toward our best guess of goal based on loc"""
        self.target.x = goalX
        self.target.y = goalY
        self.target.height = goalHeight
        #allows us to update target values but not confuse FSA
        if not self.currentState == 'lookToPoint':
            self.switchTo('lookToPoint')

    def bounceHead(self):
        self.switchTo('bounceUp')
