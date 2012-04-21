from . import SoccerFSA
from . import WallStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(WallStates)
        self.setName('pWall')
