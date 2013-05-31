from . import TrackingStates
from . import BasicStates
from . import HeadTrackingHelper as helper
from . import HeadMoves
from ..util import FSA

from .. import StiffnessModes as stiff

class HeadTracker(FSA.FSA):
    """FSA to control actions performed by head"""

    def __init__(self, brain):
        FSA.FSA.__init__(self, brain)
        # Initialize brain and helper, add states
        self.brain = brain
        self.addStates(TrackingStates)
        self.addStates(BasicStates)
        self.helper = helper.HeadTrackingHelper(self)

        # Set debug printing variables
        self.setPrintStateChanges(True)
        self.stateChangeColor = 'yellow'
        self.setName('headTracker')

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


    """Note that all API methods are safe to call every frame."""
    ##################### Basic States API #####################

    def stopHeadMoves(self):
        """Stop all head moves."""
        if (self.currentState is not 'stopped'
            and self.currentState is not 'stop'):
            self.switchTo('stop')

    def isStopped(self):
        """Checks that all head moves have stopped."""
        return self.currentState == 'stopped'

    def setNeutralHead(self):
        """Executes sweet move to move head to neutral position, then stops."""
        self.switchTo('neutralHead')

    def penalizeHeads(self):
        """Sets the head to the penalized position."""
        self.switchTo('penalizeHeads')

    def performHeadMove(self, headMove):
        """Executes the given headMove, then stops."""
        if (headMove != self.headMove
            or self.currentState != 'doHeadMove'):
            self.headMove = headMove
            self.switchTo('doHeadMove')

    def repeatHeadMove(self, headMove):
        '''Executes the given headMove, then repeats it forever.'''
        if (self.headMove != headMove
            or (self.currentState != 'repeatHeadMove' and
                self.currentState != 'repeatingHeadMove')):
            self.headMove = headMove
            self.switchTo('repeatHeadMove')

    ##################### Fixed Pitch #######################

    def repeatBasicPan(self):
        '''Repeat the basic fixed pitch pan.'''
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_PAN)

    def repeatWidePan(self):
        """
        Repeat the wide fixed pitch pan.
        Good for localizing.
        """
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_PAN_WIDE)

    def repeatNarrowPan(self):
        '''Repeat the narrow fixed pitch pan.'''
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_PAN_NARROW)

    def performWidePan(self):
        self.performHeadMove(HeadMoves.FIXED_PITCH_PAN_WIDE)

    # @param invert: false if pan should start to the left,
    #                true if pan should start to the right
    def performKickPan(self, invert = False):
        self.performHeadMove(self.helper.convertKickPan(HeadMoves.FIXED_PITCH_KICK_PAN, invert))

    def trackBall(self):
        """
        Enters a state cycle:
        When ball is in view, tracks via vision values.
        Once ball is gone for some time, switch to wide pans.
        """
        self.target = self.brain.ball
        if (self.currentState is not 'fullPan' and
                self.currentState is not 'tracking'):
            self.switchTo('tracking')

    def trackFieldObject(self, newTarget):
        self.target = newTarget
        if self.currentState is not 'trackingFieldObject':
            self.switchTo('trackingFieldObject')

    def spinPan(self):
        """
        Regardless of which direction we are spinning, look directly ahead.
        This should result in the robot facing the ball when it sees it.
        """
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_LOOK_STRAIGHT)

    def lookToAngle(self, yaw):
        """
        Look to the given yaw at an appropriate (fixed) pitch.
        """
        self.performHeadMove(self.helper.lookToAngle(yaw))

    def lookStraightThenTrack(self):
        """
        Look straight. Once the ball is seen, begin tracking it.
        """
        self.switchTo('lookStraightThenTrack')

    ################### Misc. API #####################

    # TODO: update for current kicks (in constants.KICK_DICT)
    def afterKickScan(self, name):
        """
        After a kick, looks in the appropriate direction
        that the ball was kicked in.
        """
        self.target = self.brain.ball
        self.kickName = name
        self.switchTo('afterKickScan')

    # Not currently used, but would be good functionality to have in the future.
    # TODO: add this functionality back in
    def lookAtTarget(self, target):
        """Look towards given target, using localization values."""
        self.target = target
        self.target.height = 0
        self.switchTo('lookAtTarget')
