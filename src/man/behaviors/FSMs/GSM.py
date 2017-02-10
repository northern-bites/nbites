from transitions import Machine
from ..Say import *

class GSM(object):

	states = ['gameInitial', 'gameReady', 'gameSet', 'gamePlaying', 'gamePenalty']

	def __init__(self, brain):
		self.brain = brain

		self.machine = Machine(model=self, states=GSM.states, initial='gameInitial')

		self.machine.add_transition(trigger='buttonPress', source='gameInitial', dest='gameReady')
		
		say(Say.IN_DEBUG, "G S M class loaded")