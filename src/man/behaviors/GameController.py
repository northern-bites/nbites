import noggin_constants as Constants

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

# Dictionary for input state:output state
# @inputs are from gameState messages
# @outputs are FSA states
convertStateFormat = { STATE_INITIAL : 'gameInitial',
                       STATE_READY : 'gameReady',
                       STATE_SET : 'gameSet',
                       STATE_PLAYING : 'gamePlaying',
                       STATE_FINISHED : 'gameFinished'
                       }
convertStateFormatPenaltyShots = { STATE_INITIAL : 'penaltyShotsGameSet',
                                   STATE_READY : 'penaltyShotsGameSet',
                                   STATE_SET : 'penaltyShotsGameSet',
                                   STATE_PLAYING : 'penaltyShotsGamePlaying',
                                   STATE_FINISHED : 'gameFinished'
                                   }

class GameController():
    def __init__(self, brain):
        self.brain = brain
        self.currentState = -1
        self.lastState = -1
        self.stateChanged = False
        self.teamColor = TEAM_BLUE
        self.teamColorChanged = False
        self.ownKickOff = False
        self.kickOffChanged = False
        self.penaltyShots = False
        self.penalized = False
        self.penalizedChanged = False

    def run(self):
        gameState = self.brain.interface.gameState

        # reset field for change
        self.stateChanged = False

        if (self.currentState != gameState.state):
            self.stateChanged = True
            self.lastState = self.currentState
            self.currentState = gameState.state

        # reset field for change
        self.teamColorChanged = False

        if (gameState.team(self.teamColor).team_number != self.brain.teamNumber):
            # We have the wrong team color
            self.teamColorChanged = True
            # This function might look weird, but!
            #   0 -> 1 so team blue switches to team red
            #   1 -> 0 so team red  switches to team blue
            self.teamColor = -1*(self.teamColor)+1

        # reset field for change
        self.kickOffChanged = False

        if (gameState.kick_off_team == self.teamColor):
            # It is currently our kick off
            if not self.ownKickOff:
                self.kickOffChanged = True
                self.ownKickOff = True
        else:
            # It is currently not our kick off
            if self.ownKickOff:
                self.kickOffChanged = True
                self.ownKickOff = False

        # Egocentric check for penalty.
        # Note: being penalized is considered a state change, and should
        # override self.currentState when checked by the player FSA.
        if (gameState.team(self.teamColor).player(self.brain.playerNumber-1).penalty):
            # I am penalized.
            if not self.penalized:
                self.stateChanged = True
                self.penalized = True
        else:
            # I am not penalized.
            if self.penalized:
                self.stateChanged = True
                self.penalized = False

        # If state has changed, tell player FSA to switch to new state
        if self.stateChanged:
            if self.penalized:
                self.brain.player.switchTo('gamePenalized')
            else:
                if gameState.secondary_state != STATE2_PENALTYSHOOT:
                    self.brain.player.switchTo(convertStateFormat[self.currentState])
                else:
                    self.brain.player.switchTo(convertStateFormatPenaltyShots[self.currentState])
