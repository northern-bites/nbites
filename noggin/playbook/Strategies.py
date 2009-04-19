
from .. import NogginConstants
from . import PBConstants
from . import Formations

def sSpread(team):
    '''
    This is our standard strategy.  Based around the 2007 with changes for 
    the middie.
    '''
    # Game Ready Setup
    if team.brain.gameController.currentState == 'gameReady':
        # team is kicking off
        return ['sSpread'] + Formations.fReady(team)

    # Game Playing Formations
    elif team.brain.gameController.currentState == 'gamePlaying':

        # Kickoff Formations
        if (team.brain.gameController.getTimeSincePlay() < 
            PBConstants.KICKOFF_FORMATION_TIME):
            # Kickoff play
            if PBConstants.KICKOFF_PLAY:
                return ['sSpread'] + Formations.fKickoffPlay(team)
            # Kickoff
            else:
                return ['sSpread'] + Formations.fKickoff(team)

        # Formation for ball in our goal box
        elif team.shouldUseDubD():
            return ['sSpread'] + Formations.fDubD(team)

        # ball hasn't been seen by me or teammates in a while
        elif (team.brain.ball.timeSinceSeen() > 
              PBConstants.FINDER_TIME_THRESH): 
              #and team.brain.gameController.getTimeSinceUnpenalized() > 
              #PBConstants.FINDER_TIME_THRESH):
            return ['sSpread'] + Formations.fFinder(team)

    # Standard spread formation
    return ['sSpread'] + Formations.fSpread(team)

# Add strategies for testing various roles
def sTestDefender(team):
    return ['sTestDefender'] + Formations.fTestDefender(team)
def sTestOffender(team):
    return ['sTestOffender'] + Formations.fTestOffender(team)
def sTestMiddie(team):
    return ['sTestMiddie'] + Formations.fTestMiddie(team)
def sTestChaser(team):
    return ['sTestChaser'] + Formations.fTestChaser(team)

# Group of strategies for playing shorthanded
def sOneDown(team):
    # Game Ready Setup
    if team.brain.gameController.currentState == 'gameReady':
        # team is kicking off
        return ['sOneDown'] + Formations.fReady(team)

    # Kickoff Formations
    #if (team.brain.gameController.getTimeSincePlay() < 
        #PBConstants.KICKOFF_FORMATION_TIME):
        #return ['sOneDown'] + Formations.fOneKickoff(team)

    # Formation for ball in our goal box
    elif team.shouldUseDubD():
        return ['sOneDown'] + Formations.fDubD(team)

    elif (team.brain.ball.timeSinceSeen() >
          PBConstants.FINDER_TIME_THRESH):
          #and team.brain.gameController.getTimeSinceUnpenalized() > 
          #PBConstants.FINDER_TIME_THRESH):
        return ['sOneDown'] + Formations.fFinder(team)
    return ['sOneDown'] + Formations.fOneDown(team)
