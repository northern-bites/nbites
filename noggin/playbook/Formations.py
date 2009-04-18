
from . import Roles
from . import SubRoles
from . import PBConstants

def fDefend(team):
    """
    Formation for one missing field player
    """
    # gets teammate that is chaser (could be me)
    chaser_mate = team.determineChaser()
    # if i am chaser
    if chaser_mate.playerNumber == team.brain.my.playerNumber:
        team.me.role = PBConstants.CHASER
        return ['fOneDown'] + Roles.rChaser(team)

    other_teammates = team.getNonChaserTeammates(chaser_mate)
    # get fellow teammate who isn't chaser
    for mate in other_teammates:
        if not mate.inactive:
            otherMate = mate
        else:
            mate.role = PBConstants.OFFENDER

    defInfo = Roles.rDefender(team)
    defPlayer = team.determineSupporter([otherMate], defInfo)

    if defPlayer.playerNumber == team.brain.my.playerNumber:
        return ['fOneDownDefend'] + defInfo
    return ['fOneDownDefend'] + Roles.rMiddie(team)

def fAttack(team):
    """
    Formation for one missing field player
    """
    # gets teammate that is chaser (could be me)
    chaser_mate = team.determineChaser()
    # if i am chaser
    if chaser_mate.playerNumber == team.brain.my.playerNumber:
        team.me.role = PBConstants.CHASER
        return ['fOneDown'] + Roles.rChaser(team)

    other_teammates = team.getNonChaserTeammates(chaser_mate)
    # get fellow teammate who isn't chaser
    for mate in other_teammates:
        if not mate.inactive:
            otherMate = mate
        else:
            mate.role = PBConstants.DEFENDER

    midInfo = Roles.rMiddie(team)
    midPlayer = team.determineSupporter([otherMate], midInfo)

    if midPlayer.playerNumber == team.brain.my.playerNumber:
        return ['fOneDownAttack'] + midInfo
    return ['fOneDownAttack'] + Roles.rOffender(team)
    
def fOneDown(team):
    """
    Formation for one missing field player
    """
    # gets teammate that is chaser (could be me)
    chaser_mate = team.determineChaser()
    # if i am chaser
    if chaser_mate.playerNumber == team.brain.my.playerNumber:
        team.me.role = PBConstants.CHASER
        return ['fOneDown'] + Roles.rChaser(team)
    else:
        team.me.role = PBConstants.DEFENDER
        return ['fOneDown'] + Roles.rDefender(team)

def fSpread(team):
    # gets teammate that is chaser (could be me)
    chaser_mate = team.determineChaser()
    chaser_mate.role = PBConstants.CHASER

    # if i am chaser
    if chaser_mate.playerNumber == team.brain.my.playerNumber:
        return ['fSpread'] + Roles.rChaser(team)
    # Get where the defender should be
    else:
        defInfo = Roles.rDefender(team)
        return ['fSpread'] + defInfo

def fDubD(team):
    # If we're down a player, use different positions
    if team.numInactiveMates == 0:

        # Figure out who isn't penalized with you
        other_teammate = team.getOtherActiveTeammate()

        # Determine if we should have two defenders or a defender 
        # and a middie dependent on score differential
        if team.getGoalDifferential() >= 2:
            pos1 = (PBConstants.SWEEPER_X, PBConstants.SWEEPER_Y)
            currentCenterMidX = team.brain.clip(team.brain.ball.x, 
                                       PBConstants.CENTER_MID_LEFT_LIMIT, 
                                       PBConstants.CENTER_MID_RIGHT_LIMIT)
            pos2 = (currentCenterMidX, 
                    PBConstants.OFFENSIVE_CENTER_MIDFIELD_Y)
        else:
            pos1 = PBConstants.LEFT_DEEP_BACK_POS
            pos2 = PBConstants.RIGHT_DEEP_BACK_POS
            team.me.role = PBConstants.DEFENDER

        # Figure out who should go to which position
        pos = team.getLeastWeightPosition((pos1,pos2), other_teammate)
        if pos == PBConstants.LEFT_DEEP_BACK_POS:
            team.me.role = PBConstants.DEFENDER
            team.me.subRole = PBConstants.LEFT_DEEP_BACK

        elif pos == PBConstants.RIGHT_DEEP_BACK_POS:
            team.me.role = PBConstants.DEFENDER
            team.me.subRole = PBConstants.RIGHT_DEEP_BACK

        else:
            team.me.role = PBConstants.OFFENDER
            team.me.subRole = PBConstants.CENTER_O_MIDFIELD

    # If we are the only player, become the sweeper
    elif team.numInactiveMates == 1:
        pos = (PBConstants.SWEEPER_X, PBConstants.SWEEPER_Y)
        team.me.role = PBConstants.DEFENDER
        team.me.subRole = PBConstants.SWEEPER

    # position setting
    return ['fDubD', team.me.role, team.me.subRole, pos]

def fFinder(team):
    '''no one knows where the ball is'''
    team.me.role = PBConstants.SEARCHER
    return ['fFinder'] + Roles.rSearcher(team)

def fKickoffPlay(team):
    '''time immediately after kickoff'''
    if team.brain.my.playerNumber == 2:
        return ['fKickoffPlay', PBConstants.DEFENDER] + \
            SubRoles.pKickoffPlaySweeper(team)
    elif team.brain.my.playerNumber == 3:
        return ['fKickoffPlay'] + Roles.rChaser(team)

def fKickoff(team):
    '''time immediately after kickoff'''
    if team.me.playerNumber == 2:
        team.me.role = PBConstants.DEFENDER
        return ['fKickoff',PBConstants.DEFENDER] + \
            SubRoles.pKickoffSweeper(team)
    elif team.me.playerNumber == 3:
        team.me.role = PBConstants.CHASER
        return ['fKickoff'] + Roles.rChaser(team)

def fOneKickoff(team):
    """
    kickoff for only having one field player
    """
    team.me.role = PBConstants.CHASER
    return ['fOneKickoff'] + Roles.rChaser(team)

def fReady(team):
    '''kickoff positions'''

    # ready state depends on number of players alive
    inactive_teammates = team.getInactiveFieldPlayers()
    num_inactive_teammates = len(inactive_teammates)

    # if two dogs alive, position normally
    if num_inactive_teammates == 0:
        if team.me.playerNumber == 2:
            return ['fReady', PBConstants.DEFENDER] + \
                SubRoles.pReadyDefender(team)

        elif team.me.playerNumber == 3:
            return ['fReady', PBConstants.CHASER] + \
                SubRoles.pReadyChaser(team)

    # one dogs alive, alter positions a bit
    elif num_inactive_teammates == 1:
        return ['fReady', PBConstants.CHASER] + \
            SubRoles.pReadyChaser(team)

# Formations for testing roles
def fTestDefender(team):
    return ['fTestDefender'] + Roles.rDefender(team)
def fTestOffender(team):
    return ['fTestOffender'] + Roles.rOffender(team)
def fTestMiddie(team):
    return ['fTestMiddie'] + Roles.rMiddie(team)
def fTestChaser(team):
    return ['fTestChaser'] + Roles.rChaser(team)


