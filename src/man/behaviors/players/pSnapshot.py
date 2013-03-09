
from . import SoccerFSA
from . import SnapshotStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(SnapshotStates)
        self.setName('pSnapshot')
