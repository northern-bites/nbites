from . import TrackingStates
from . import PanningStates
from . import ActiveLookStates
from . import BasicStates
from . import HeadTrackingHelper as helper
from . import HeadMoves
from ..util import FSA

from .. import StiffnessModes as stiff

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

        # Set sweetmove and scan variables and enums
        self.currentHeadScan = None
        self.headMove = None
        self.lookDirection = None
        self.kickName = ""

        # Set object variables
        self.target = self.brain.ball #default
        # target should either be ball or instance of FieldObject

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

    def penalizeHeads(self):
        """Penalizes the heads."""
        self.switchTo('penalizeHeads')

    def performHeadMove(self, headMove):
        """Executes the given headMove, then stops."""
        if headMove != self.headMove or self.currentState != 'doHeadMove':
            self.headMove = headMove
            # If we were already in the state, reset our counter so that
            #  firstFrame() will be true again.
            self.switchTo('stop')
            self.switchTo('doHeadMove')

    # Note: safe to call every frame.
    def repeatHeadMove(self, headMove):
        '''Execute the given headMove, then repeats it forever.'''
        if (self.headMove != headMove or \
                self.currentState != 'repeatHeadMove'):
            self.headMove = headMove
            self.switchTo('repeatHeadMove')

    ##################### Fixed Pitch #######################

    def repeatBasicPanFixedPitch(self):
        '''Repeat the basic fixed pitch pan.'''
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_PAN)

    def repeatWidePanFixedPitch(self):
        """
        Repeat the wide fixed pitch pan.
        Good for localizing.
        """
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_PAN_WIDE)

    def repeatNarrowPanFixedPitch(self):
        '''Repeat the narrow fixed pitch pan.'''
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_PAN_NARROW)

    def performWidePanFixedPitch(self):
        self.performHeadMove(HeadMoves.FIXED_PITCH_PAN_WIDE)

    def performKickPanFixedPitch(self, invert = False):
        self.performHeadMove(self.helper.convertKickPan(HeadMoves.FIXED_PITCH_KICK_PAN, invert))

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

    def spinPanFixedPitch(self):
        """
        Regardless of which direction we are spinning, look directly ahead.
        This should result in the robot facing the ball when it sees it.
        """
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_LOOK_STRAIGHT)
        #if self.brain.nav.isSpinningLeft():
        #    self.switchTo('lookLeftFixedPitch')
        #else:
        #    self.switchTo('lookRightFixedPitch')

    def lookToAngleFixedPitch(self, yaw):
        """
        Look to the given yaw at an appropriate fixed pitch.
        """
        # HACK
        self.switchTo('stopped')
        self.brain.motion.stopHeadMoves()
        self.helper.lookToAngleFixedPitch(yaw)

    def lookStraightThenTrackFixedPitch(self):
        """
        Look straight. Once the ball is seen, begin tracking it.
        """
        self.switchTo('lookStraightThenTrackFixedPitch')

    ################### End Fixed Pitch #####################

    # Needs adjustments for current kicks.
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

    def lookToTarget(self, target):
        """Look towards given target, using localization values."""
        self.target = target
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

    def lookToAngle(self, angle):
        """
        Look toward a specific angle relative to forward (ie set yaw).
        """
        self.target = 0
        if angle < 57.0 and angle > -57.0:
            self.headMove = (((angle, 17.0), 2.0, 1,
                              stiff.LOW_HEAD_STIFFNESSES), )
        else:
            self.headMove = (((angle, 11.0), 2.0, 1,
                              stiff.LOW_HEAD_STIFFNESSES), )
        self.switchTo('doHeadMove')
