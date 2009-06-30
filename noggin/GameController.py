
from man import comm

from . import NogginConstants as Constants
from . import GameStates
from .util import FSA

from . import Leds


TEAM_BLUE = 0
TEAM_RED = 1

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
        self.timeRemaining = self.gc.timeRemaining()
        self.kickOff = self.gc.kickOff
        self.penaltyShots = False
        self.ownKickOff = (self.kickOff == self.brain.my.teamColor)

    def run(self):
        self.setGCLEDS()

        if self.gc.secondaryState == comm.STATE2_PENALTYSHOOT:
            if self.gc.state == comm.STATE_INITIAL:
                self.switchTo('penaltyShotsGameInitial')
            elif self.gc.state == comm.STATE_SET:
                self.switchTo('penaltyShotsGameSet')
            elif self.gc.state == comm.STATE_READY:
                self.switchTo('penaltyShotsGameReady')
            elif self.gc.state == comm.STATE_PLAYING:
                if self.gc.penalty != comm.PENALTY_NONE:
                    self.switchTo('penaltyShotsGamePenalized')
                else:
                    self.switchTo("penaltyShotsGamePlaying")
            elif self.gc.state == comm.STATE_FINISHED:
                self.switchTo('penaltyShotsGameFinished')

        elif self.gc.secondaryState == comm.STATE2_NORMAL:
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
        self.timeRemaining = self.gc.timeRemaining()
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

    def timeRemaining(self):
        return self.timeRemaining()

    def timeSincePlay(self):
        return Constants.LENGTH_OF_HALF - self.timeRemaining

    def getScoreDifferential(self):
        '''
        negative when we're losing
        '''
        return self.brain.gameController.gc.teams(self.brain.my.teamColor)[1] -\
            self.brain.gameController.gc.teams((self.brain.my.teamColor+1)%2)[1]

    def setGCLEDS(self):
        '''
        Method to set the chest and feet according to the current
        GC states and infos
        '''

        #######  KICKOFF  ######
        if (self.gc.kickOff == self.gc.team and
            (self.gc.state == comm.STATE_INITIAL or
             self.gc.state == comm.STATE_READY or
             self.gc.state == comm.STATE_PLAYING)):
            self.brain.leds.executeLeds(Leds.HAVE_KICKOFF_LEDS)
        else:
            self.brain.leds.executeLeds(Leds.NO_KICKOFF_LEDS)

        ###### TEAM COLOR ######
        if self.gc.color == TEAM_BLUE:
            self.brain.leds.executeLeds(Leds.TEAM_BLUE_LEDS)
        else:
            self.brain.leds.executeLeds(Leds.TEAM_RED_LEDS)

        ###### GAME STATE ######

        if self.gc.state == comm.STATE_INITIAL:
            self.brain.leds.executeLeds(Leds.STATE_INITIAL_LEDS)
        elif self.gc.state == comm.STATE_SET:
            self.brain.leds.executeLeds(Leds.STATE_SET_LEDS)
        elif self.gc.state == comm.STATE_READY:
            self.brain.leds.executeLeds(Leds.STATE_READY_LEDS)
        elif self.gc.state == comm.STATE_PLAYING:
            if self.gc.penalty != comm.PENALTY_NONE:
                self.brain.leds.executeLeds(Leds.STATE_PENALIZED_LEDS)
            else:
                self.brain.leds.executeLeds(Leds.STATE_PLAYING_LEDS)
        elif self.gc.state == comm.STATE_FINISHED:
            self.brain.leds.executeLeds(Leds.STATE_FINISHED_LEDS)
