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
        # Enable safeBallTracking to always keep ball in frame while tracking
        # Currently not supported. Leave false.
        self.safeBallTracking = False
        self.isPreKickScanning = False
        self.preActivePanHeads = None
        self.locObjectList = []
        self.locObjectList.extend(self.brain.myFieldObjects)
        self.locObjectList.extend(self.brain.corners)
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

# ** # new method (main input method)
    def spinFindBall(self):
        """Assumes robot is spinning and looking for ball."""
        """Scans along side robot is spinning to."""
        self.switchTo('spinningPan')

# ** # old method
    def locPans(self):
        """Repeatedly performs quick pan."""
        self.activeLocOn = False
        self.switchTo('locPans')

# ** # old method (main input method)
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
    def startScan(self,  newScan):
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
        """
        Continually bounce head up and down.
        """
        self.switchTo('bounceUp')

    # PRIMARY INPUT METHODS FOR TRACKING FSA:

# ** # new method
    def readyLoc(self):
        """
        Should be called by all robots in 'ready' state.
        The robot will cycle through nearby landmarks, looking at each
        for a short time before moving on to the next. Will function
        properly if the robot is in motion.
        """
        self.target = None
        self.decisionState = 'trackLandmarks'
        self.switchTo('trackLandmarks')

# ** # new method
    def newTrackBall(self):
        """
        Should be called by chaser.
        The robot will switch between looking at the ball and
        periodically looking up for nearby posts.
        """
        self.target = self.brain.ball
        self.decisionState = 'trackingBallLoc'
        self.switchTo('trackingBallLoc')

# ** # new method
    def passiveLoc(self):
        """
        Should be called by all robots in 'set' state.
        The robot will cycle through nearby landmarks, looking at each
        for a short time before moving on to the next. Once localization
        is good, the robot will look towards the center of the field.
        NOTE: Should work if robot is moving, provided they stop before
        becoming well localized. Should use 'readyLoc' state instead.
        """
        self.target = None
        self.decisionState = 'passiveLoc'
        self.switchTo('passiveLoc')

# ** # new method
#    def activeLoc(self):
#        """
#        Should be called by all field players (offender, defender).
#        While localization is poor, the robot will cycle through nearby
#        landmarks, looking at each for a short time. When localization is
#        good enough, the robot will look to the ball for a short time.
#        NOTE: If localization stays good, the robot will stare at the ball.
#        """
#        self.target = None
#        self.decisionState = 'activeLoc'
#        self.switchTo('activeLoc')

# ** # new method
    def stareBall(self):
        """
        Should be called by chaser pre-kick.
        The robot will track the ball and stare at it constantly.
        NOTE: Currently, if ball is not visible at it's estimated
        coordinates, robot will continue to stare there, and will not
        begin scripted pans.
        """
        self.target = self.brain.ball
        self.decisionState = 'trackingBall'
        self.switchTo('trackingBall')

# ** # new method
    def panScan(self):
        """
        The robot will perform full scans until a corner or post is located in
        vision. Then, they will stare at that corner or post until it is lost
        from vision, at which point the robot will resume scanning.
        """
        self.target = None
        self.decisionState = 'panScanForLoc'
        self.switchTo('panScanForLoc')

# ** # new method
    def newKickDecidePan(self):
        """
        The robot should already be in the newTrackBall state cycle. If not already
        scanning for a post, the robot will begin one such scan. Afterwards,
        states will continue in identical manner to newTrackBall.
        """
        if self.currentState != 'scanForPost':
            self.decisionState = 'trackingBallLoc'
            self.switchTo('trackingBallLoc')
