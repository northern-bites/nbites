
"""
This file contains an implementation of a finite state automaton.
"""

import time

DEBUG =False

#HELPER method syntax for external state changes requiring helpers
HELPER = "Helper"

# Should I stay? Or should I go?
THIS_FRAME = True
NEXT_FRAME = False

class FSA:
    """ Finite State Automaton implementation.

    Direct subclasses of this class should be made for the create application.
    (i.e. behaviors, motion, etc.).
    
    """
    def __init__(self, owner):
	self.owner = owner
        
	self.currentState = ""
	self.lastFrameState = ""
	self.lastDiffState = ""

        self.name = "FSA"
        self.helperName = "Helper"

	self.counter = 0
	self.startTime = 0
	self.stateTime = 0
	self.states = dict()

        self.getTime = time.time

        #debug switches
        self.printStateChanges = False

    def addStates(self,module):
	# gives a list of all methods and attributes of a module
        if DEBUG: print "Listing states loaded:"
	for candidate in dir(module):
	    attribute = getattr(module,candidate)
	    if callable(attribute):
		if DEBUG: print candidate
		self.states[candidate] = attribute

    def addState(self, name, method):
        if callable(method):
            if DEBUG: print "Additional state loaded:", name
            self.states[name] = method

    def run(self):
	""" Called once every frame by Brain.
	Controls the flow of states for the current frame."""

	stayInFrame = True
	# Switches through states until one relinquishes control of the frame
	# by returning NEXT_FRAME
	while stayInFrame:
	    # grab the method which describes what the current state does
	    methodCall = self.states[self.currentState]
	    # execute the state
            if DEBUG:
                print self.name
                print " DEBUG: current state = ",self.currentState
	    (stayInFrame, nextState) = methodCall(self)
	    self.currentState = nextState
	    self.updateStateInfo()

    def stay(self):
	"""
	Used by states to indicate that the FSA should stay in its
	current state but wait for the next vision frame.
	"""
	return self.goLater(self.currentState)
	
    def goNow(self,newState):
	""" Use this method to switch to a new state immediately. """
	if newState == self.currentState:
	    raise Exception("Calling goNow on the current state causes an "+
			    "infinite loop. Use stay()")
	return (THIS_FRAME, newState)

    def goLater(self,newState):
	"""
	Use this method to switch to a new state and wait for a new vision
	frame.

	As a note: goLater(self.currentState) == stay()
	"""
	self.lastFrameState = self.currentState
	return (NEXT_FRAME, newState)

    def firstFrame(self):
	'''
        Helper method that returns if this is the first frame of the state.
        '''
        return self.counter == 0

    def setTimeFunction(self, newTimeFunction):
        '''
        allows changing the getTime function to something like time.time() 
        '''
        self.timeTime = newTimeFunction
        
    def setPrintFunction(self,newPrintFunction):
        '''
        allows changing the printing to use new function like printf(string)
        '''
        self.printf = newPrintFunction
        
    def setName(self,string):
        self.name = string
        
    def setHelperName(self,string):
        self.helperName = string
        
    def printf(self,str):
        ''' default print method for the FSA '''
        print str

    def updateStateInfo(self):
	"""
	Internal method that will make sure all of the global information
	we store in this class is up to date.
	"""
	# reseting the state counter + state timer when we switch states. 
	if self.currentState != self.lastFrameState:
            #debug prints
            if self.printStateChanges:
                self.printf(self.name+": switched to '"+
                            self.currentState+"\' after " +
                            str(self.counter + 1) +
                            " frames in state \'"+self.lastFrameState+"\'")

	    self.lastDiffState = self.lastFrameState
	    self.counter = 0
	    self.startTime = self.getTime()
	    self.stateTime = 0
	else:
	    self.counter +=1
	    self.stateTime = self.getTime() - self.startTime


    #Debug methods
    def setPrintStateChanges(self, bool):
        ''' Pass True to enable printing of the state name after state switch'''
        self.printStateChanges = bool


    def switchTo(self,newState,*args):
        '''
        Method used to change the state from outside the FSA.

        If your state switch needs arguments, we will call the helper function
        self.stateNameHelper(args). If this function is not initialized,
        you will get an error.
        
        If arguments are passed, we will always reset the counters.
        By default, the coutners are NOT reset if we already in the desired state
        '''
        if len(args) > 0:
            m = getattr(self,newState+self.helperName)
            m(args)
        
        elif self.currentState == newState:
            return

        self.currentState = newState

        self.updateStateInfo()
