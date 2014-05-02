import time

from . import SoccerFSA
from . import FallControllerStates
from . import GameControllerStates
from . import RoleSwitchingStates
from . import BrunswickStates
from . import ChaseBallStates
from . import PositionStates
from . import PenaltyStates
from . import FindBallStates
from . import KickingStates
from . import PenaltyKickStates
from . import GoaliePositionStates
from . import GoalieSaveStates
from . import DribbleStates
from . import BoxPositionStates

import noggin_constants as NogginConstants

from . import BoxPositionConstants as BPConstants
from ..playbook import PBConstants

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
        self.addStates(PenaltyKickStates)
        self.setName('pBrunswick')
        self.currentState = 'fallController' # initial state

        ### THE STATE OF THE PLAYER ###
        self.inKickingState = False
        #Figure out home & kickoff, even/odd player.
        #All that good stuff...
        if brain.playerNumber == 2:
            self.homePosition = BPConstants.evenDefenderHome
            self.kickoffPosition = self.homePosition
            self.box = BPConstants.evenDefenderBox
            self.isKickingOff = False
        elif brain.playerNumber == 3:
            self.homePosition = BPConstants.oddDefenderHome
            self.kickoffPosition = self.homePosition
            self.box = BPConstants.oddDefenderBox
            self.isKickingOff = False
        elif brain.playerNumber == 4:
            self.homePosition = BPConstants.evenChaserHome
            self.kickoffPosition = BPConstants.theirKickoff
            self.box = BPConstants.chaserBox
            self.isKickingOff = True
        elif brain.playerNumber == 5:
            self.homePosition = BPConstants.cherryPickerHome
            self.kickoffPosition = BPConstants.cherryPickerKickoff
            self.box = BPConstants.cherryPickerBox
            self.isKickingOff = False
        self.frameCounter = 0
        self.shouldRelocalizeCounter = 0
        # Penalty kick player variables
        self.penaltyKicking = False
        # Kickoff kick
        self.shouldKickOff = False
        # Controls whether we check for a falling/fallen robot
        self.runFallController = True 
        # Controls whether we want to dribble it from the corner
        self.corner_dribble = False
        # Controls whether we do a motion kick
        self.motionKick = False
