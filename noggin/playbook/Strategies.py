
from . import PBConstants
from . import Formations

def sReady(team):
    return ['sReady'] + Formations.fReady(team)

def sNoFieldPlayers(team):
    return ['sNoFieldPlayers'] + Formations.fNoFieldPlayers(team)

def sOneField(team):
    # Kickoff Formations
    if (team.brain.gameController.timeSincePlay() <
        PBConstants.KICKOFF_FORMATION_TIME):
        return ['sOneField'] + Formations.fOneKickoff(team)

    # Formation for ball in our goal box
    elif team.shouldUseDubD():
        return ['sOneField'] + Formations.fDubD(team)

    elif (PBConstants.USE_FINDER and
          team.brain.ball.timeSinceSeen() >
          PBConstants.FINDER_TIME_THRESH):
          #and team.brain.gameController.getTimeSinceUnpenalized() >
          #PBConstants.FINDER_TIME_THRESH):
        return ['sOneField'] + Formations.fFinder(team)
    return ['sOneField'] + Formations.fOneField(team)

def sTwoField(team):
    '''
    This is our standard strategy.  Based around the 2008.
    '''
    # Game Playing Formations
    if team.brain.gameController.currentState == 'gamePlaying':

        # Kickoff Formations
        if (team.brain.gameController.timeSincePlay() <
            PBConstants.KICKOFF_FORMATION_TIME):
            # Kickoff
            return ['sTwoField'] + Formations.fTwoKickoff(team)

        # Formation for ball in our goal box
        elif team.shouldUseDubD():
            return ['sTwoField'] + Formations.fDubD(team)

        # ball hasn't been seen by me or teammates in a while
        elif (PBConstants.USE_FINDER and
              team.brain.ball.timeSinceSeen() >
              PBConstants.FINDER_TIME_THRESH):
            return ['sTwoField'] + Formations.fFinder(team)

    # Keep a defender and a chaser
    return ['sTwoField'] + Formations.fTwoField(team)

def sThreeField(team):
    '''
    This is our pulled goalie strategy.
    '''
    # Game Playing Formations
    if team.brain.gameController.currentState == 'gamePlaying':
        # Kickoff Formations
        if (team.brain.gameController.timeSincePlay() <
            PBConstants.KICKOFF_FORMATION_TIME):
            #use twokickoff b/c we want the goalie starting in goal
            #like in twoKickoff
            return ['sThreeField'] + Formations.fTwoKickoff(team)

        # Formation for ball in our goal box
        elif team.shouldUseDubD():
            return ['sThreeField'] + Formations.fDubD(team)

        # ball hasn't been seen by me or teammates in a while
        elif (PBConstants.USE_FINDER and team.brain.ball.timeSinceSeen() >
              PBConstants.FINDER_TIME_THRESH):
            return ['sThreeField'] + Formations.fFinder(team)

    # Standard spread formation
    return ['sThreeField'] + Formations.fThreeField(team)

def sTwoZone(team):
    return sTwoField(team)

def sDefensiveMid(team):
    strat = ["sDefensiveMid"]
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
    return ['sTestDefender'] + Formations.fTestDefender(team)
def sTestOffender(team):
    return ['sTestOffender'] + Formations.fTestOffender(team)
def sTestMiddie(team):
    return ['sTestMiddie'] + Formations.fTestMiddie(team)
def sTestChaser(team):
    # Game Ready Setup
    if team.brain.gameController.currentState == 'gameReady' or\
        team.brain.gameController.currentState =='gameSet':
        # team is kicking off
        return ['sTestChaser'] + Formations.fReady(team)

    return ['sTestChaser'] + Formations.fTestChaser(team)
