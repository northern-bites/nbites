
from . import TrackingStates
#from . import PanningStates
from .util import FSA

#constants need to go in another file.
#maybe auto generated on boot?
TRACKING = 'tracking'
NOTHING = 'nothing'

LOC_PANS = 'locPans'

PAN_LEFT_ONCE = 'panLeftOnce'

class HeadTracking(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(TrackingStates)
        #self.addStates(PanningStates)
        
        self.currentState = NOTHING
        self.setPrintFunction(self.brain.out.printf)
        self.setPrintStateChanges(True)
        self.setName('headTracking')


    def trackingHelper(self,args):
        self.object = None
        self.gain = 1.0
        self.angleX = None
        self.angleY = None
        if len(args) == 4:
            (self.target,self.gain,self.angleX,self.angleY) = args
        elif len(args) ==2:
            (self.target,self.gain) = args
        elif len(args) == 1:
            self.target = args[0]
        else:
            raise ValueError("Num params are wrong: besure to"+\
                             " specifiy both angleX and angleY"+str(len(args)))

    def isTracking(self):
        return self.currentState == TRACKING

    def stopHead(self):
        self.switchTo('nothing')

    def trackBall(self):
        self.switchTo('tracking',self.brain.ball)
