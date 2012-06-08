
import man.motion as motion
from . import GoalieTestStates
from . import SoccerFSA
from . import PositionStates
from . import GoaliePositionStates
from . import GoalieSaveStates
from . import GoalieChanges
from . import GoalieTransitions

import noggin_constants as  NogginConstants
from ..playbook import PBConstants

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GoaliePositionStates)
        self.addStates(GoalieSaveStates)
        self.addStates(PositionStates)

        self.setName('pGoalieTest')

        self.counterRightSave = 0
        self.counterLeftSave = 0
        self.counterCenterSave = 0

        self.isSaving = False

        self.frameCounter = 0

        self.shouldRelocalizeCounter = 0

        self.shouldOrbit = False

        self.inKickingSate = False
        self.penaltyKicking = False
