from . import Roles
from . import SubRoles
from . import PBConstants

def fNoFieldPlayers(team):
    '''when only the goalie is active'''
    if team.me.isGoalie():
        return [PBConstants.NO_FIELD_PLAYERS] + Roles.rGoalie(team)
    else:
        return [PBConstants.NO_FIELD_PLAYERS, PBConstants.INIT_ROLE,
            PBConstants.INIT_SUB_ROLE, [0,0] ]

def fOneField(team):
    """Formation for one field player"""
    if team.me.isGoalie():
        return [PBConstants.ONE_FIELD] + Roles.rGoalie(team)

    return [PBConstants.ONE_FIELD] + Roles.rChaser(team)


def fTwoField(team):
    if team.me.isGoalie():
        return [PBConstants.TWO_FIELD] + Roles.rGoalie(team)
    # gets teammate that is chaser (could be me)
    chaser_mate = team.determineChaser()
    chaser_mate.role = PBConstants.CHASER

    # if i am chaser
    if chaser_mate.playerNumber == team.brain.my.playerNumber:
        return [PBConstants.TWO_FIELD] + Roles.rChaser(team)
    # Get where the defender should be
    else:
        defInfo = Roles.rDefender(team)
        return [PBConstants.TWO_FIELD] + defInfo

def fThreeField(team):

    # gets teammate that is chaser (could be me)
    chaser_mate = team.determineChaser()
    chaser_mate.role = PBConstants.CHASER

    # if i am chaser
    if chaser_mate.playerNumber == team.brain.my.playerNumber:
        return [PBConstants.THREE_FIELD] + Roles.rChaser(team)
    # Get where the defender should be
    if team.me.isGoalie():
        return [PBConstants.THREE_FIELD] + Roles.rDefender(team)
    else:
        return [PBConstants.THREE_FIELD] + Roles.rOffender(team)

def fDubD(team):
    if team.me.isGoalie():
        return [PBConstants.DUB_D] + Roles.rGoalie(team)
    if team.numActiveFieldPlayers == 2:

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
    elif team.numActiveFieldPlayers == 1:
        pos = (PBConstants.SWEEPER_X, PBConstants.SWEEPER_Y)
        role = PBConstants.DEFENDER
        subRole = PBConstants.SWEEPER

    # position setting
    return [PBConstants.DUB_D, role, subRole, pos]

def fFinder(team):
    '''no one knows where the ball is'''
    if team.me.isGoalie():
        return [PBConstants.FINDER] + Roles.rGoalie(team)
    #team.me.role = PBConstants.SEARCHER
    return [PBConstants.FINDER] + Roles.rSearcher(team)

def fKickoffPlay(team):
    '''time immediately after kickoff'''
    if team.me.isGoalie():
        return [PBConstants.KICKOFF_PLAY] + Roles.rGoalie(team)
    if team.brain.my.playerNumber == 2:
        return [PBConstants.KICKOFF_PLAY, PBConstants.DEFENDER] + \
            SubRoles.pKickoffPlaySweeper(team)
    elif team.brain.my.playerNumber == PBConstants.DEFAULT_CHASER_NUMBER:
        return [PBConstants.KICKOFF_PLAY] + Roles.rChaser(team)

def fTwoKickoff(team):
    '''time immediately after kickoff'''
    if team.me.isGoalie():
        return [PBConstants.KICKOFF] + Roles.rGoalie(team)
    if team.me.playerNumber == 2:
        #team.me.role = PBConstants.DEFENDER
        return [PBConstants.KICKOFF,PBConstants.DEFENDER] + \
            SubRoles.pKickoffSweeper(team)
    elif team.me.playerNumber == PBConstants.DEFAULT_CHASER_NUMBER:
        #team.me.role = PBConstants.CHASER
        return [PBConstants.KICKOFF] + Roles.rChaser(team)

def fOneKickoff(team):
    """
    kickoff for only having one field player
    """
    if team.me.isGoalie():
        return [PBConstants.ONE_KICKOFF] + Roles.rGoalie(team)
    #team.me.role = PBConstants.CHASER
    return [PBConstants.ONE_KICKOFF] + Roles.rChaser(team)

def fReady(team):
    '''kickoff positions'''
    if team.me.isGoalie():
        return [PBConstants.READY] + Roles.rGoalie(team)

    # if two dogs alive, position normally
    if team.numActiveFieldPlayers == 2:
        if team.me.playerNumber == 2:
            return [PBConstants.READY, PBConstants.DEFENDER] + \
                SubRoles.pReadyDefender(team)

        elif team.me.playerNumber == PBConstants.DEFAULT_CHASER_NUMBER:
            return [PBConstants.READY, PBConstants.CHASER] + \
                SubRoles.pReadyChaser(team)

    # one dogs alive, alter positions a bit
    elif team.numActiveFieldPlayers == 1:
        return [PBConstants.READY, PBConstants.CHASER] + \
            SubRoles.pReadyChaser(team)

# Formations for testing roles
def fTestDefender(team):
    return [PBConstants.TEST_DEFEND] + Roles.rDefender(team)
def fTestOffender(team):
    return [PBConstants.TEST_OFFEND] + Roles.rOffender(team)
def fTestChaser(team):
    return [PBConstants.TEST_CHASE] + Roles.rChaser(team)
