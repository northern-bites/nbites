
import util.FSA as FSA

import comm
import NogginConstants as Constants

import noggin.GameStates

class GameController(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.gc = brain.comm.gc
        #jf- self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(noggin.GameStates)
	self.currentState = 'gameInitial'
        self.setName('GameController')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)

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
            print "Switching team color to ",Constants.teamColorDict[self.brain.my.teamColor]
        FSA.FSA.run(self)
        
