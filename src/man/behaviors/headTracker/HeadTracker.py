from . import TrackingStates
from . import BasicStates
from . import HeadTrackingHelper as helper
from . import HeadMoves
from . import TrackingConstants
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
    def performBasicPan(self):
        '''Perform the basic fixed pitch pan once.'''
        self.performHeadMove(HeadMoves.FIXED_PITCH_PAN)

    def repeatBasicPan(self):
        '''Repeat the basic fixed pitch pan.'''
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_PAN)

    def performSnapPan(self):
        self.performHeadMove(HeadMoves.SNAP_PAN)

    def repeatSnapPan(self):

        # print "REPEATING STANDARD SNAP PAN WITH NEW TIMES"

        self.repeatHeadMove(HeadMoves.SNAP_PAN)

    def performWideSnapPan(self):
        self.performHeadMove(HeadMoves.WIDE_SNAP_PAN)

    def repeatWideSnapPan(self):
        self.repeatHeadMove(HeadMoves.WIDE_SNAP_PAN)

    def performFastSnapPan(self):
        self.performHeadMove(HeadMoves.FAST_SNAP_PAN)

    def repeatFastSnapPan(self):
        self.repeatHeadMove(HeadMoves.FAST_SNAP_PAN)

    def performWideFastSnapPan(self):
        self.performHeadMove(HeadMoves.WIDE_FAST_SNAP_PAN)

    def repeatWideFastSnapPan(self):
        self.repeatHeadMove(HeadMoves.WIDE_FAST_SNAP_PAN)

    def performCenterSnapPan(self):
        self.performHeadMove(HeadMoves.CENTER_SNAP_PAN)

    def repeatCenterSnapPan(self):
        self.repeatHeadMove(HeadMoves.CENTER_SNAP_PAN)

    def performGoalieWideSnapPan(self):
        self.performHeadMove(HeadMoves.GOALIE_WIDE_SNAP_PAN)

    def repeatGoalieWideSnapPan(self):
        self.repeatHeadMove(HeadMoves.GOALIE_WIDE_SNAP_PAN)

    def performWidePan(self):
        self.performHeadMove(HeadMoves.FIXED_PITCH_PAN_WIDE)

    def performGameSetWideSnapPan(self):
        self.performHeadMove(HeadMoves.GAME_SET_WIDE_SNAP_PAN)

    def performGameSetInitialWideSnapPan(self):
        self.performHeadMove(HeadMoves.GAME_SET_INITIAL_WIDE_SNAP_PAN)

    def performFixedPitchLookAhead(self):
        self.performHeadMove(HeadMoves.FIXED_PITCH_LOOK_STRAIGHT)

    def repeatFixedPitchLookAhead(self):
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_LOOK_STRAIGHT)

    def repeatWidePan(self):
        """
        Repeat the wide fixed pitch pan.
        Good for localizing.
        """
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_PAN_WIDE)

    def repeatNarrowPan(self):
        '''Repeat the narrow fixed pitch pan.'''
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_PAN_NARROW)

    def repeatFastNarrowPan(self):
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_PAN_NARROW_FAST)

    def performFastTwoIntervalPan(self):
        self.performHeadMove(HeadMoves.FAST_TWO_INTERVAL)

    def repeatFastTwoIntervalPan(self):
        self.repeatHeadMove(HeadMoves.FAST_TWO_INTERVAL)

    # @param invert: false if pan should start to the left,
    #                true if pan should start to the right
    def performKickPan(self, invert = False):
        self.performHeadMove(self.helper.convertKickPan(HeadMoves.FIXED_PITCH_KICK_PAN, invert))

    def trackBall(self, gameSet = False, gamePlaying = False):
        """
        Enters a state cycle:
        When ball is in view, tracks via vision values.
        Once ball is gone for some time, switch to wide pans.
        """

        # Check if we're using bounceTracking
        if TrackingConstants.USE_BOUNCE_TRACKING:
            self.bounceTrackBall()
            return

        self.target = self.brain.ball

        if (gameSet): # == True

            if (self.currentState is not 'gameSetSnapPan' and self.currentState is not 'gameSetTracking'):
                self.switchTo('gameSetTracking')
                return

        if (gamePlaying):

            if (self.currentState is not 'goalieSnapPan' and self.currentState is not 'goalieTracking'):
                self.switchTo('goalieTracking')
                return

        if (self.currentState is not 'gamePlayingSnapPan' and self.currentState is not 'tracking'):
            self.switchTo('tracking') # which is in TrackingStates.py

    def bounceTrackBall(self):
        """
        Same as above, but using the new bounce tracking.
        """
        self.target = self.brain.ball
        if (self.currentState is not 'bounceFullPan' and
            self.currentState is not 'bounceTracking'):
            self.switchTo('bounceTracking')

    def trackFieldObject(self, newTarget):
        self.target = newTarget
        if self.currentState is not 'trackingFieldObject':
            self.switchTo('trackingFieldObject')

    def lookToSpinDirection(self, direction):
        """
        Look to the direction we are spinning.
        """
        if direction < 0:

            print "Look right"

            self.repeatHeadMove(HeadMoves.FIXED_PITCH_LOOK_LEAST_RIGHT)
        else:
            self.repeatHeadMove(HeadMoves.FIXED_PITCH_LOOK_LEAST_LEFT)

    def lookToAngle(self, yaw):
        """
        Look to the given yaw at an appropriate (fixed) pitch.
        """
        self.performHeadMove(self.helper.lookToAngle(yaw))

    def lookToAngleWithTime(self, yaw, time):
        self.performHeadMove(self.helper.lookToAngleWithTime(yaw, time))

    def trackSharedBall(self):
        self.switchTo('trackSharedBall')

    def snapToCorner(self):
        self.switchTo('snapToCorner')

    def lookStraightThenTrack(self):
        """
        Look straight. Once the ball is seen, begin tracking it.
        """
        self.switchTo('lookStraightThenTrack')

    def lookStraight(self):
        self.repeatHeadMove(HeadMoves.FIXED_PITCH_LOOK_STRAIGHT)

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

    def trackObstacle(self, direction):
        """
        Track visual obstacle so we can dodge accordingly
        """
        self.obstacleDir = direction
        self.switchTo('trackObstacle')

    # Not currently used, but would be good functionality to have in the future.
    # TODO: add this functionality back in
    def lookAtTarget(self, target):
        """Look towards given target, using localization values."""
        self.target = target
        self.target.height = 0
        self.switchTo('lookAtTarget')
