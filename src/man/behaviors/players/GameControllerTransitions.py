import noggin_constants as nogginConstants

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

def gcChangedStateNoPKs(player):
    """
    The game controller changed the state.
    It is not time for PKs.
    """
    gameState = player.brain.interface.gameState
    return (player.gameState != gameState and
            gameState.secondary_state != STATE2_PENALTYSHOOT)

def gcChangedStatePKs(player):
    """
    The game controller changed the state.
    It is time for PKs.
    """
    gameState = player.brain.interface.gameState
    return (player.gameState != gameState and
            gameState.secondary_state == STATE2_PENALTYSHOOT)

def penalized(player):
    """
    We have been penalized.
    """
    gameState = player.brain.interface.gameState
    return (gameState.team(player.brain.teamNumber).
            player(player.brain.playerNumber-1).penalty)
