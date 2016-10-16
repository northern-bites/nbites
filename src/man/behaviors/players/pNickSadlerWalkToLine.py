from . import SoccerFSA
from . import walkToLineStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
	def __init__(self, brain):
		SoccerFSA.SoccerFSA.__init__(self, brain)
		self.addStates(walkToLineStates)
		self.setName("pWalkToLine")					
