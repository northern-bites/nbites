#
# A behavior to test SweetMoves.
#

from . import SoccerFSA
from . import SweetStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(SweetStates)
        self.setName('pSweet')
        # No fall protection when testing sweetMoves
        brain.roboguardian.enableFallProtection(False)

