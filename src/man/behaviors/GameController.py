import noggin_constants as Constants

# TODO unify these constants!
STATE_INITIAL = 0
STATE_READY = 1
STATE_SET = 2
STATE_PLAYING = 3
STATE_FINISHED = 4
PENALTY_NONE = 0
STATE2_PENALTYSHOOT = 1
STATE2_NORMAL = 0
STATE2_OVERTIME = 2
TEAM_BLUE = Constants.teamColor.TEAM_BLUE
TEAM_RED = Constants.teamColor.TEAM_RED

# Dictionary for input state:output state
# @inputs are from gameState messages
# @outputs are FSA states
convertStateFormat = { STATE_INITIAL :  'gameInitial',
                       STATE_READY :    'gameReady',
                       STATE_SET :      'gameSet',
                       STATE_PLAYING :  'gamePlaying',
                       STATE_FINISHED : 'gameFinished'
                       }
convertStateFormatPenaltyShots = { STATE_INITIAL :  'penaltyShotsGameSet',
                                   STATE_READY :    'penaltyShotsGameSet',
                                   STATE_SET :      'penaltyShotsGameSet',
                                   STATE_PLAYING :  'penaltyShotsGamePlaying',
                                   STATE_FINISHED : 'gameFinished'
                                   }

class GameController():
    """
    The GameController's job is to update its own state to reflect the current
    gameState according to self.brain.interface.gameState. It should only 
    report the gameState, not do anything with that information. It is left up
    to the player FSA how to respond, for example see GameControllerStates.py.
    """
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
        self.timeSincePlaying = 0
        self.playingStartTime = 0

    def run(self):
        gameState = self.brain.interface.gameState

        # reset field for change
        self.stateChanged = False

        if (self.currentState != gameState.state):
            self.stateChanged = True
            self.lastState = self.currentState
            self.currentState = gameState.state
            if (self.currentState == STATE_PLAYING):
                self.playingStartTime = self.brain.time
            else:
                self.playingStartTime = 0

        if self.playingStartTime != 0:
            self.timeSincePlaying = self.brain.time - self.playingStartTime
        else:
            self.timeSincePlaying = 0

        # The GC doesn't maintain the same indexing in packets, so we can't
        # rely on always having the same index in the gameState message
        teamIndex = 0
        if gameState.team(1).team_number == self.brain.teamNumber:
            teamIndex = 1
        # reset field for change
        self.teamColorChanged = False

        if gameState.team(teamIndex).team_color != self.teamColor:
            self.teamColor = gameState.team(teamIndex).team_color
            self.teamColorChanged = True # Used by LEDs
            print "Team color changed to: ", self.teamColor

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
        if (gameState.team(teamIndex).player(self.brain.playerNumber-1).penalty):
            # I am penalized.
            if not self.penalized:
                self.stateChanged = True
                self.penalized = True
        else:
            # I am not penalized.
            if self.penalized:
                self.stateChanged = True
                self.penalized = False

        if self.stateChanged:
            if gameState.secondary_state != STATE2_PENALTYSHOOT:
                self.penaltyShots = False
            else:
                self.penaltyShots = True
