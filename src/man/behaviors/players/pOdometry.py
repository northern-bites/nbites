
from . import SoccerFSA
from . import OdometryStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(OdometryStates)
        self.setName('pOdometry')
