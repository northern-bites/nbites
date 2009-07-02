
from . import PBConstants
from . import Formations

def sReady(team):
    return [PBConstants.S_READY] + Formations.fReady(team)

def sNoFieldPlayers(team):
    return [PBConstants.S_NO_FIELD_PLAYERS] + Formations.fNoFieldPlayers(team)

def sOneField(team):
    # Kickoff Formations
    if (team.brain.gameController.timeSincePlay() <
        PBConstants.KICKOFF_FORMATION_TIME):
        return [PBConstants.S_ONE_FIELD_PLAYER] + Formations.fOneKickoff(team)

    # Formation for ball in our goal box
    elif team.shouldUseDubD():
        return [PBConstants.S_ONE_FIELD_PLAYER] + Formations.fDubD(team)

    elif (PBConstants.USE_FINDER and
          team.brain.ball.timeSinceSeen() >
          PBConstants.FINDER_TIME_THRESH):
        return [PBConstants.S_ONE_FIELD_PLAYER] + Formations.fFinder(team)

    return [PBConstants.S_ONE_FIELD_PLAYER] + Formations.fOneField(team)

def sTwoField(team):
    '''
    This is our standard strategy.  Based around the 2008.
    '''
    # Kickoff Formations
    if (team.brain.gameController.timeSincePlay() <
        PBConstants.KICKOFF_FORMATION_TIME):
        # Kickoff
        return [PBConstants.S_TWO_FIELD_PLAYERS] + Formations.fTwoKickoff(team)

    # Formation for ball in our goal box
    elif team.shouldUseDubD():
        return [PBConstants.S_TWO_FIELD_PLAYERS] + Formations.fDubD(team)

    # ball hasn't been seen by me or teammates in a while
    elif (PBConstants.USE_FINDER and
          team.brain.ball.timeSinceSeen() >
          PBConstants.FINDER_TIME_THRESH):
        return [PBConstants.S_TWO_FIELD_PLAYERS] + Formations.fFinder(team)

    # Keep a defender and a chaser
    return [PBConstants.S_TWO_FIELD_PLAYERS] + Formations.fTwoField(team)

def sThreeField(team):
    '''
    This is our pulled goalie strategy.
    '''
    # Kickoff Formations
    if (team.brain.gameController.timeSincePlay() <
        PBConstants.KICKOFF_FORMATION_TIME):
        #use twokickoff b/c we want the goalie starting in goal
        #like in twoKickoff
        return [PBConstants.S_THREE_FIELD_PLAYERS] + Formations.fTwoKickoff(team)

    # Formation for ball in our goal box
    elif team.shouldUseDubD():
        return [PBConstants.S_THREE_FIELD_PLAYERS] + Formations.fDubD(team)

    # ball hasn't been seen by me or teammates in a while
    elif (PBConstants.USE_FINDER and team.brain.ball.timeSinceSeen() >
          PBConstants.FINDER_TIME_THRESH):
        return [PBConstants.S_THREE_FIELD_PLAYERS] + Formations.fFinder(team)

    # Standard spread formation
    return [PBConstants.S_THREE_FIELD_PLAYERS] + Formations.fThreeField(team)

def sTwoZone(team):
    """
    We attempt to keep one robot forward and one back
    They become chaser if the ball is closer to them
    """
    return sTwoField(team)

def sWin(team):
    strat = [PBConstants.S_WIN]
    # Kickoff Formations
    if (team.brain.gameController.timeSincePlay() <
        PBConstants.KICKOFF_FORMATION_TIME):
        # Kickoff
        return strat + Formations.fTwoKickoff(team)

    # Formation for ball in our goal box
    elif team.shouldUseDubD():
        return strat + Formations.fDubD(team)

    # ball hasn't been seen by me or teammates in a while
    elif (PBConstants.USE_FINDER and
          team.brain.ball.timeSinceSeen() >
          PBConstants.FINDER_TIME_THRESH):
        return strat + Formations.fFinder(team)

    # Move the defender forward if the ball is close enough to opp goal, then become a middie
    if team.brain.ball.x > PBConstants.S_MIDDIE_DEFENDER_THRESH:
        return strat + Formations.fNeutralDefense(team)
    return strat + Formations.fDefensive(team)

# Add strategies for testing various roles
def sTestDefender(team):
    return [PBConstants.S_TEST_DEFENDER] + Formations.fTestDefender(team)
def sTestOffender(team):
    return [PBConstants.S_TEST_OFFENDER] + Formations.fTestOffender(team)
def sTestMiddie(team):
    return [PBConstants.S_TEST_MIDDIE] + Formations.fTestMiddie(team)
def sTestChaser(team):
    # Game Ready Setup
    if team.brain.gameController.currentState == 'gameReady' or\
        team.brain.gameController.currentState =='gameSet':
        # team is kicking off
        return [PBConstants.S_TEST_CHASER] + Formations.fReady(team)

    return [PBConstants.S_TEST_CHASER] + Formations.fTestChaser(team)
