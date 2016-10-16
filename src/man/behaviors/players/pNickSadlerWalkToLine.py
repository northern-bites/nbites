from . import SoccerFSA
from . import WalkToLineStates
import noggin_constants as NogginConstants
from . import FallControllerStates
from . import RoleSwitchingStates
from . import GameControllerStates
from . import CommMonitorStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
	def __init__(self, brain):
		SoccerFSA.SoccerFSA.__init__(self, brain)
		self.addStates(WalkToLineStates)
		self.addStates(FallControllerStates)
		self.addStates(RoleSwitchingStates)
		self.addStates(CommMonitorStates)
		self.addStates(GameControllerStates)	
		self.setName("pWalkToLine")
		
		self.currentState = 'fallController'
		
		self.brain.fallController.enabled = True
		self.roleSwitching = False
		self.role = brain.playerNumber
		self.commMode = -1					
