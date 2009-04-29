
from .. import NogginConstants
from . import PBConstants
from . import SubRoles

def rChaser(team):
    """
    Chaser role decides if normal chasing should occur or if pFields need
    to be used in order to avoid the goalie box
    """
    # If we are in the corners or the ball is in a corner, we avoid the box
    if ((team.brain.ball.x < NogginConstants.MY_GOALBOX_LEFT_X and
        team.brain.ball.y < NogginConstants.MY_GOALBOX_TOP_Y and
        team.brain.my.x > NogginConstants.MY_GOALBOX_LEFT_X) or
        (team.brain.ball.x > NogginConstants.MY_GOALBOX_RIGHT_X and
        team.brain.ball.y < NogginConstants.MY_GOALBOX_TOP_Y and
        team.brain.my.x < NogginConstants.MY_GOALBOX_RIGHT_X) or
        (team.brain.my.x < NogginConstants.MY_GOALBOX_LEFT_X and
        team.brain.my.y < NogginConstants.MY_GOALBOX_TOP_Y and
        team.brain.ball.x > NogginConstants.MY_GOALBOX_LEFT_X) or
        (team.brain.my.x > NogginConstants.MY_GOALBOX_RIGHT_X and
        team.brain.my.y < NogginConstants.MY_GOALBOX_TOP_Y and
        team.brain.ball.x < NogginConstants.MY_GOALBOX_RIGHT_X)):
        pos = (team.brain.ball.x,team.brain.ball.y)
        return [PBConstants.CHASER, PBConstants.CHASE_AROUND_BOX, pos]
    # Almost always chase normal, i.e. without potential fields
    else:
        pos = (team.brain.my.x,team.brain.my.y)
        return [PBConstants.CHASER, PBConstants.CHASE_NORMAL, pos]
        #team.position = (team.brain.my.x,team.brain.my.y,False,False)

def rSearcher(team):
    '''
    Determines positioning for robots while using the finder formation
    '''
    
    team.me.role = PBConstants.SEARCHER
    
    if team.numInactiveMates == 1:
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
    if team.brain.ball.y < PBConstants.SWEEPER_Y:
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
