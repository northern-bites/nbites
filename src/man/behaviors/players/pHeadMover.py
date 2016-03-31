from . import SoccerFSA
from . import FallControllerStates
from . import RoleSwitchingStates
from . import CommMonitorStates
from . import GameControllerStates
from . import HeadMoverStates


class SoccerPlayer(SoccerFSA.SoccerFSA):
	def __init__(self, brain):
		SoccerFSA.SoccerFSA.__init__(self, brain)
		self.addStates(FallControllerStates)
		self.addStates(RoleSwitchingStates)
		self.addStates(CommMonitorStates)
		self.addStates(GameControllerStates)
		self.addStates(HeadMoverStates)
		self.setName('pHeadMover')
		self.currentState = 'fallController' # initial state

		self.brain.fallController.enabled = False 
		self.roleSwitching = False
		self.role = brain.playerNumber
		self.returningFromPenalty = False
		self.commMode = -1