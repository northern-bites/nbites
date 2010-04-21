""" a behavoir that tests the lookTo of headTracking by using points strictly relative to the robots posiiton and shouldn't be screwed up by bad localization"""

from . import SoccerFSA
from . import TestLookToStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self, brain)
        self.addStates(TestLookToStates)
        self.setName('pTestLookTo')
