
import math

from ..util import FSA
from . import StandupStates

class SoccerPlayer(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(StandupStates)
	self.currentState = 'nothing'
        self.setName('Player pStandup')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)
        self.lastState = 'nothing'
        self.uncertainityCounter = 0


    def checkPosition(self):
        inertialX = self.brain.nao.inertial.getAx()
        inertialY = self.brain.nao.inertial.getAy()
        inertialZ = self.brain.nao.inertial.getAz()
        
        
        xYResultant = math.hypot(inertialX,inertialY)
        xYAngle = math.degrees(math.atan2(inertialX,inertialY))
        zResultantAngle = math.degrees(math.atan2(inertialZ,xYResultant))
        print "inertialX =",inertialX," inertialY=",inertialY," inertialZ=", \
            inertialZ," zResultantAngle =",zResultantAngle," xYAngle=",xYAngle
        return (xYAngle,zResultantAngle)
