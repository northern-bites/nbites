from . import SoccerFSA
from . import LocStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(LocStates)
        self.setName('pLoc')
