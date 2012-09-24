""" a behavior that tests how the robot approaches the ball, positions on it, and makes its kick decision."""

from . import SoccerFSA
from . import TestKickDecisionStates
from . import ChaseBallStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self, brain)
        self.addStates(TestKickDecisionStates)
        self.addStates(ChaseBallStates)
        self.setName('pTestKickDecision')
