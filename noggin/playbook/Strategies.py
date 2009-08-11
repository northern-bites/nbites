
from . import PBConstants
from . import Formations

def sReady(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_READY)
    Formations.fReady(team)

def sNoFieldPlayers(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_NO_FIELD_PLAYERS)
    Formations.fNoFieldPlayers(team)

def sOneField(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_ONE_FIELD_PLAYER)
    # Kickoff Formations
    if useKickoffFormation(team):
        Formations.fOneKickoff(team)

    # Formation for ball in our goal box
    elif team.shouldUseDubD():
        Formations.fDubD(team)

    elif useFinder(team):
        Formations.fFinder(team)
    else:
        Formations.fOneField(team)

def sTwoField(team, workingPlay):
    '''
    This is our standard strategy.  Based around the 2008.
    '''
    workingPlay.setStrategy(PBConstants.S_TWO_FIELD_PLAYERS)
    # Kickoff Formations
    if useKickoffFormation(team):
        Formations.fTwoKickoff(team)

    # Formation for ball in our goal box
    elif team.shouldUseDubD():
        Formations.fDubD(team)

    # ball hasn't been seen by me or teammates in a while
    elif useFinder(team):
        Formations.fFinder(team)
    else:
        # Keep a defender and a chaser
        Formations.fTwoField(team)

def sThreeField(team, workingPlay):
    '''
    This is our pulled goalie strategy.
    '''
    workingPlay.setStrategy(PBConstants.S_THREE_FIELD_PLAYERS)
    # Kickoff Formations
    if useKickoffFormation(team):
        #use twokickoff b/c we want the goalie starting in goal
        #like in twoKickoff
        Formations.fTwoKickoff(team)

    # Formation for ball in our goal box
    elif team.shouldUseDubD():
        Formations.fDubD(team)

    # ball hasn't been seen by me or teammates in a while
    elif useFinder(team):
        Formations.fFinder(team)
    else:
        # Standard spread formation
        Formations.fThreeField(team)

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
        # Kickoff
        Formations.fTwoKickoff(team)

    # Formation for ball in our goal box
    elif team.shouldUseDubD():
        Formations.fDubD(team)

    # ball hasn't been seen by me or teammates in a while
    elif useFinder(team):
        Formations.fFinder(team)

    # Move the defender forward if the ball is close enough to opp goal, then become a middie
    if team.brain.ball.x > PBConstants.S_MIDDIE_DEFENDER_THRESH:
        Formations.fNeutralDefense(team)
    else:
        Formations.fDefensive(team)

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
