from . import Roles
from . import SubRoles
from . import PBConstants

def fNoFieldPlayers(team, workingPlay):
    '''when only the goalie is active'''
    workingPlay.setFormation(PBConstants.NO_FIELD_PLAYERS)
    if team.me.isGoalie():
        Roles.rGoalie(team, workingPlay)
    #role, subrole and position are already initialized to INIT

def fOneField(team, workingPlay):
    """Formation for one field player"""
    workingPlay.setFormation(PBConstants.ONE_FIELD)
    if team.me.isGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        Roles.rChaser(team, workingPlay)

def fTwoField(team, workingPlay):
    workingPlay.setFormation(PBConstants.TWO_FIELD)
    if team.me.isGoalie():
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

def fDefensive(team, workingPlay):
    workingPlay.setFormation(PBConstants.DEFENSIVE)
    if team.me.isGoalie():
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

def fNeutralDefense(team, workingPlay):
    """
    Have a defensive midfielder
    """
    workingPlay.setFormation(PBConstants.NEUTRAL_DEFENSE)
    if team.me.isGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        # gets teammate that is chaser (could be me)
        chaser_mate = team.determineChaser()

        # if i am chaser
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        # Get where the middie should be
        else:
            Roles.rDefensiveMiddie(team, workingPlay)

def fNeutralOffense(team, workingPlay):
    """
    Have an offensive midfielder
    """
    workingPlay.setFormation(PBConstants.NEUTRAL_OFFENSE)
    if team.me.isGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        # gets teammate that is chaser (could be me)
        chaser_mate = team.determineChaser()

        # if i am chaser
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        # Get where the middie should be
        else:
            Roles.rOffensiveMiddie(team, workingPlay)

def fOffensive(team, workingPlay):
    """
    Have a supporting attacker
    """
    workingPlay.setFormation(PBConstants.OFFENSIVE)
    if team.me.isGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        # gets teammate that is chaser (could be me)
        chaser_mate = team.determineChaser()

        # if i am chaser
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        # Get where the offender should be
        else:
            Roles.rOffender(team, workingPlay)

def fThreeField(team, workingPlay):
    """
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
    elif team.me.isGoalie():
        Roles.rDefender(team, workingPlay)
    elif chaser_mate.isGoalie():
        if team.me.highestActivePlayerNumber():
        Roles.rOffender(team, workingPlay)
        else:
        Roles.rDefender(team, workingPlay)
    else:
        Roles.rOffender(team, workingPlay)

def fDubD(team, workingPlay):
    workingPlay.setFormation(PBConstants.DUB_D)
    pos = None
    role = None
    subRole = None
    if team.me.isGoalie():
        Roles.rGoalie(team, workingPlay)
    elif team.numActiveFieldPlayers == 2 or team.numActiveFieldPlayers == 3:
        # Figure out who isn't penalized with you
        other_teammate = team.getOtherActiveTeammate()

        # Determine if we should have two defenders or a defender
        # and a middie dependent on score differential
        pos1 = PBConstants.LEFT_DEEP_BACK_POS
        pos2 = PBConstants.RIGHT_DEEP_BACK_POS

        # Figure out who should go to which position
        pos = team.getLeastWeightPosition((pos1,pos2), other_teammate)
        workingPlay.setPosition(pos)
        if pos == PBConstants.LEFT_DEEP_BACK_POS:
            workingPlay.setRole(PBConstants.DEFENDER)
            workingPlay.setSubRole(PBConstants.LEFT_DEEP_BACK)

        elif pos == PBConstants.RIGHT_DEEP_BACK_POS:
            workingPlay.setRole(PBConstants.DEFENDER)
            workingPlay.setSubRole(PBConstants.RIGHT_DEEP_BACK)

        else:
            workingPlay.setRole(PBConstants.OFFENDER)
            workingPlay.setSubRole(PBConstants.DUBD_OFFENDER)

    # If we are the only player, become the sweeper
    else:
        pos = (PBConstants.SWEEPER_X, PBConstants.SWEEPER_Y)
        workingPlay.setPosition(pos)
        workingPlay.setRole(PBConstants.DEFENDER)
        workingPlay.setSubRole(PBConstants.SWEEPER)

def fFinder(team, workingPlay):
    '''no one knows where the ball is'''
    workingPlay.setFormation(PBConstants.FINDER)
    if team.me.isGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        Roles.rSearcher(team, workingPlay)

def fTwoKickoff(team, workingPlay):
    '''time immediately after kickoff'''
    workingPlay.setFormation(PBConstants.TWO_KICKOFF)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    elif team.me.isDefaultDefender():
        Roles.rDefender(team, workingPlay)
    elif team.me.isDefaultChaser():
        Roles.rChaser(team, workingPlay)

def fOneKickoff(team, workingPlay):
    """
    kickoff for only having one field player
    """
    workingPlay.setFormation(PBConstants.ONE_KICKOFF)
    if team.me.isGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        Roles.rChaser(team, workingPlay)

def fReady(team, workingPlay):
    '''kickoff positions'''
    workingPlay.setFormation(PBConstants.READY_FORMATION)
    if team.me.isGoalie():
        Roles.rGoalie(team, workingPlay)

    elif team.numActiveFieldPlayers == 2 or team.numActiveFieldPlayers == 3:
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
        Roles.rDefensiveMiddie(team, workingPlay)
    else:
        Roles.rDefender(team, workingPlay)

def fTestOffender(team, workingPlay):
    workingPlay.setFormation(PBConstants.TEST_OFFEND)
    Roles.rOffender(team, workingPlay)

def fTestChaser(team, workingPlay):
    workingPlay.setFormation(PBConstants.TEST_CHASE)
    Roles.rChaser(team, workingPlay)
