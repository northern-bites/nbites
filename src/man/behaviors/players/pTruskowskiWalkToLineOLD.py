from . import SoccerFSA
from . import truskowskiWalkToLine

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(truskowskiWalkToLine)
        self.setName('pTruskowskiWalkToLine')
