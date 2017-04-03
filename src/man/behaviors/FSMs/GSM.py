from ..Say import *
from . import State

class GameInitial(State.State):
	def onEnter(self, fromState):
		self.brain.leds.setChestLed(0x000000)

	def run(self):
		command = self.brain.interface.bodyMotionCommand
		command.type = command.CommandType.STAND
		command.timestamp = int(self.brain.time * 1000)
		print(self.brain.interface.bodyMotionCommand)

class GameReady(State.State):
	def onEnter(self, fromState):
		self.brain.leds.setChestLed(0x0000FF)

class GameSet(State.State):
	def onEnter(self, fromState):
		self.brain.leds.setChestLed(0xFFFF00)

class GamePlaying(State.State):
	def onEnter(self, fromState):
		self.brain.leds.setChestLed(0x00FF00)

class GameFinished(State.State):
	def onEnter(self, fromState):
		self.brain.leds.setChestLed(0x000000)

State.State.register(GameInitial)

# class GameReady(State):
# 	def onEnter(self, from):
# 		brain.leds.setChestLed(0x0000FF)

# class GameSet(State):
# 	def onEnter(self, from):
# 		brain.leds.setChestLed(0xFFFF00)

# class GamePlaying(State):
# 	def onEnter(self, from):
# 		brain.leds.setChestLed(0x00FF00)

# class GameFinished(State):
# 	def onEnter(self, from):
# 		brain.leds.setChestLed(0x000000)



class GSM(State.Machine):

	def __init__(self, brain):
		super(GSM, self).__init__()
		self.brain = brain
		self.registerState("GameInitial", GameInitial({'brain': self.brain}))
		self.registerState("GameReady", GameReady({'brain': self.brain}))
		self.registerState("GameSet", GameSet({'brain': self.brain}))
		self.registerState("GamePlaying", GamePlaying({'brain': self.brain}))
		self.registerState("GameFinished", GameFinished({'brain': self.brain}))
		# self.switchToState("GameInitial")
		# self.registerState(GameReady)
		# self.registerState(GameSet)
		# self.registerState(GamePlaying)
		# self.registerState(GameFinished)
