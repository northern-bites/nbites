
from . import SoccerFSA
from . import MotionStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(MotionStates)
        self.setName('pMotion')
