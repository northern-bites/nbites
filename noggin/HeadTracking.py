
from . import TrackingStates
from . import PanningStates
from .util import FSA
import man.motion as motion

#constants need to go in another file.
#maybe auto generated on boot?
TRACKING = 'tracking'

LOC_PANS = 'locPans'

PAN_LEFT_ONCE = 'panLeftOnce'

class HeadTracking(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(TrackingStates)
        self.addStates(PanningStates)

        self.currentState = 'stopped'
        self.setPrintFunction(self.brain.out.printf)
        self.setPrintStateChanges(True)
        self.stateChangeColor = 'yellow'
        self.setName('headTracking')


    def ballTrackingHelper(self,args):
        self.target = None
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
        self.switchTo('stop')

    def stopHeadMoves(self):
        self.stopHead()
        self.brain.motion.stopHeadMoves()

    def trackBall(self):
        self.target = self.brain.ball
        self.gain = 1.0
        if(self.currentState != 'tracking' or self.currentState != 'scanBall'):
           self.switchTo('ballTracking')

    def execute(self,sweetMove):
        for position in sweetMove:
            if len(position) == 6:
                move = motion.BodyJointCommand(position[4], #time
                                               position[0], #larm
                                               position[1], #lleg
                                               position[2], #rleg
                                               position[3], #rarm
                                               position[5], #interpolation type
                                               )
                self.brain.motion.enqueue(move)

            elif len(position) == 3:
                move = motion.HeadJointCommand(position[1],#time
                                               position[0],#head pos
                                               position[2],#interpolation type
                                                   )
                self.brain.motion.enqueue(move)

            else:
                print "What kind of sweet ass-Move is this?"
