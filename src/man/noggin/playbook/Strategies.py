from .. import NogginConstants
from . import PBConstants
from . import Formations

def sReady(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_READY)
    Formations.fReady(team, workingPlay)

def sNoFieldPlayers(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_NO_FIELD_PLAYERS)
    Formations.fNoFieldPlayers(team, workingPlay)

def sOneField(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_ONE_FIELD_PLAYER)
    # no kickoff formation- would be identical to fOneField
    # Formation for ball in our goal box
    if shouldUseDubD(team):
        Formations.fOneDubD(team, workingPlay)

    elif useFinder(team):
        Formations.fFinder(team, workingPlay)
    else:
        Formations.fOneField(team, workingPlay)

def sTwoField(team, workingPlay):
    '''
    This is our standard strategy.  Based around the 2008.
    '''
    workingPlay.setStrategy(PBConstants.S_TWO_FIELD_PLAYERS)
    # Kickoff Formations
    if useKickoffFormation(team):
        Formations.fKickoff(team, workingPlay)

    # Formation for ball in our goal box
    elif shouldUseDubD(team):
        Formations.fTwoDubD(team, workingPlay)

    # ball hasn't been seen by me or teammates in a while
    elif useFinder(team):
        Formations.fFinder(team, workingPlay)
    else:
        # Keep a defender and a chaser
        Formations.fDefensiveTwoField(team, workingPlay)

def sThreeField(team, workingPlay):
    '''
    This is our pulled goalie strategy.
    '''
    workingPlay.setStrategy(PBConstants.S_THREE_FIELD_PLAYERS)
    # Kickoff Formations
    if useKickoffFormation(team):
        Formations.fKickoff(team, workingPlay)

    # Formation for ball in our goal box
    elif shouldUseDubD(team):
        Formations.fThreeDubD(team, workingPlay)

    # ball hasn't been seen by me or teammates in a while
    elif useFinder(team):
        Formations.fFinder(team, workingPlay)

    # Standard spread formation
    else:
        Formations.fThreeField(team, workingPlay)

def sTwoZone(team, workingPlay):
    """
    We attempt to keep one robot forward and one back
    They become chaser if the ball is closer to them
    """
    sTwoField(team, workingPlay)

def sWin(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_WIN)

    # Kickoff Formations
    if useKickoffFormation(team):
        Formations.fKickoff(team,workingPlay)

    # Formation for ball in our goal box
    elif shouldUseDubD(team):
        Formations.fTwoDubD(team, workingPlay)

    # ball hasn't been seen by me or teammates in a while
    elif useFinder(team):
        Formations.fFinder(team, workingPlay)

    # Move the defender forward if the ball is close enough to opp goal, then become a middie
    elif team.brain.ball.x > PBConstants.S_MIDDIE_DEFENDER_THRESH:
        Formations.fNeutralDefenseTwoField(team, workingPlay)
    else:
        Formations.fDefensiveTwoField(team, workingPlay)

# Add strategies for testing various roles
def sTestDefender(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_TEST_DEFENDER)
    Formations.fTestDefender(team, workingPlay)
def sTestOffender(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_TEST_OFFENDER)
    Formations.fTestOffender(team, workingPlay)
def sTestMiddie(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_TEST_MIDDIE)
    Formations.fTestMiddie(team, workingPlay)
def sTestChaser(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_TEST_CHASER)
    # Game Ready Setup
    if team.brain.gameController.currentState == 'gameReady' or\
        team.brain.gameController.currentState =='gameSet':
        # team is kicking off
        Formations.fReady(team, workingPlay)
    else:
        Formations.fTestChaser(team, workingPlay)

#not sure this is the best place for these yet...
def useKickoffFormation(team):
    if (team.brain.gameController.timeSincePlay() <
        PBConstants.KICKOFF_FORMATION_TIME):
        return True
    else:
        return False

def useFinder(team):
    if (PBConstants.USE_FINDER and
        team.brain.ball.timeSinceSeen() >
        PBConstants.FINDER_TIME_THRESH):
        return True
    else:
        return False

def shouldUseDubD(team):
    if not PBConstants.USE_DUB_D:
        return False
    ballY = team.brain.ball.y
    ballX = team.brain.ball.x
    goalie = team.teammates[0]
    return (
        ( ballY > NogginConstants.MY_GOALBOX_BOTTOM_Y + 5. and
          ballY < NogginConstants.MY_GOALBOX_TOP_Y - 5. and
          ballX < NogginConstants.MY_GOALBOX_RIGHT_X - 5.) or
        ( ballY > NogginConstants.MY_GOALBOX_TOP_Y - 5. and
          ballY < NogginConstants.MY_GOALBOX_BOTTOM_Y + 5. and
          ballX < NogginConstants.MY_GOALBOX_RIGHT_X + 5. and
          goalie.isTeammateRole(PBConstants.CHASER) )
        )

