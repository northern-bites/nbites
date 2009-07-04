
from .. import NogginConstants
from . import PBConstants
from . import SubRoles

def rChaser(team):
    pos = (team.brain.my.x,team.brain.my.y)
    return [PBConstants.CHASER, PBConstants.CHASE_NORMAL, pos]

def rSearcher(team):
    '''
    Determines positioning for robots while using the finder formation
    '''
    if team.numActiveFieldPlayers == 1:
        pos = PBConstants.READY_KICKOFF_STOPPER
        subRole = PBConstants.OTHER_FINDER
    else:
        pos = team.getLeastWeightPosition(
            PBConstants.TWO_DOG_FINDER_POSITIONS,
            team.getOtherActiveTeammate())
        if pos == PBConstants.TWO_DOG_FINDER_POSITIONS[0]:
            subRole = PBConstants.FRONT_FINDER
        else:
            subRole = PBConstants.OTHER_FINDER

    return [PBConstants.SEARCHER, subRole, pos[:2]]

def rDefender(team):
    '''gets positioning for defender'''
    # If the ball is deep in our side, we become a sweeper
    if team.brain.ball.x < PBConstants.SWEEPER_X:
        return [PBConstants.DEFENDER] + SubRoles.pSweeper(team)

    elif team.brain.ball.x > NogginConstants.MIDFIELD_X:
        if team.brain.ball.y < NogginConstants.MIDFIELD_Y:
            return [PBConstants.DEFENDER] + SubRoles.pBottomStopper(team)
        else:
            return [PBConstants.DEFENDER] + SubRoles.pTopStopper(team)

    elif team.brain.ball.y < NogginConstants.MIDFIELD_Y:
        return [PBConstants.DEFENDER] + SubRoles.pTopStopper(team)
    else:
        return [PBConstants.DEFENDER] + SubRoles.pBottomStopper(team)

def rDefenderOld(team):
    '''obsolete: gets positioning for defender'''
    # If the ball is deep in our side, we become a sweeper
    if team.brain.ball.x < PBConstants.SWEEPER_X:
        return [PBConstants.DEFENDER] + SubRoles.pSweeper(team)

    # Stand between the ball and the back of the goal if it is on our side
    elif PBConstants.USE_DEEP_STOPPER:
        return [PBConstants.DEFENDER] + SubRoles.pDeepStopper(team)
    else:
        return [PBConstants.DEFENDER] + SubRoles.pStopper(team)

def rOffender(team):
    """
    The offensive attacker!
    """
    # RIGHT_WING  if ball is in opp half but not in a corner
    if ((team.brain.ball.x > NogginConstants.CENTER_FIELD_X) and
        (team.brain.ball.y < NogginConstants.CENTER_FIELD_Y)):
        return [PBConstants.OFFENDER] + SubRoles.pRightWing(team)
    # LEFT_WING otherwise
    else:
        return [PBConstants.OFFENDER] + SubRoles.pLeftWing(team)

def rGoalie(team):
    """
    The Goalie
    """
    subrole = None
    if (team.brain.gameController.currentState == 'gameReady' or
        team.brain.gameController.currentState =='gameSet'):
        subrole = SubRoles.pGoalieReady(team)
    if team.goalieShouldChase():
        subrole = SubRoles.pGoalieChaser(team)
    else:
        subrole = SubRoles.pGoalieNormal(team)
    return [PBConstants.GOALIE] + subrole

def rDefensiveMiddie(team):
    """
    Midfielder who plays on the defensive side
    """
    return [PBConstants.DEFENDER] + SubRoles.pDefensiveMiddie(team)

def rOffensiveMiddie(team):
    """
    Midfielder who plays on the offensive side
    """
    return [PBConstants.DEFNEDER] + SubRoles.pOffensiveMiddie(team)
