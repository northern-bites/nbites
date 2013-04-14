from . import SoccerFSA
from . import GaurdianStates

import noggin_constants as NogginConstants

from objects import Location

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self, brain)
        self.addStates(GaurdianStates)
        self.setName('pGaurdian')
