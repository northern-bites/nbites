
import man.motion as motion

from . import SoccerFSA
from . import GoalieTestStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GoalieTestStates)
        self.setName('Player pGoalieTest')

        self.counterCenterSave = 0
        self.counterRightSave = 0
        self.counterLeftSave = 0
