
import man.motion as motion
from . import GoalieTestStates
from . import SoccerFSA
from . import PositionStates
from . import GoaliePositionStates
from . import GoalieSaveStates
from . import GoalieChanges
from . import GoalieTransitions

from .. import NogginConstants
from ..playbook import PBConstants

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GoaliePositionStates)
        self.addStates(GoalieSaveStates)
        self.addStates(PositionStates)

        self.setName('pGoalieTest')
 
        #GOALIE COUNTERS AND BOOLEANS
        # Counters for goalie dive decision making
        self.counterRightSave = 0
        self.counterLeftSave = 0
        self.counterCenterSave = 0

        self.isSaving = False

        #END GOALIE COUNTERS AND BOOLEANS

        self.frameCounter = 0

        self.shouldRelocalizeCounter = 0

        # Orbiting
        self.angleToOrbit = 0.0

        self.inKickingSate = False
        self.penaltyKicking = False
