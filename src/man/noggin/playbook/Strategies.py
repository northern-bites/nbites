from . import PBConstants
from . import Formations


#### No Field Players ####

def sNoFieldPlayers(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_NO_FIELD_PLAYERS)
    Formations.fNoFieldPlayers(team, workingPlay)


#### One Field Player ####

def sOneField(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_ONE_FIELD_PLAYER)
    # No Kickoff play because it would be identical
    if (team.shouldUseDubD()):
        Formations.fOneFieldD(team, workingPlay)
    else:
        Formations.fOneField(team, workingPlay)

#### Two Field Players ####

def sTwoField(team, workingPlay):
    '''
    More defensive minded two player strategy than zone
    '''
    workingPlay.setStrategy(PBConstants.S_TWO_FIELD_PLAYERS)
    # Post-Kickoff Formations
    if team.useKickoffFormation():
        Formations.fKickoff(team, workingPlay)
    # Double defensive support for ball in our goalbox
    elif team.shouldUseDubD():
        Formations.fTwoDubD(team, workingPlay)
    # Agressive middie support for ball deep in opponents' territory.
    elif team.brain.ball.x > PBConstants.S_MIDDIE_DEFENDER_THRESH:
        Formations.fNeutralTwoField(team, workingPlay)
    # Standard formation
    else:
        Formations.fTwoField(team, workingPlay)

def sTwoZone(team, workingPlay):
    '''
    We attempt to keep one robot forward and one back
    They become chaser if the ball is closer to them
    '''
    workingPlay.setStrategy(PBConstants.S_TWO_ZONE)
    # Post-Kickoff Formations
    if team.useKickoffFormation():
        Formations.fKickoff(team, workingPlay)
    # Ball on offensive side of the field, keep defender
    # @TODO: this can cause chaser to ossilate back and forth. need tie-breaking
    elif team.brain.ball.x > PBConstants.S_TWO_ZONE_DEFENDER_THRESH:
        Formations.fTwoZoneD(team, workingPlay)
    # Ball on defensive side of the field, keep offender
    else:
        Formations.fTwoZoneO(team, workingPlay)


#### Three Field Players ####

def sWin(team, workingPlay):
    '''
    Main Strategy (2011)
    '''
    workingPlay.setStrategy(PBConstants.S_WIN)
    # Post-Kickoff Formations
    if team.useKickoffFormation():
        Formations.fKickoff(team,workingPlay)
    # Double defensive support for ball in our goalbox
    elif team.shouldUseDubD():
        Formations.fThreeDubD(team, workingPlay)
    # Make the defender a middie if the ball is close enough to opp goal
    elif team.brain.ball.x > PBConstants.S_MIDDIE_DEFENDER_THRESH:
        Formations.fNeutralOThreeField(team, workingPlay)
    # Make the offender a middie if the ball is close enough to our goal
    elif team.brain.ball.x < PBConstants.S_MIDDIE_OFFENDER_THRESH:
        Formations.fNeutralDThreeField(team, workingPlay)
    # Standard Formation
    else:
        Formations.fThreeField(team, workingPlay)


#### Four Field Players ####

def sPullGoalie(team, workingPlay):
    '''
    Pull Goalie Strategy (2011)
    '''
    workingPlay.setStrategy(PBConstants.S_PULL_GOALIE)
    # Kickoff Formations
    if team.useKickoffFormation():
        Formations.fKickoff(team, workingPlay)
    # Double defensive support for ball in our goalbox
    elif team.shouldUseDubD():
        Formations.fThreeDubD(team, workingPlay)
    # Standard Formation
    else:
        Formations.fFourField(team, workingPlay)


#### Special Strategies ####

def sReady(team, workingPlay):
    workingPlay.setStrategy(PBConstants.S_READY)
    Formations.fReady(team, workingPlay)


#### Test Strategies ####

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

