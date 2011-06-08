
import man.motion as motion

from . import SoccerFSA
from . import GoalieTestStates
from . import GoalieSaveStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GoalieTestStates)
        self.addStates(GoalieSaveStates)

        self.setName('pGoalieTest')

        # Counters for goalie dive decision making
        self.counterRightSave = 0
        self.counterLeftSave = 0
        self.counterCenterSave = 0
