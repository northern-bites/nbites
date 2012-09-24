from man import comm
import noggin_constants as Constants
from . import GameStates
from .util import FSA

STATE_INITIAL = 0
STATE_SET = 1
STATE_READY = 2
STATE_PLAYING = 3
STATE_FINISHED = 4
PENALTY_NONE = 0
STATE2_PENALTYSHOOT = 1
STATE2_NORMAL = 0
# @TODO: unify these constants!
TEAM_BLUE = Constants.teamColor.TEAM_BLUE
TEAM_RED = Constants.teamColor.TEAM_RED

class GameController(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.gd = brain.comm.gd
        self.addStates(GameStates)
        self.currentState = 'gameInitial'
        self.setName('GameController')
        self.setPrintStateChanges(True)
        self.stateChangeColor = 'green'
        self.setPrintFunction(self.brain.out.printf)
        self.timeLeft = self.gd.timeRemaining
        self.kickOff = self.gd.ourKickoff
        self.penaltyShots = False

        if self.gd.myTeamColor == TEAM_BLUE:
            self.brain.leds.executeLeds(Leds.TEAM_BLUE_LEDS)
        else:
            self.brain.leds.executeLeds(Leds.TEAM_RED_LEDS)

        if self.kickOff:
            self.ownKickOff = True
        else:
            self.ownKickOff = False

        print  "kickoff:%g teamColor:%g" % (self.kickOff, self.gd.myTeamColor)

    def run(self):
        gcState = self.gd.currentState

        if self.gd.secondaryState == STATE2_PENALTYSHOOT:
            if gcState == STATE_INITIAL:
                self.switchTo('penaltyShotsGameInitial')
            elif gcState == STATE_SET:
                self.switchTo('penaltyShotsGameSet')
            elif gcState == STATE_READY:
                self.switchTo('penaltyShotsGameReady')
            elif gcState == STATE_PLAYING:
                if self.gd.isOurPlayerPenalized(self.brain.my.playerNumber):
                    self.switchTo('penaltyShotsGamePenalized')
                else:
                    self.switchTo("penaltyShotsGamePlaying")
            elif gcState == STATE_FINISHED:
                self.switchTo('penaltyShotsGameFinished')
            else:
                self.printf("ERROR: INVALID GAME CONTROLLER STATE")
        elif self.gd.secondaryState == STATE2_NORMAL:
            if gcState == STATE_PLAYING:
                if self.gd.isOurPlayerPenalized(self.brain.my.playerNumber):
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

        self.timeLeft = self.gd.timeRemaining

        #Set team color
        if self.gd.myTeamColor != self.brain.my.teamColor:
            if self.gd.myTeamColor == TEAM_BLUE:
                self.brain.my.teamColor = TEAM_BLUE
            else:
                self.brain.my.teamColor = TEAM_RED

            self.brain.leds.teamChange = True

            self.brain.makeFieldObjectsRelative()
            self.printf("Switching team color to: " +
                        str(self.brain.my.teamColor))

            # need to update kickoff when we swap team color
            if self.gd.ourKickoff:
                self.ownKickOff = True
                self.brain.leds.kickoffChange = True
            else:
                self.ownKickOff = False
                self.brain.leds.kickoffChange = True

        if self.gd.ourKickoff != self.kickOff:
            self.printf("Switching our kickoff to " + str(self.gd.ourKickoff))
            self.kickOff = self.gd.ourKickoff

            if self.kickOff:
                self.ownKickOff = True
            else:
                self.ownKickOff = False
            self.brain.leds.kickoffChange = True

        FSA.FSA.run(self)

    def timeRemaining(self):
        return self.timeLeft

    def timeSincePlay(self):
        return Constants.LENGTH_OF_HALF - self.timeLeft

    def getScoreDifferential(self):
        """
        negative when we're losing
        """
        return self.gd.goalDifferential
