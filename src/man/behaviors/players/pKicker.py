#
# A behavior to test kicking. States are defined as methods in Kicking States
# Note that it is crucial to implement the appropriate game controller states
# that you wish to override. (By default they do  nothing.)
# By inheriting from the SoccerFSA, we only need to set the name of the player
# and add any additional gourps of states. (GameControlerStates are added in the
# super class.)
#

from . import SoccerFSA
from . import KickerStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(KickerStates)
        self.setName('pKicker')
