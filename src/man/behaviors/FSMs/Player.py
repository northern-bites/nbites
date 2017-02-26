from transitions import Machine
from ..Say import *

class Player(object):

	gsmStates = ['initial', 'ready', 'set', 'playing', 'finished']

	def __init__(self, brain):
		self.brain = brain
		self.gsmMachine = Machine(states=gsmStates, initial="initial")

		self.playerNumber = 0
		self.role = 0
		self.score = (0, 0)

		self.isPenalized = False

	def run(self):
