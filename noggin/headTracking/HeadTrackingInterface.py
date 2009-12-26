from . import HeadTracking

class HeadTrackingInterface:
    """
    Provides one class that contains all externally accessible headtracking
    control methods
    """
    def __init__(self, brain):
        self.tracker = HeadTracking.HeadTracking(self, brain)

    def stopHeadMoves(self):
        """stop all head moves. In TrackingStates.py"""
        self.tracker.switchTo('stop')

    def trackBall(self):
        """automatically tracks the ball. scans for the ball if not in view"""
        self.tracker.target = self.tracker.brain.ball
        self.tracker.gain = 1.0
        if ( ( not self.tracker.currentState == 'tracking' )
            and ( not self.tracker.currentState == 'scanBall' ) ):
            self.tracker.switchTo('ballTracking')

    def locPans(self):
        """repeatedly performs quick pan"""
        self.tracker.activeLocOn = False
        self.tracker.stopHeadMoves()
        self.tracker.switchTo('locPans')

    def activeLoc(self):
        """tracks the ball but periodically looks away"""
        self.tracker.target = self.tracker.brain.ball
        self.tracker.gain = 1.0
        if( not self.tracker.activeLocOn ):
            self.tracker.switchTo('activeTracking')

    def startScan(self,  newScan):
        """repeatedly performs passed in scan"""
        if newScan != self.tracker.currentHeadScan:
            self.tracker.currentHeadScan = newScan
            self.tracker.switchTo('scanning')

    def lookToDir(self,  direction):
        """performs scripted look: down, up, right, or left"""
        if self.tracker.currentState is not 'look' or \
                self.tracker.lookDirection != direction:
            self.tracker.lookDirection = direction
            self.tracker.switchTo('look')

    def trackLandmark(self, landmark):
        """automatically tracks landmark, scans for landmark if not in view"""
        self.tracker.target = landmark
        self.tracker.gain = 1.0
        if (not self.tracker.currentState == 'tracking'):
            self.tracker.switchTo('landmarkTracking')

    def scanLandmarks(self):
        """looks toward then scans for each landmark in a sequence of landmarks"""
        if (not self.tracker.currentState == 'landmarkScan'):
            self.tracker.switchTo('landmarkScan')

    def lookToLandmark(self,  landmark):
        """looks toward our best guess of landmark position based on loc"""
        self.tracker.lookToPoint(landmark.x, landmark.y, 0)

    def lookToPoint(self,  visGoalX, visGoalY, visGoalHeight):
        """look to an absolute position on the field"""
        lensHeightInCM = self.tracker.getCameraHeight()
        my = self.tracker.brain.my
        relX = visGoalX - my.x
        relY = visGoalY - my.y
        #relH is relative to camera height. negative is normal
        relHeight = visGoalHeight - (lensHeightInCM)
        headMove = self.tracker.Motion.CoordHeadCommand(relX, relY, relHeight, my.h)
        self.tracker.brain.motion.coordHead(headMove)
