from transitions import Machine
from ..Say import *

class Player(object):

	def __init__(self, brain):
		self.gsmStates = {'initial': self.initial, 'ready': self.ready, 
		   'set': self.set, 'playing': self.playing, 'finished': self.finished}

		self.brain = brain
		self.gsmMachine = Machine(states=[k for k in self.gsmStates], initial="initial")

		self.playerNumber = 0
		self.role = 0
		self.score = (0, 0)

		self.isPenalized = False

	def run(self):
		self.gsmStates[self.gsmMachine.state]()
	
	def initial(self):
		print "in game initial"

	def ready(self):
		print "in game ready"

	def set(self):
		print "in game set"

	def playing(self):
		print "in game playing"

	def finished(self):
		print "in game finished"