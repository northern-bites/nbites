
import man.motion as motion

from . import SoccerFSA
from . import GoalieTestStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GoalieTestStates)
        self.setName('pGoalieTest')

        # Counters for goalie dive decision making
        self.counterRightSave = 0
        self.counterLeftSave = 0
        self.counterCenterSave = 0
        self.shouldSaveCounter = 0

        #Counters for goalie positioning
        self.shouldPositionRightCounter = 0
        self.shouldPositionLeftCounter = 0
        self.shouldPositionCenterCounter = 0

        #Goalie Chasing Counters
        self.shouldChaseCounter = 0
        self.shouldStopChaseCounter = 0

        #Boolean for Goalie Current State
        self.isChasing = False
        self.isSaving = False
        self.isPositioning = False
