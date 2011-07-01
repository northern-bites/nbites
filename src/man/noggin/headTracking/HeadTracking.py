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

        self.goalieActiveLoc = False

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
        self.kickDirection = None
        self.target = self.brain.ball #default
        # target should either be ball or instance of FieldObject

# ** # old method - should keep
    def stopHeadMoves(self):
        """Stop all head moves."""
        self.switchTo('stop')

# ** # new method - Used in player FSA
    def isStopped(self):
        """Checks that all head moves have stopped."""
        return self.currentState == 'stopped'

# ** # old method - should keep
    def setNeutralHead(self):
        """Executes sweet move to move head to neutral position."""
        self.switchTo('neutralHead')

# ** # old method - kept unchanged
    def performHeadMove(self, headMove):
        """Executes the given headMove, then stops."""
        self.headMove = headMove
        self.switchTo('doHeadMove')

# ** # old method (main input method)
    def trackBall(self):
        """
        Enters a state cycle:
        When ball is in view, tracks via vision values.
        When ball is not in view, executes naive pans.
        """
        self.target = self.brain.ball
        self.gain = 1.0
        if ( (not self.currentState == 'tracking')
            and (not self.currentState == 'scanBall') ):
            self.switchTo('ballTracking')

# ** # old method (main input method)
    def trackBallSpin(self):
        """
        Enters a state cycle:
        When ball is in view, tracks via vision values.
        When ball is not in view, continuously executes short pans
        based on direction robot is spinning.
        """
        self.target = self.brain.ball
        self.gain = 1.0
        if ( (not self.currentState == 'tracking')
            and (not self.currentState == 'spinScanBall') ):
            self.switchTo('ballSpinTracking')

# ** # old method
    def locPans(self):
        """Repeatedly performs quick pan."""
        self.activeLocOn = False
        self.switchTo('locPans')

# ** # old method (main input method)
# ** # look into cleaning this up
    def activeLoc(self):
        """
        If ball is visible and close, track it via vision values.
        If ball is not visible, execute naive pans.
        If state counter is low enough, track ball via vision values.
        If state counter is high enough, perform triangle pans
        and return to last head angles.
        """
        self.target = self.brain.ball
        self.gain = 1.0
        self.goalieActiveLoc = False
        if (not self.activeLocOn):
            self.switchTo('activeTracking')

    # only need to set goalieActiveLoc in this
    # and activeLoc because they are the only states
    # that directly initially call activeTracking

# ** # goalie method
    def activeLocGoaliePos(self):
        """looks at the ball for shorter amount of time that activeLoc"""
        self.target = self.brain.ball
        self.gain = 1.0
        self.goalieActiveLoc = True
        if (not self.activeLocOn):
            self.switchTo('activeTracking')

# ** # old method
# ** # CLEAN THIS UP
    def preKickScan(self):
        """
        Pan up once (quickly), then stop.
        """
        if not self.isPreKickScanning:
            self.isPreKickScanning = True
            self.switchTo('scanQuickUp')

# ** # old method
    def kickDecideScan(self):
        """Execute a triangle pan."""
        self.lastDiffState = 'stop' # doesn't seem to do anything...
        self.goalieActiveLoc = False
        if self.currentState != 'trianglePan':
            self.switchTo('trianglePan')

# ** # old method
    def startScan(self, newScan):
        """Repeatedly performs the given scan."""
        if newScan != self.currentHeadScan:
            self.currentHeadScan = newScan
            self.switchTo('scanning')

# ** # old method
    def lookToDir(self,  direction):
        """
        Performs scripted look: down, up, right, or left.
        If ball becomes visible, go to state 'ballTracking'.
        """
        if self.currentState is not 'look' or \
                self.lookDirection != direction:
            self.lookDirection = direction
            self.switchTo('look')

# ** # old method (main input method)
    def trackTarget(self, target):
        """
        Track given target via vision information while possible.
        Once target is no longer visible, perform naive pans.
        If target becomes visible again, either return to state
        'targetTracking' or go to state 'activeTracking'.
        """
        self.target = target
        self.target.height = 0
        self.gain = 1.0
        if (not self.currentState == 'targetTracking' and
            not self.currentState == 'lookToTarget' and
            not self.currentState == 'scanForTarget' ):
            self.switchTo('targetTracking')

# ** # old method - deprecated (replaced by helper.lookToTargetCoords)
    def lookToTarget(self, target):
        """Look towards given target, using localization values."""
        self.target.height = 0
        self.switchTo('lookToPoint')

# ** # old method - deprecated (replaced)
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
        Continuously looks at given relative coordinates.
        """
        self.target.x = goalX
        self.target.y = goalY
        self.target.height = goalZ
        self.helper.lookToPoint(self.target)

# ** # debugging method
    def lookToAngles(self,pitch,yaw):
        """Looks to the given head angles. """
        self.helper.lookToAngles(yaw,pitch)

# ** # old method
    def bounceHead(self):
        """Continually bounce head up and down."""
        self.switchTo('bounceUp')
