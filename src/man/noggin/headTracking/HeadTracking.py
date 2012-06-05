from . import TrackingStates
from . import PanningStates
from . import ActiveLookStates
from . import BasicStates
from . import HeadTrackingHelper as helper
import man.motion.HeadMoves as HeadMoves
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

        # Set object variables
        # Note: This variable is never used.
        self.locObjectList = []
        self.locObjectList.extend(self.brain.myFieldObjects)
        self.target = self.brain.ball #default
        # target should either be ball or instance of FieldObject

        # Set flag variables
        self.goalieActiveLoc = False
        self.activeLocOn = False
        self.activePanUp = False

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

    # Note: safe to call every frame.
    def repeatHeadMove(self, headMove):
        '''Execute the given headMove, then repeats it forever.'''
        if (self.headMove != headMove or \
                self.currentState != 'repeatHeadMove'):
            self.headMove = headMove
            self.switchTo('repeatHeadMove')

    # Fixed Pitch
    def repeatBasicPanFixedPitch(self):
        '''Repeat the basic fixed pitch pan.'''
        self.repeatHeadMove(self, HeadMoves.FIXED_PITCH_PAN)

    # Fixed Pitch
    def repeatWidePanFixedPitch(self):
        '''Repeat the wide fixed pitch pan.'''
        self.repeatHeadMove(self, HeadMoves.FIXED_PITCH_PAN_WIDE)

    # Fixed Pitch
    def repeatNarrowPanFixedPitch(self):
        '''Repeat the narrow fixed pitch pan.'''
        self.repeatHeadMove(self, HeadMoves.FIXED_PITCH_PAN_NARROW)

    # Fixed Pitch
    def trackBallFixedPitch(self):
        """
        Enters a state cycle:
        When ball is in view, tracks via vision values.
        Once ball is gone for some time, switch to wide pans.
        Note: can be safely called every frame.
        """
        self.target = self.brain.ball
        self.gain = 1.0
        if (self.currentState is 'fullPanFixedPitch' or \
                self.currentState is 'trackingFixedPitch'):
            self.lastDiffState = 'trackBallFixedPitch'
        else:
            self.switchTo('trackBallFixedPitch')

    # Fixed Pitch
    def spinPanFixedPitch(self):
        """
        Determines which direction the robot is spinning.
        Then, continually looks in that direction.
        """
        if self.brain.nav.isSpinningLeft():
            self.switchTo('lookLeftFixedPitch')
        else:
            self.switchTo('lookRightFixedPitch')

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
