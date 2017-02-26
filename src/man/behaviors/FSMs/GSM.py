from transitions import Machine
from ..Say import *
from pprint import pprint

class GSM(Machine):

	states = ['gameInitial', 'gameReady', 'gameSet', 'gamePlaying', 'gamePenalty', 'gameFinished']

	def __init__(self, brain):
		Machine.__init__(self, states=GSM.states, initial='gameInitial')
	
	def on_enter_gameInitial(self):
		self.brain.leds.setChestLed(0x000000)

	def on_enter_gameReady(self):
		self.brain.leds.setChestLed(0x0000FF)

	def on_enter_gameSet(self):
		self.brain.leds.setChestLed(0xFFFF00)

	def on_enter_gamePlaying(self):
		self.brain.leds.setChestLed(0x00FF00)
