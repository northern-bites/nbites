from . import Roles
from . import SubRoles
from . import PBConstants

def fNoFieldPlayers(team, workingPlay):
    '''goalie'''
    workingPlay.setFormation(PBConstants.NO_FIELD_PLAYERS)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)

def fOneField(team, workingPlay):
    '''goalie and chaser'''
    workingPlay.setFormation(PBConstants.ONE_FIELD)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        Roles.rChaser(team, workingPlay)

def fDefensiveTwoField(team, workingPlay):
    '''goalie, chaser and defender'''
    workingPlay.setFormation(PBConstants.DEFENSIVE_TWO_FIELD)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        # gets teammate that is chaser (could be me)
        chaser_mate = team.determineChaser()

        # if i am chaser
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        # Get where the defender should be
        else:
            Roles.rDefender(team, workingPlay)

def fNeutralDefenseTwoField(team, workingPlay):
    """
    goalie, chaser, middie
    """
    workingPlay.setFormation(PBConstants.NEUTRAL_DEFENSE_TWO_FIELD)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        # gets teammate that is chaser (could be me)
        chaser_mate = team.determineChaser()

        # if i am chaser
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        # Get where the middie should be
        else:
            Roles.rMiddie(team, workingPlay)

def fThreeField(team, workingPlay):
    """
    chaser, offender, defender
    right now (2009) we will only have 3 field players if the goalie is
    pulled.
    """
    workingPlay.setFormation(PBConstants.THREE_FIELD)
    # gets teammate that is chaser (could be me)
    chaser_mate = team.determineChaser()
    # if i am chaser
    if chaser_mate.playerNumber == team.brain.my.playerNumber:
        Roles.rChaser(team, workingPlay)
    # Get where the defender should be
    elif team.me.isDefaultGoalie():
        Roles.rDefender(team, workingPlay)
    elif chaser_mate.isDefaultGoalie():
        #b/c all 3 nao's are active, the default chaser has highest player #
        if team.me.isDefaultChaser():
            Roles.rOffender(team, workingPlay)
        else:
            Roles.rDefender(team, workingPlay)
    else:
        Roles.rOffender(team, workingPlay)

def fOneDubD(team, workingPlay):
    '''
    goalie and defender.
    '''
    workingPlay.setFormation(PBConstants.ONE_DUB_D)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    # If we are the only player, become a defender
    else:
        Roles.rDefender(team, workingPlay)

def fTwoDubD(team, workingPlay):
    '''
    goalie, two defenders
    '''
    workingPlay.setFormation(PBConstants.TWO_DUB_D)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        Roles.rDefenderDubD(team, workingPlay)

def fThreeDubD(team, workingPlay):
    """
    because three active field players right now(09) means we have a pulled
    goalie we treat it exactly the same as two field players and force the
    goalie back into the goalie role
    """
    fTwoDubD(team, workingPlay)

def fFinder(team, workingPlay):
    '''no one knows where the ball is'''
    workingPlay.setFormation(PBConstants.FINDER)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        Roles.rSearcher(team, workingPlay)

def fKickoff(team, workingPlay):
    '''time immediately after kickoff.
    chaser, defender, goalie. different than normal because roles are assigned
    entirely be player number'''
    workingPlay.setFormation(PBConstants.KICKOFF)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    elif team.me.isDefaultDefender():
        Roles.rDefender(team, workingPlay)
    elif team.me.isDefaultChaser():
        Roles.rChaser(team, workingPlay)

def fReady(team, workingPlay):
    '''kickoff positions'''
    workingPlay.setFormation(PBConstants.READY_FORMATION)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)

    elif team.numActiveFieldPlayers >= 2:
        if team.me.isDefaultDefender():
            workingPlay.setRole(PBConstants.DEFENDER)
            SubRoles.pReadyDefender(team, workingPlay)
        elif team.me.isDefaultChaser():
            workingPlay.setRole(PBConstants.CHASER)
            SubRoles.pReadyChaser(team, workingPlay)
    else:
        workingPlay.setRole(PBConstants.CHASER)
        SubRoles.pReadyChaser(team, workingPlay)

# Formations for testing roles
def fTestDefender(team, workingPlay):
    workingPlay.setFormation(PBConstants.TEST_DEFEND)
    if team.brain.ball.x > PBConstants.S_MIDDIE_DEFENDER_THRESH:
        Roles.rMiddie(team, workingPlay)
    else:
        Roles.rDefender(team, workingPlay)

def fTestOffender(team, workingPlay):
    workingPlay.setFormation(PBConstants.TEST_OFFEND)
    Roles.rOffender(team, workingPlay)

def fTestChaser(team, workingPlay):
    workingPlay.setFormation(PBConstants.TEST_CHASE)
    Roles.rChaser(team, workingPlay)
