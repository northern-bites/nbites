""" a behavior that tests ball scanning. Used to test if robot will turn in correct direction to follow last seen ball movement."""

from . import SoccerFSA
from . import TestScanBallStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self, brain)
        self.addStates(TestScanBallStates)
        self.setName('pTestScanBall')
