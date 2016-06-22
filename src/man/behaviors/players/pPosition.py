import time

from . import SoccerFSA
from . import FallControllerStates
from . import RoleSwitchingStates
from . import CommMonitorStates
from . import GameControllerStates
from . import BrunswickStates
from . import ChaseBallStates
from . import PositionStates
from . import PenaltyStates
from . import FindBallStates
from . import KickingStates
from . import PlayOffBallStates
from . import KickOffStates
from . import PenaltyKickingStates

import noggin_constants as NogginConstants

from . import RoleConstants as roleConstants

from objects import Location

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        ### ADD STATES AND NAME FSA ###
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(FallControllerStates)
        self.addStates(GameControllerStates)
        self.addStates(RoleSwitchingStates)
        self.addStates(CommMonitorStates)
        self.addStates(BrunswickStates)
        self.addStates(PlayOffBallStates)
        self.addStates(PositionStates)
        self.addStates(PenaltyStates)
        self.addStates(ChaseBallStates)
        self.addStates(FindBallStates)
        self.addStates(KickingStates)
        self.addStates(KickOffStates)
        self.addStates(PenaltyKickingStates)
        self.setName('pPosition')
        self.currentState = 'fallController' # initial state

        ### THE STATE OF THE PLAYER ###
        self.inKickingState = False
        self.role = brain.playerNumber
        # Initialized for the sake of those who aren't
        self.isKickingOff = False
        # Set home position, box, kickoff vars, etc. based on role number
        roleConstants.setRoleConstants(self, self.role)

        self.frameCounter = 0
        self.shouldRelocalizeCounter = 0
        # Penalty kick player variables
        self.penaltyKicking = False
        # Kickoff kick
        self.shouldKickOff = False
        self.inKickOffPlay = False
        # To keep track of when we are coming out of penalty
        self.wasPenalized = False
        # Controls whether we check for a falling/fallen robot
        self.brain.fallController.enabled = True
        # Controls whether we do a motion kick
        self.motionKick = False
        # if we purposely kicked the ball out
        self.kickedOut = False
        # Controls whether we will role switch
        self.roleSwitching = True
        # Controls whether we use claims
        self.useClaims = False
        self.returningFromPenalty = False

        # Trinary flag indicating state of communications
        # 0 -- all field players are online
        # 1 -- one field player is offline
        # 2 -- more than one field player is offline
        self.commMode = 0


