from abc import ABCMeta, abstractmethod

class Machine(object):

	def __init__(self):
		self.currentState = None
		self.currentStateName = ""
		self.lastState = None
		self.stateCounter = 0
		self.allStates = {}

	def registerState(self, name, state):
		self.allStates[name] = state

	# def deregisterState(self, state):
		# @TODO

	def switchToState(self, name):
		oldstate = None

		if self.currentState:
			self.currentState.onExit(self.allStates[name])
			oldstate = self.currentState
		self.currentState = self.allStates[name]
		self.currentStateName = name
		self.currentState.onEnter(oldstate)

	def run(self):
		self.stateCounter += 1
		if self.currentState:
			self.currentState.run()


class State(object):
	__metaclass__ = ABCMeta

	def __init__(self, data):
		for key in data:
			setattr(self, key, data[key])

	@abstractmethod
	def onEnter(self, fromState):
		pass

	def run(self):
		pass

	def onExit(self, toState):
		pass
