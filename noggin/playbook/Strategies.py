
from .. import NogginConstants
from . import PBConstants
from . import Formations

def sNoFieldPlayers(team):
    # Game Ready Setup
    if team.brain.gameController.currentState == 'gameReady' or\
        team.brain.gameController.currentState =='gameSet':
        # team is kicking off
        return ['sNoFieldPlayers'] + Formations.fReady(team)
    return ['sNoFieldPlayers'] + Formations.fNoFieldPlayers(team)

def sOneField(team):
    # Game Ready Setup
    if team.brain.gameController.currentState == 'gameReady' or\
        team.brain.gameController.currentState =='gameSet':
        # team is kicking off
        return ['sOneField'] + Formations.fReady(team)

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
    # Game Ready Setup
    if team.brain.gameController.currentState == 'gameReady' or\
        team.brain.gameController.currentState =='gameSet':
        # team is kicking off
        return ['sTwoField'] + Formations.fReady(team)

    # Game Playing Formations
    elif team.brain.gameController.currentState == 'gamePlaying':

        # Kickoff Formations
        if (team.brain.gameController.timeSincePlay() <
            PBConstants.KICKOFF_FORMATION_TIME):
            # Kickoff play
            if PBConstants.KICKOFF_PLAY:
                return ['sTwoField'] + Formations.fKickoffPlay(team)
            # Kickoff
            else:
                return ['sTwoField'] + Formations.fTwoKickoff(team)

        # Formation for ball in our goal box
        elif team.shouldUseDubD():
            return ['sTwoField'] + Formations.fDubD(team)

        # ball hasn't been seen by me or teammates in a while
        elif (PBConstants.USE_FINDER and
              team.brain.ball.timeSinceSeen() >
              PBConstants.FINDER_TIME_THRESH):
              #and team.brain.gameController.getTimeSinceUnpenalized() >
              #PBConstants.FINDER_TIME_THRESH):
            return ['sTwoField'] + Formations.fFinder(team)
    # Standard spread formation
    return ['sTwoField'] + Formations.fTwoField(team)

def sThreeField(team):
    '''
    This is our standard strategy.  Based around the 2008.
    '''
    # Game Ready Setup
    if team.brain.gameController.currentState == 'gameReady' or\
       team.brain.gameController.currentState =='gameSet':
       # team is kicking off
       return ['sThreeField'] + Formations.fReady(team)

    # Game Playing Formations
    elif team.brain.gameController.currentState == 'gamePlaying':
       # Kickoff Formations
       if (team.brain.gameController.timeSincePlay() <
           PBConstants.KICKOFF_FORMATION_TIME):
           # Kickoff play
           if PBConstants.KICKOFF_PLAY:
               return ['sThreeField'] + Formations.fKickoffPlay(team)
           # Kickoff
           else:
               #use twokickoff b/c we want the goalie starting in goal
               #like in twoKickoff
               return ['sThreeField'] + Formations.fTwoKickoff(team)

       # Formation for ball in our goal box
       elif team.shouldUseDubD():
           return ['sThreeField'] + Formations.fDubD(team)

       # ball hasn't been seen by me or teammates in a while
       elif (PBConstants.USE_FINDER and
             team.brain.ball.timeSinceSeen() >
             PBConstants.FINDER_TIME_THRESH):
             #and team.brain.gameController.getTimeSinceUnpenalized() >
             #PBConstants.FINDER_TIME_THRESH):
           return ['sThreeField'] + Formations.fFinder(team)
    # Standard spread formation
    return ['sThreeField'] + Formations.fThreeField(team)
       
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
