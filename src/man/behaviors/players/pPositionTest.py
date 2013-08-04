
from . import SoccerFSA
from . import PositionTestStates
from . import PositionStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self, brain)
        self.addStates(PositionTestStates)
        self.addStates(PositionStates)
        self.setName('pPositionTest')
