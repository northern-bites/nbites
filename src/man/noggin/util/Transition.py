import Queue

SOME_OF_THE_TIME = .5
MOST_OF_THE_TIME = .75

LOW_PRECISION = 10
OK_PRECISION = 20

DEBUG = True

def resetTransitions(state):
    """
    resets the transitions of a state
    """
    for targetState, transition in state.transitions.iteritems():
        transition.reset()

def getNextState(fsa, state):
    """
    Method for checking all of the transitions of a state
    and returning the next state
    If any of the transitions are true, then it returns the associate state
    """
    if fsa.firstFrame():
        resetTransitions(state)
    
    #@todo: make it so we can tell the transition whether to use goNow or goLater
    for targetState, transition in state.transitions.iteritems():
        if transition.checkCondition(fsa):
            
            if DEBUG:
                fsa.printf(fsa.name + " switching to " + targetState.__name__ +
                           " from " + state.__name__ + " after " + str(transition))
                
            return fsa.goNow(targetState.__name__) #FSA use states by their names
        
    return fsa.stay()
     

class CountTransition:
    """
    Class that represents a transition to a different state based
    on satisfying a given condition for a certain percent of the frames
    in a window of time
    The higher the value of frameWindow, the better the precision,
    but the worse the delay before the transition occurs 
    """
    
    def __init__(self, condition, threshold = MOST_OF_THE_TIME, frameWindow = OK_PRECISION):
        """
        @param condition: the condition checked for the transition
        @param threshold: (optional) number of frames threshold    
        """
        self.condition = condition
        self.threshold = threshold
        self.frameWindow = frameWindow
        
        self.count = 0
        self.fifo = Queue.Queue()
        
    def checkCondition(self, fsa):
        """
        If the transition's condition was true for a certain number of frames
        in the frameWindow then return true
        """
        if self.condition(fsa):
            self.count += 1
            self.fifo.put(1)
        else:
            self.fifo.put(0)
            
        if self.fifo.qsize() > self.frameWindow:
            self.count -= self.fifo.get()
        
        if self.count > self.threshold * self.frameWindow:
            return True 
        else:
            return False
    
    def __str__(self):
        return (self.condition.__name__ + " happened " + 
                str(self.count) + " out of " + str(self.frameWindow) + " frames")
    
    def reset(self):
        self.fifo = Queue.Queue()
        self.count = 0