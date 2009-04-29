
from man import comm

from . import NogginConstants as Constants
from . import GameStates
from .util import FSA

class GameController(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.gc = brain.comm.gc
        #jf- self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(GameStates)
        self.currentState = 'gameInitial'
        self.setName('GameController')
        self.setPrintStateChanges(True)
        self.stateChangeColor = 'cyan'
        self.setPrintFunction(self.brain.out.printf)
        self.kickOff = self.gc.kickOff

    def run(self):
        if self.gc.state == comm.STATE_INITIAL:
            self.switchTo('gameInitial')
        elif self.gc.state == comm.STATE_SET:
            self.switchTo('gameSet')
        elif self.gc.state == comm.STATE_READY:
            self.switchTo('gameReady')
        elif self.gc.state == comm.STATE_PLAYING:
            if self.gc.penalty != comm.PENALTY_NONE:
                self.switchTo("gamePenalized")
            else:
                self.switchTo("gamePlaying")
        elif self.gc.state == comm.STATE_FINISHED:
            self.switchTo('gameFinished')


        #Set team color
        if self.gc.color != self.brain.my.teamColor:

            self.brain.my.teamColor = self.gc.color
            self.brain.makeFieldObjectsRelative()
            self.printf("Switching team color to " +
                        Constants.teamColorDict[self.brain.my.teamColor])
        if self.gc.kickOff != self.kickOff:
            self.printf("Switching kickoff to team #%g"%self.gc.kickOff +
                        " from team #%g"% self.kickOff)
            self.kickOff = self.gc.kickOff

        FSA.FSA.run(self)



    def setGCLEDS(self):
        pass
