import time

from . import SoccerFSA
from . import FallControllerStates
from . import RoleSwitchingStates
from . import GameControllerStates
from . import BrunswickStates
from . import ChaseBallStates
from . import PositionStates
from . import PenaltyStates
from . import FindBallStates
from . import KickingStates
from . import DribbleStates
from . import BoxPositionStates

import noggin_constants as NogginConstants

from . import BoxPositionConstants as BPConstants

from objects import Location

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        ### ADD STATES AND NAME FSA ###
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(FallControllerStates)
        self.addStates(GameControllerStates)
        self.addStates(RoleSwitchingStates)
        self.addStates(BrunswickStates)
        self.addStates(BoxPositionStates)
        self.addStates(PositionStates)
        self.addStates(PenaltyStates)
        self.addStates(ChaseBallStates)
        self.addStates(DribbleStates)
        self.addStates(FindBallStates)
        self.addStates(KickingStates)
        self.setName('pBrunswick')
        self.currentState = 'fallController' # initial state

        ### THE STATE OF THE PLAYER ###
        self.inKickingState = False
        self.role = brain.playerNumber
        # Initialized for the sake of those who aren't
        self.isKickingOff = False
        #Figure out home & kickoff, even/odd player.
        #All that good stuff...
        BPConstants.setRoleConstants(self, self.role)

        self.frameCounter = 0
        self.shouldRelocalizeCounter = 0
        # Penalty kick player variables
        self.penaltyKicking = False
        # Kickoff kick
        self.shouldKickOff = False
        # To keep track of when we are coming out of penalty
        self.wasPenalized = False
        # Controls whether we check for a falling/fallen robot
        self.brain.fallController.enabled = True
        # Controls whether we want to dribble it from the corner
        self.corner_dribble = False
        # Controls whether we do a motion kick
        self.motionKick = False
        # Controls whether we will role switch
        self.roleSwitching = False
        # Controls whether we use claims
        self.useClaims = True
        self.returningFromPenalty = False
