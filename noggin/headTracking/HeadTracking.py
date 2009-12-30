from . import TrackingStates
from . import PanningStates
from . import ActiveLookStates
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

        self.currentState = 'stopped'
        self.setPrintFunction(self.brain.out.printf)
        self.setPrintStateChanges(False)
        self.stateChangeColor = 'yellow'
        self.setName('headTracking')

        self.currentHeadScan = None

        self.activePanDir = False
        self.activeLocOn = False
        self.activePanOut = False
        self.activePanUp = False
        self.preActivePanHeads = None
        self.helper = helper.HeadTrackingHelper(self)

        self.lookDirection = None

    def stopHeadMoves(self):
        """stop all head moves. In TrackingStates.py"""
        self.switchTo('stop')

    def setNeutralHead(self):
        """executes sweet move to move head to neutral position.
        Does not call stop head moves. In TrackingStates.py"""
        self.switchTo('neutralHead')

    def trackBall(self):
        """automatically tracks the ball. scans for the ball if not in view"""
        self.target = self.brain.ball
        self.gain = 1.0
        if ( (not self.currentState == 'tracking')
            and (not self.currentState == 'scanBall') ):
            self.switchTo('ballTracking')

    def locPans(self):
        """repeatedly performs quick pan"""
        self.activeLocOn = False
        self.stopHeadMoves()
        self.switchTo('locPans')

    def activeLoc(self):
        """tracks the ball but periodically looks away"""
        self.target = self.brain.ball
        self.gain = 1.0
        if (not self.activeLocOn):
            self.switchTo('activeTracking')

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

    def trackLandmark(self, landmark):
        """automatically tracks landmark, scans for landmark if not in view"""
        self.target = landmark
        self.gain = 1.0
        if (not self.currentState == 'tracking'):
            self.switchTo('landmarkTracking')

    def scanLandmarks(self):
        """looks toward then scans for each landmark in a sequence of landmarks"""
        if (not self.currentState == 'landmarkScan'):
            self.switchTo('landmarkScan')

    def lookToLandmark(self, landmark):
        """looks toward our best guess of landmark position based on loc"""
        self.visGoalX = landmark.x
        self.visGoalY = landmark.y
        self.visGoalHeight = 0
        self.switchTo('lookToPoint')

    def lookToPoint(self, goalX, goalY, goalHeight):
        """looks toward our best guess of position based on loc"""
        self.visGoalX = goalX
        self.visGoalY = goalY
        self.visGoalHeight = goalHeight
        self.switchTo('lookToPoint')
