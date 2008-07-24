
import noggin.NogginConstants as NogginConstants
import PBConstants
import Formations

def sSpread(team):
    '''
    This is our standard strategy.  Based around the 2007 with changes for 
    the middie.
    '''
    # Game Ready Setup
    if team.brain.game.state == NogginConstants.GAME_READY:
        # team is kicking off
        return ['sSpread'] + Formations.fReady(team)

    # Game Playing Formations
    elif team.brain.game.state == NogginConstants.GAME_PLAYING:

        # Kickoff Formations
        if (team.brain.game.getTimeSincePlay() < 
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
        elif (team.brain.timeSinceBallSeen() > 
              PBConstants.FINDER_TIME_THRESH and
              team.brain.game.getTimeSinceUnpenalized() > 
              PBConstants.FINDER_TIME_THRESH):
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
    if team.brain.game.state == NogginConstants.GAME_READY:
        # team is kicking off
        return ['sTwoDown'] + Formations.fReady(team)

    # Kickoff Formations
    if (team.brain.game.getTimeSincePlay() < 
        PBConstants.KICKOFF_FORMATION_TIME):
        return ['sTwoDown'] + Formations.fTwoKickoff(team)

    # Formation for ball in our goal box
    elif team.shouldUseDubD():
        return ['sTwoDown'] + Formations.fDubD(team)

    elif (team.brain.timeSinceBallSeen() > 
          PBConstants.FINDER_TIME_THRESH and
          team.brain.game.getTimeSinceUnpenalized() > 
          PBConstants.FINDER_TIME_THRESH):
        return ['sSpread'] + Formations.fFinder(team)

    return ['sTwoDown'] + Formations.fTwoDown(team)

def sTwoDown(team):
    # Game Ready Setup
    if team.brain.game.state == NogginConstants.GAME_READY:
        # team is kicking off
        return ['sThreeDown'] + Formations.fReady(team)

    # Kickoff Formations
    if (team.brain.game.getTimeSincePlay() < 
        PBConstants.KICKOFF_FORMATION_TIME):
        return ['sThreeDown'] + Formations.fOneKickoff(team)

    # Formation for ball in our goal box
    elif team.shouldUseDubD():
        return ['sThreeDown'] + Formations.fDubD(team)

    elif (team.brain.timeSinceBallSeen() > 
          PBConstants.FINDER_TIME_THRESH and
          team.brain.game.getTimeSinceUnpenalized() > 
          PBConstants.FINDER_TIME_THRESH):
        return ['sSpread'] + Formations.fFinder(team)

    return ['sThreeDown'] + Formations.fThreeDown(team)

