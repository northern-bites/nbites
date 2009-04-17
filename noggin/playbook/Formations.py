
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
        other_teammates = team.getNonChaserTeammates(chaser_mate)
        # If two robots are inactive, we become defender
        team.me.role = PBConstants.DEFENDER
        other_teammates[0].role = PBConstants.OFFENDER
        other_teammates[1].role = PBConstants.MIDDIE
        return ['fOneDown'] + Roles.rDefender(team)

def fTwoDown(team):
    """
    Formation for missing two robots
    """
    return ['fTwoDown'] + Roles.rChaser(team)

def fSpread(team):
    # gets teammate that is chaser (could be me)
    chaser_mate = team.determineChaser()
    chaser_mate.role = PBConstants.CHASER

    # if i am chaser
    if chaser_mate.playerNumber == team.brain.my.playerNumber:
        return ['fSpread'] + Roles.rChaser(team)

    # We now figure out the roles of non-chasers
    posPlayers = team.getNonChaserTeammates(chaser_mate)

    # Get where the defender should be
    defInfo = Roles.rDefender(team)
    defPlayer = team.determineSupporter(posPlayers, defInfo)

    if defPlayer.playerNumber == team.brain.my.playerNumber:
        return ['fSpread'] + defInfo

    return ['fSpread'] + Roles.rOffender(team)

def fDubD(team):
    # If we're down a player, use different positions
    if team.numInactiveMates == 2:

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
    elif team.numInactiveMates == 3:
        pos = (PBConstants.SWEEPER_X, PBConstants.SWEEPER_Y)
        team.me.role = PBConstants.DEFENDER
        team.me.subRole = PBConstants.SWEEPER

    # We have three dogs, we can have a middie and two deep backs
    elif team.numInactiveMates == 1:

        # Our playerNumber - 1 usually returns the correct player
        offset = 1
        # If an inactive mate has a lower playerNumber than us
        # Then we need to take that into account when indexing the list
        for mate in team.inactiveMates:
            if mate.playerNumber < team.brain.my.playerNumber:
                offset += 1

        # Assign role based on closest position
        currentCenterMidX = team.brain.clip(team.brain.ball.x, 
                                        PBConstants.CENTER_MID_LEFT_LIMIT, 
                                        PBConstants.CENTER_MID_RIGHT_LIMIT)
        currentCenterMidPos = (currentCenterMidX, 
                               PBConstants.OFFENSIVE_CENTER_MIDFIELD_Y)
        playerPos = team.getLeastWeightPosition(
            (PBConstants.LEFT_DEEP_BACK_POS,
             PBConstants.RIGHT_DEEP_BACK_POS,
             currentCenterMidPos), team.getOtherActiveTeammates())

        pos = playerPos[team.me.playerNumber - offset]

        if pos == PBConstants.LEFT_DEEP_BACK_POS:
            team.me.role = PBConstants.DEFENDER
            team.me.subRole = PBConstants.LEFT_DEEP_BACK

        elif pos == PBConstants.RIGHT_DEEP_BACK_POS:
            team.me.role = PBConstants.DEFENDER
            team.me.subRole = PBConstants.RIGHT_DEEP_BACK

        else:
            team.me.role = PBConstants.OFFENDER
            team.me.subRole = PBConstants.CENTER_O_MIDFIELD

        # Tie breaking
        if not team.highestActivePlayerNumber():
            for i in xrange(team.brain.my.playerNumber+1, 4):
                if (team.teammates[i-1].calledSubRole == team.me.subRole
                    and not team.teammates[i-1].inactive):
                    pos = playerPos[i - 1]
                    if pos == PBConstants.LEFT_DEEP_BACK_POS:
                        team.role = PBConstants.DEFENDER
                        team.subRole = PBConstants.LEFT_DEEP_BACK

                    elif pos == PBConstants.RIGHT_DEEP_BACK_POS:
                        team.role = PBConstants.DEFENDER
                        team.subRole = PBConstants.RIGHT_DEEP_BACK

                    else:
                        team.role = PBConstants.OFFENDER
                        team.subRole = PBConstants.CENTER_O_MIDFIELD
                    break

    # We have four dogs, two defenders, two middies?
    else:
        # Assign role based on closest position
        leftMiddiePos = (PBConstants.CENTER_MID_LEFT_LIMIT,
                         PBConstants.DEFENSIVE_CENTER_MIDFIELD_Y)
        rightMiddiePos = (PBConstants.CENTER_MID_RIGHT_LIMIT,
                          PBConstants.OFFENSIVE_CENTER_MIDFIELD_Y)

        playerPos = team.getLeastWeightPosition(
            (PBConstants.LEFT_DEEP_BACK_POS,
             PBConstants.RIGHT_DEEP_BACK_POS,leftMiddiePos, rightMiddiePos))

        pos = playerPos[team.me.playerNumber - 1]

        if pos == PBConstants.LEFT_DEEP_BACK_POS:
            team.me.role = PBConstants.DEFENDER
            team.me.subRole = PBConstants.LEFT_DEEP_BACK

        elif pos == PBConstants.RIGHT_DEEP_BACK_POS:
            team.me.role = PBConstants.DEFENDER
            team.me.subRole = PBConstants.RIGHT_DEEP_BACK

        elif pos == leftMiddiePos:
            team.me.role = PBConstants.MIDDIE
            team.me.subRole = PBConstants.RIGHT_O_MIDDIE

        else:
            team.me.role = PBConstants.OFFENDER
            team.me.subRole = PBConstants.CENTER_O_MIDFIELD

        # add some tiebreaking here
        if not team.highestActivePlayerNumber():
            for i in xrange(team.me.playerNumber + 1, 5):
                if team.teammates[i-1].calledSubRole == team.me.subRole:
                    pos = playerPos[i - 1]
                    if pos == PBConstants.LEFT_DEEP_BACK_POS:
                        team.me.role = PBConstants.DEFENDER
                        team.me.subRole = PBConstants.LEFT_DEEP_BACK

                    elif pos == PBConstants.RIGHT_DEEP_BACK_POS:
                        team.me.role = PBConstants.DEFENDER
                        team.me.subRole = PBConstants.RIGHT_DEEP_BACK

                    elif pos == leftMiddiePos:
                        team.me.role = PBConstants.MIDDIE
                        team.me.subRole = PBConstants.RIGHT_O_MIDDIE

                    else:
                        team.me.role = PBConstants.OFFENDER
                        team.me.subRole = PBConstants.CENTER_O_MIDFIELD

                    break

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
    #elif team.brain.my.playerNumber == 4:
    else:
        return ['fKickoffPlay', PBConstants.OFFENDER] + \
            SubRoles.pKickoffPlayStriker(team)

def fKickoff(team):
    '''time immediately after kickoff'''
    if team.me.playerNumber == 2:
        team.me.role = PBConstants.DEFENDER
        return ['fKickoff',PBConstants.DEFENDER] + \
            SubRoles.pKickoffSweeper(team)
    elif team.me.playerNumber == 3:
        team.me.role = PBConstants.CHASER
        return ['fKickoff'] + Roles.rChaser(team)
    elif team.playerNumber == 4:
        team.me.role = PBConstants.OFFENDER
        return ['fKickoff', PBConstants.OFFENDER] + \
            SubRoles.pKickoffStriker(team)

def fTwoKickoff(team):
    """
    Kickoff for only two field players
    """
    other_teammate = team.getOtherActiveTeammate()
    if team.me.playerNumber > other_teammate.playerNumber:
        team.me.role = PBConstants.CHASER
        return ['fTwoKickoff'] + (Roles.rChaser(team))
    else:
        team.me.role = PBConstants.DEFENDER
        return ['fTwoKickoff',PBConstants.DEFENDER] + \
            SubRoles.pKickoffSweeper(team)

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

    # if four dogs alive, position normally
    if num_inactive_teammates == 0:
        if team.me.playerNumber == 2:
            return ['fReady', PBConstants.DEFENDER] + \
                SubRoles.pReadyDefender(team)

        elif team.me.playerNumber == 3:
            return ['fReady', PBConstants.CHASER] + \
                SubRoles.pReadyChaser(team)

        #elif team.me.playerNumber == 4:
        else:
            return ['fReady', PBConstants.OFFENDER] + \
                SubRoles.pReadyOffender(team)

    # two dogs alive, alter positions a bit
    elif num_inactive_teammates == 1:
        other_teammate = team.getOtherActiveTeammate()
        if team.me.playerNumber > other_teammate.playerNumber:
            return ['fReady', PBConstants.CHASER] + \
                SubRoles.pReadyTwoChase(team)
        else:
            return ['fReady', PBConstants.DEFENDER] + \
                SubRoles.pReadyStopper(team)
    # just you
    else:
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


