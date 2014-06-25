import Queue

OCCASIONALLY = 0.3
SOME_OF_THE_TIME = .5
MOST_OF_THE_TIME = .75
ALL_OF_THE_TIME = 1

INSTANT = 1
LOW_PRECISION = 10
OK_PRECISION = 20
HIGH_PRECISION = 50

DEBUG = True

#######################################
# OLD METHOD OF CHECKING TRANSITIONS #
# Store transitions in state itself  #
######################################
def resetTransitions(state):
    """
    Resets the transitions of a state
    """
    for transition, targetState in state.transitions.iteritems():
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
    for transition, targetState in state.transitions.iteritems():
        if transition.checkCondition(fsa):

            if DEBUG:
                fsa.printf(fsa.name + " switching to " + targetState.__name__ +
                           " from " + state.__name__ + " after " + str(transition))
            targetState.incomingTransition = transition
            targetState.incomingState = state
            return fsa.goNow(targetState.__name__) #FSA use states by their names

    return fsa.stay()

#############################################################
# NEW METHOD OF CHECKING TRANSITIONS                        # 
# Python generators used for describing transitional logic  #
# See the following for more info:                          #
# For info on Python decorators:                            #
# http://stackoverflow.com/questions/739654/how-can-i-make-a-chain-of-function-decorators-in-python/1594484#1594484 
# For info on hierarchical state machines:                  #
# "Statecharts: A Visual Formalism for Complex Systems"     #
#############################################################

### FOR FLAT STATE MACHINES
def ifSwitch(predicate, state, nextFrame=False):
    """
    Function that returns a Python decorator that allows cleaner use of
    transitions in an FSA.
    See the following website for an explanation of how decorators work:
        http://stackoverflow.com/questions/739654/how-can-i-make-a-chain-of-
        function-decorators-in-python/1594484#1594484

    Example usage:
        @ifSwitch(Transitions.ballMoved, chase, True)
        def dribble(player):
            doStuffHere()

    This would transition to the chase state if Transitions.ballMoved(player)
    returned True. It would transition to the chase state in the next frame. If
    ballMoved returned False, then the FSA would stay in dribble.

    @param predicate, truth value returning function, in FSA-speak, a transition
    @param state, the name of a state to switch to AS A STRING
    @param nextFrame, a boolean deciding whether we transition now or next frame

    IMPORTANT:
    (1) If in the state itself a call to goNow or goLater is made, that
        transition will be made without checking the decorated transitions.
    (2) Decorated transitions are checked after the state is run.
    (3) Order of the decorators decides order of the checks. THE FIRST DECORATOR
        IS ACTUALLY THE LAST ONE CHECKED, REVERSE ORDER!
    """
    def decorator(fn):
        def decoratedFunction(fsa):
            newState = fn(fsa)
            if newState:
                return newState

            if predicate(fsa):
                if nextFrame:
                    return fsa.goLater(state)
                else:
                    return fsa.goNow(state)

        decoratedFunction.__name__ = fn.__name__
        return decoratedFunction
    return decorator

def ifSwitchNow(predicate, state):
    """
    Overloaded ifSwitch, see above for complete documentation.

    Like ifSwitch but nextFrame is false (goNow).
    """
    return ifSwitch(predicate, state, False)

def ifSwitchLater(predicate, state):
    """
    Overloaded ifSwitch, see above for complete documentation.

    Like ifSwitch but nextFrame is true (goLater).
    """
    return ifSwitch(predicate, state, True)

def switch(state, nextFrame=False):
    """
    Overloaded ifSwitch, see above for complete documentation.

    Like ifSwitch but predicate is assumed to be True.
    """
    def alwaysTrue(fsa):
        return True

    return ifSwitch(alwaysTrue, state, nextFrame)

def switchNow(state):
    """
    Overloaded switch, see above for complete documentation.

    Like switch but nextFrame is false (goNow).
    """
    return switch(state, False)

def switchLater(state):
    """
    Overloaded switch, see above for complete documentation.

    Like switch but nextFrame is true (goLater).
    """
    return switch(state, True)

def stay(fn):
    """
    Like ifSwitch, see above for complete documentation.

    Return fsa.stay() at the end of function call if nothing else is
    returned. 
    """
    def decoratedFunction(fsa):
        newState = fn(fsa)
        if newState:
            return newState

        return fsa.stay()

    decoratedFunction.__name__ = fn.__name__
    return decoratedFunction

### FOR HIERARCHICAL STATE MACHINES (STATECHARTS)
def superState(state):
    """
    A child state must mark its parent state with this decorator-returning 
    function. This way the children inherit all the conditional logic of the 
    parent.

    See paper titled "Statecharts: A Visual Formalism for Complex Systems" for
    more detailed information.

    @param state, the function representing the parent state AS A STRING

    IMPORTANT: Must be the first decorator applied to the state, before all
    the state-specific conditions and switches.
    """
    def decorator(fn):
        def decoratedFunction(fsa):
            # (1) Child states run before super states run, BUT WE PREFER switching
            #     to states switched to by super states
            stateSwitchedToByChildState = fn(fsa)

            # (2) Super states run AFTER child states, BUT WE PREFER switching
            #     to states switched to by super states
            fsa.ignoreDefaultState = True
            stateSwitchedToBySuperState = fsa.states[state](fsa)
            fsa.ignoreDefaultState = False

            # (3) Switch to state switched to by super first
            if stateSwitchedToBySuperState and stateSwitchedToBySuperState[1] != fsa.currentState:
                return stateSwitchedToBySuperState

            # (4) Switch to state switched to by child
            if stateSwitchedToByChildState:
                return stateSwitchedToByChildState

        decoratedFunction.__name__ = fn.__name__
        return decoratedFunction
    return decorator

def defaultState(state):
    """
    A parent state can have a default state that is switched to automatically 
    if control switches to the parent.

    See paper titled "Statecharts: A Visual Formalism for Complex Systems" for
    more detailed information.

    @param state, the name of the default state AS A STRING

    IMPORTANT: Must be the last decorator applied to the state, after
    all the conditions that the children inherit.
    """
    def decorator(fn):
        def decoratedFunction(fsa):
            if fsa.ignoreDefaultState:
                newState = fn(fsa)
                if newState:
                    return newState
            else:
                return fsa.goNow(state)
        decoratedFunction.__name__ = fn.__name__
        return decoratedFunction
    return decorator

###########################
# STATISTICAL TRANSITIONS #
###########################
class CountTransition:
    """
    Class that represents a transition to a different state based
    on satisfying a given condition for a certain percent of the frames
    in a window of time.

    The higher the value of frameWindow, the better the precision,
    but the worse the delay before the transition occurs.

    Basically a rolling filter on frameWindow frames.
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

        if self.count >= self.threshold * self.frameWindow:
            return True
        else:
            return False

    def __call__(self, fsa):
        return self.checkCondition(fsa)
    
    def __str__(self):
        return (self.condition.__name__ + " happened " +
                str(self.count) + " out of " + str(self.frameWindow) + " frames")

    def reset(self):
        self.fifo = Queue.Queue()
        self.count = 0
