
from man import comm

import noggin_constants as Constants
from . import GameStates
from .util import FSA
from . import Leds

STATE_INITIAL = comm.STATE_INITIAL
STATE_SET = comm.STATE_SET
STATE_READY = comm.STATE_READY
STATE_PLAYING = comm.STATE_PLAYING
STATE_FINISHED = comm.STATE_FINISHED
PENALTY_NONE = comm.PENALTY_NONE
STATE2_PENALTYSHOOT = comm.STATE2_PENALTYSHOOT
STATE2_NORMAL = comm.STATE2_NORMAL
#TODO: unify these constants!
TEAM_BLUE = Constants.teamColor.TEAM_BLUE
TEAM_RED = Constants.teamColor.TEAM_RED

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
        self.timeLeft = self.gc.timeRemaining()
        self.kickOff = self.gc.kickOff
        self.penaltyShots = False

        if self.gc.color == TEAM_BLUE:
            self.brain.leds.executeLeds(Leds.TEAM_BLUE_LEDS)
        else:
            self.brain.leds.executeLeds(Leds.TEAM_RED_LEDS)

        print  "kickoff:%g teamColor:%g" % (self.gc.kickOff, self.gc.color)
        if self.kickOff == self.gc.color:
            self.ownKickOff = True
            self.brain.leds.executeLeds(Leds.HAVE_KICKOFF_LEDS)
        else:
            self.ownKickOff = False
            self.brain.leds.executeLeds(Leds.NO_KICKOFF_LEDS)


    def run(self):
        gcState = self.gc.state

        if self.gc.secondaryState == STATE2_PENALTYSHOOT:
            if gcState == STATE_INITIAL:
                self.switchTo('penaltyShotsGameInitial')
            elif gcState == STATE_SET:
                self.switchTo('penaltyShotsGameSet')
            elif gcState == STATE_READY:
                self.switchTo('penaltyShotsGameReady')
            elif gcState == STATE_PLAYING:
                if self.gc.penalty != PENALTY_NONE:
                    self.switchTo('penaltyShotsGamePenalized')
                else:
                    self.switchTo("penaltyShotsGamePlaying")
            elif gcState == STATE_FINISHED:
                self.switchTo('penaltyShotsGameFinished')
            else:
                self.printf("ERROR: INVALID GAME CONTROLLER STATE")
        elif self.gc.secondaryState == STATE2_NORMAL:
            if gcState == STATE_PLAYING:
                if self.gc.penalty != PENALTY_NONE:
                    self.switchTo("gamePenalized")
                else:
                    self.switchTo("gamePlaying")
            elif gcState == STATE_READY:
                self.switchTo('gameReady')
            elif gcState == STATE_SET:
                self.switchTo('gameSet')
            elif gcState == STATE_INITIAL:
                self.switchTo('gameInitial')
            elif gcState == STATE_FINISHED:
                self.switchTo('gameFinished')
            else:
                self.printf("ERROR: INVALID GAME CONTROLLER STATE")

        self.timeLeft = self.gc.timeRemaining()

        #Set team color
        if self.gc.color != self.brain.my.teamColor:
            if self.gc.color == TEAM_BLUE:
                self.brain.my.teamColor = TEAM_BLUE
            else:
                self.brain.my.teamColor = TEAM_RED
            self.brain.makeFieldObjectsRelative()
            self.printf("Switching team color to: " +
                        str(self.brain.my.teamColor))

            if self.brain.my.teamColor == TEAM_BLUE:
                self.brain.leds.executeLeds(Leds.TEAM_BLUE_LEDS)
            else:
                self.brain.leds.executeLeds(Leds.TEAM_RED_LEDS)

            # need to update kickoff when we swap team color
            if self.kickOff == self.brain.my.teamColor:
                self.ownKickOff = True
                self.brain.leds.executeLeds(Leds.HAVE_KICKOFF_LEDS)
            else:
                self.ownKickOff = False
                self.brain.leds.executeLeds(Leds.NO_KICKOFF_LEDS)


        if self.gc.kickOff != self.kickOff:
            self.printf("Switching kickoff to team color%g"%self.gc.kickOff +
                        " from team color%g"% self.kickOff)
            self.kickOff = self.gc.kickOff

            if self.kickOff == self.brain.my.teamColor:
                self.ownKickOff = True
                self.brain.leds.executeLeds(Leds.HAVE_KICKOFF_LEDS)
            else:
                self.ownKickOff = False
                self.brain.leds.executeLeds(Leds.NO_KICKOFF_LEDS)

        FSA.FSA.run(self)

    def timeRemaining(self):
        return self.timeLeft

    def timeSincePlay(self):
        return Constants.LENGTH_OF_HALF - self.timeLeft

    def getScoreDifferential(self):
        """
        negative when we're losing
        """
        return self.gc.teams(self.brain.my.teamColor)[1] -\
            self.gc.teams((self.brain.my.teamColor+1)%2)[1]
