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
STATE2_OVERTIME = 2
# @TODO: unify these constants!
TEAM_BLUE = Constants.teamColor.TEAM_BLUE
TEAM_RED = Constants.teamColor.TEAM_RED

class GameController(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(GameStates)
        self.currentState = 'gameInitial'
        self.setName('GameController')
        self.setPrintStateChanges(True)
        self.stateChangeColor = 'green'
        self.setPrintFunction(self.brain.out.printf)
        self.timeLeft = 600 #temporary value
        self.ownKickOff = True
        self.penaltyShots = False

        print  "kickoff:%g teamColor:%g" % (self.ownKickOff, self.gd.myTeamColor)

    def run(self):
        # Currently set up to ignore button presses if game
        # controller is sending packets 3/6/2013

        self.gd = self.brain.inMessages['gameState']

        gcState = self.gd.state
        if self.gd.team[0].team_number == self.brain.teamNumber:
            penalized = self.gd.team[0].player[self.brain.playerNumber-1].penalty
        else:
            penalized = self.gd.team[1].player[self.brain.playerNumber-1].penalty

        if self.gd.secondary_state == STATE2_PENALTYSHOOT:
            if gcState == STATE_INITIAL:
                self.switchTo('penaltyShotsGameInitial')
            elif gcState == STATE_SET:
                self.switchTo('penaltyShotsGameSet')
            elif gcState == STATE_READY:
                self.switchTo('penaltyShotsGameReady')
            elif gcState == STATE_PLAYING:
                if penalized:
                    self.switchTo('penaltyShotsGamePenalized')
                else:
                    self.switchTo("penaltyShotsGamePlaying")
            elif gcState == STATE_FINISHED:
                self.switchTo('penaltyShotsGameFinished')
            else:
                self.printf("ERROR: INVALID GAME CONTROLLER STATE")
        elif self.gd.secondary_state == STATE2_NORMAL:
            if gcState == STATE_PLAYING:
                if penalized:
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

        self.timeLeft = self.gd.secs_remaining

        #Set team color
        for TeamInfo teamInfo in self.gd.team:
            if teamInfo.team_number == self.brain.teamNumber:
                if teamInfo.team_color != self.brain.teamColor:
                    if teamInfo.team_color == TEAM_BLUE:
                        self.brain.teamColor = TEAM_BLUE
                    else:
                        self.brain.teamColor = TEAM_RED

                    self.brain.leds.teamChange = True

            # need to update kickoff when we swap team color
        if self.gd.kick_off_team == self.brain.teamColor:
            if self.ownKickOff == False:
                self.ownKickOff = True
                self.brain.leds.kickoffChange = True
        else:
            if self.ownKickOff == True:
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
        return 0
