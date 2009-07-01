from . import Roles
from . import SubRoles
from . import PBConstants

def fNoFieldPlayers(team):
    '''when only the goalie is active'''
    if team.me.isGoalie():
        return [PBConstants.NO_FIELD_PLAYERS] + Roles.rGoalie(team)

    return [PBConstants.NO_FIELD_PLAYERS, PBConstants.INIT_ROLE,
            PBConstants.INIT_SUB_ROLE, [0,0,0] ]

def fOneField(team):
    """Formation for one field player"""
    role = None
    if team.me.isGoalie():
        role = Roles.rGoalie(team)
    else:
        role = Roles.rChaser(team)
    return [PBConstants.ONE_FIELD] + role

def fTwoField(team):
    role = None
    if team.me.isGoalie():
        role = Roles.rGoalie(team)
    else:
        # gets teammate that is chaser (could be me)
        chaser_mate = team.determineChaser()

        # if i am chaser
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            role = Roles.rChaser(team)
        # Get where the defender should be
        else:
            role = Roles.rDefender(team)
    return [PBConstants.TWO_FIELD] + role

def fDefensive(team):
    role = None
    if team.me.isGoalie():
        role = Roles.rGoalie(team)
    else:
        # gets teammate that is chaser (could be me)
        chaser_mate = team.determineChaser()

        # if i am chaser
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            role = Roles.rChaser(team)
        # Get where the defender should be
        else:
            role = Roles.rDefender(team)
    return [PBConstants.DEFENSIVE] + role

def fNeutralDefense(team):
    """
    Have a defensive midfielder
    """
    role = None
    if team.me.isGoalie():
        role = Roles.rGoalie(team)
    else:
        # gets teammate that is chaser (could be me)
        chaser_mate = team.determineChaser()

        # if i am chaser
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            role = Roles.rChaser(team)
        # Get where the middie should be
        else:
            role = Roles.rDefensiveMiddie(team)
    return [PBConstants.NEUTRAL_DEFENSE] + role

def fNeutralOffense(team):
    """
    Have an offensive midfielder
    """
    role = None
    if team.me.isGoalie():
        role = Roles.rGoalie(team)
    else:
        # gets teammate that is chaser (could be me)
        chaser_mate = team.determineChaser()

        # if i am chaser
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            role = Roles.rChaser(team)
        # Get where the middie should be
        else:
            role = Roles.rOffensiveMiddie(team)
    return [PBConstants.NEUTRAL_OFFENSE] + role

def fOffensive(team):
    """
    Have a supporting attacker
    """
    role = None
    if team.me.isGoalie():
        role = Roles.rGoalie(team)
    else:
        # gets teammate that is chaser (could be me)
        chaser_mate = team.determineChaser()

        # if i am chaser
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            role = Roles.rChaser(team)
        # Get where the offender should be
        else:
            role = Roles.rOffender(team)
    return [PBConstants.OFFENSIVE] + role

def fThreeField(team):
    """
    right now (2009) we will only have 3 field players if the goalie is
	pulled.
    """
    role = None
    # gets teammate that is chaser (could be me)
    chaser_mate = team.determineChaser()
    # if i am chaser
    if chaser_mate.playerNumber == team.brain.my.playerNumber:
        role = Roles.rChaser(team)
    # Get where the defender should be
    elif team.me.isGoalie():
        role = Roles.rDefender(team)
    elif chaser_mate.isGoalie():
        if team.me.highestActivePlayerNumber():
            role = Roles.rOffender(team)
        else:
            role = Roles.rDefender(team)
    else:
        role = Roles.rOffender(team)
    return [PBConstants.THREE_FIELD] + role

def fDubD(team):
    pos = None
    role = None
    subRole = None
    if team.me.isGoalie():
        role, subRole, pos = Roles.rGoalie(team)
    elif team.numActiveFieldPlayers == 2 or team.numActiveFieldPlayers == 3:
        # Figure out who isn't penalized with you
        other_teammate = team.getOtherActiveTeammate()

        # Determine if we should have two defenders or a defender
        # and a middie dependent on score differential
        pos1 = PBConstants.LEFT_DEEP_BACK_POS
        pos2 = PBConstants.RIGHT_DEEP_BACK_POS
        role = PBConstants.DEFENDER

	    # Figure out who should go to which position
        pos = team.getLeastWeightPosition((pos1,pos2), other_teammate)
        if pos == PBConstants.LEFT_DEEP_BACK_POS:
            role = PBConstants.DEFENDER
            subRole = PBConstants.LEFT_DEEP_BACK

        elif pos == PBConstants.RIGHT_DEEP_BACK_POS:
            role = PBConstants.DEFENDER
            subRole = PBConstants.RIGHT_DEEP_BACK
        else:
            role = PBConstants.OFFENDER
            subRole = PBConstants.DUBD_OFFENDER

    # If we are the only player, become the sweeper
    else:
        pos = (PBConstants.SWEEPER_X, PBConstants.SWEEPER_Y)
        role = PBConstants.DEFENDER
        subRole = PBConstants.SWEEPER

    # position setting
    return [PBConstants.DUB_D, role, subRole, pos]

def fFinder(team):
    '''no one knows where the ball is'''
    role = None
    if team.me.isGoalie():
        role = Roles.rGoalie(team)
    else:
        role = Roles.rSearcher(team)
    return [PBConstants.FINDER] + role

def fTwoKickoff(team):
    '''time immediately after kickoff'''
    role = None
    if team.me.isGoalie():
        role = Roles.rGoalie(team)
    elif team.me.playerNumber == 2:
        role = Roles.rDefender(team)
    elif team.me.playerNumber == PBConstants.DEFAULT_CHASER_NUMBER:
        role = Roles.rChaser(team)
    return [PBConstants.TWO_KICKOFF] + role

def fOneKickoff(team):
    """
    kickoff for only having one field player
    """
    role = None
    if team.me.isGoalie():
        role = Roles.rGoalie(team)
    else:
        role = Roles.rChaser(team)
    return [PBConstants.ONE_KICKOFF] + role

def fReady(team):
    '''kickoff positions'''
    role = None
    if team.me.isGoalie():
        role = Roles.rGoalie(team)

    elif team.numActiveFieldPlayers == 2 or team.numActiveFieldPlayers == 3:
        if team.me.playerNumber == 2:
            role =  [PBConstants.DEFENDER] + SubRoles.pReadyDefender(team)
        elif team.me.playerNumber == PBConstants.DEFAULT_CHASER_NUMBER:
            role = [PBConstants.CHASER] + SubRoles.pReadyChaser(team)
    else:
        role = [PBConstants.CHASER] + SubRoles.pReadyChaser(team)
    return [PBConstants.READY_FORMATION] + role

# Formations for testing roles
def fTestDefender(team):
    if team.brain.ball.x > PBConstants.S_MIDDIE_DEFENDER_THRESH:
        return  [PBConstants.TEST_DEFEND] + Roles.rDefensiveMiddie(team)
    return [PBConstants.TEST_DEFEND] + Roles.rDefender(team)
def fTestOffender(team):
    return [PBConstants.TEST_OFFEND] + Roles.rOffender(team)
def fTestChaser(team):
    return [PBConstants.TEST_CHASE] + Roles.rChaser(team)
