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
        # Initialize brain and helper, add states
        self.brain = brain
        self.addStates(TrackingStates)
        self.addStates(PanningStates)
        self.addStates(ActiveLookStates)
        self.addStates(BasicStates)
        self.helper = helper.HeadTrackingHelper(self)

        # Set debug printing variables
        self.setPrintFunction(self.brain.out.printf)
        self.setPrintStateChanges(True)
        self.stateChangeColor = 'yellow'
        self.setName('headTracking')

        # Set state variables
        self.currentState = 'stopped'
        # Note: This variable is never used.
        self.decisionState = 'stopped'

        # Set sweetmove and scan variables and enums
        self.currentHeadScan = None
        self.headMove = None
        self.lookDirection = None
        self.kickName = ""
        # Note: Used only by scanQuickUp
        self.preActivePanHeads = None

        # Set object variables
        # Note: This variables is never used.
        self.locObjectList = []
        self.locObjectList.extend(self.brain.myFieldObjects)
        self.target = self.brain.ball #default
        # target should either be ball or instance of FieldObject

        # Set flag variables
        self.goalieActiveLoc = False
        # Note: Following 2 variables are never used.
        self.activePanDir = False
        self.activePanOut = False
        self.activeLocOn = False
        self.activePanUp = False
        # Note: Used only by scanQuickUpx
        self.isPreKickScanning = False

    def stopHeadMoves(self):
        """stop all head moves. In TrackingStates.py"""
        if self.currentState is not 'stopped' and \
                self.currentState is not 'stop':
            self.switchTo('stop')

    def isStopped(self):
        """Checks that all head moves have stopped."""
        return self.currentState == 'stopped'

    def setNeutralHead(self):
        """Executes sweet move to move head to neutral position, then stops."""
        self.switchTo('neutralHead')

    def performHeadMove(self, headMove):
        """Executes the given headMove, then stops."""
        self.headMove = headMove
        self.switchTo('doHeadMove')

    # Consider tweaking.
    def trackBall(self):
        """
        Enters a state cycle:
        When ball is in view, tracks via vision values.
        When ball is not in view, executes naive pans.
        """
        self.target = self.brain.ball
        self.gain = 1.0
        if ( self.currentState is not 'tracking'
             and self.currentState is not 'scanBall'
             and self.currentState is not 'ballTracking'
             and self.currentState is not 'activeTracking'
             and self.currentState is not 'trianglePan'):
            self.switchTo('tracking')

    # Consider tweaking.
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

    # Do we still want to do this?
    # Consider changing.
    def locPans(self):
        """Repeatedly performs quick pan."""
        self.activeLocOn = False
        self.switchTo('locPans')

    # Marked for deprecation
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

    # Marked for deprecation
    def activeLocGoaliePos(self):
        """looks at the ball for shorter amount of time than activeLoc"""
        self.target = self.brain.ball
        self.gain = 1.0
        self.goalieActiveLoc = True
        if (not self.activeLocOn):
            self.switchTo('activeTracking')

    # Probably needs fixing.
    def kickDecideScan(self):
        """Execute a triangle pan."""
        self.lastDiffState = 'stop' # ensure we go back to stop after return
        self.goalieActiveLoc = False
        if (self.currentState != 'trianglePan' or
            self.currentState != 'trianglePanLeft' or
            self.currentState != 'trianglePanRight' or
            self.currentState != 'trianglePanReturn'):
            self.switchTo('trianglePan')

    # Might need adjustments for current kicks.
    def afterKickScan(self, name):
        """
        After a kick, looks in the appropriate direction
        that the ball was kicked in.
        """
        self.target = self.brain.ball
        self.kickName = name
        self.switchTo('afterKickScan')

    def startScan(self, newScan):
        """Repeatedly performs the given scan."""
        if newScan != self.currentHeadScan:
            self.currentHeadScan = newScan
            self.switchTo('scanning')

    def lookToDir(self,  direction):
        """
        Performs scripted look: down, up, right, or left.
        If ball becomes visible, go to state 'ballTracking'.
        """
        if self.currentState is not 'look' or \
                self.lookDirection != direction:
            self.lookDirection = direction
            self.switchTo('look')

    # Clarify and simplify.
    def trackTarget(self, target):
        """
        TODO: Pick a comment!!!!

        Someone else: Track given target via vision information while possible.
        Once target is no longer visible, perform naive pans.
        If target becomes visible again, either return to state
        'targetTracking' or go to state 'activeTracking'.

        Lizzie: automatically tracks landmark, scans for landmark if not in view
        only works if target has attribute loc.dist, framesOn, framesOff,x,y
        """
        self.target = target
        self.target.height = 0
        self.gain = 1.0
        if (not self.currentState == 'targetTracking' and
            not self.currentState == 'lookToTarget' and
            not self.currentState == 'scanForTarget' ):
            self.switchTo('targetTracking')

    def lookToTarget(self, target):
        """Look towards given target, using localization values."""
        self.target.height = 0
        self.switchTo('lookToPoint')

    # Currently bypasses states.
    # Not called anywhere in code.
    # Either move into a state or remove.
    def lookToPoint(self, goalX=0, goalY=0, goalZ=0):
        """
        Continuously looks at given relative coordinates.
        """
        self.target.x = goalX
        self.target.y = goalY
        self.target.height = goalZ
        self.helper.lookToPoint(self.target)
