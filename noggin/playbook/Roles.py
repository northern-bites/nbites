
from .. import NogginConstants
from . import PBConstants
from . import SubRoles

def rChaser(team, workingPlay):
    workingPlay.setRole(PBConstants.CHASER)
    workingPlay.setSubRole(PBConstants.CHASE_NORMAL)
    pos = (team.brain.my.x,team.brain.my.y)
    workingPlay.setPosition(pos)

def rSearcher(team, workingPlay):
    '''
    Determines positioning for robots while using the finder formation
    '''
    workingPlay.setRole(PBConstants.SEARCHER)
    if team.numActiveFieldPlayers == 1:
        workingPlay.setSubRole(PBConstants.OTHER_FINDER)
        workingPlay.setPosition(PBConstants.READY_KICKOFF_STOPPER)
    else:
        teammate = team.getOtherActiveTeammate()
        pos = team.getLeastWeightPosition(
            PBConstants.TWO_DOG_FINDER_POSITIONS,
            teammate)
        if pos == PBConstants.TWO_DOG_FINDER_POSITIONS[0]:
            workingPlay.setSubRole(PBConstants.FRONT_FINDER)
        else:
            workingPlay.setSubRole(PBConstants.OTHER_FINDER)
        workingPlay.setPosition(pos[:2])

def rDefender(team, workingPlay):
    '''gets positioning for defender'''
    workingPlay.setRole(PBConstants.DEFENDER)
    # If the ball is deep in our side, we become a sweeper
    if team.brain.ball.x < PBConstants.SWEEPER_X:
        SubRoles.pSweeper(team, workingPlay)

    elif team.brain.ball.x > NogginConstants.MIDFIELD_X:
        if team.brain.ball.y < NogginConstants.MIDFIELD_Y:
            SubRoles.pBottomStopper(team, workingPlay)
        else:
            SubRoles.pTopStopper(team, workingPlay)

    elif team.brain.ball.y < NogginConstants.MIDFIELD_Y:
        SubRoles.pTopStopper(team, workingPlay)
    else:
        SubRoles.pBottomStopper(team, workingPlay)

def rOffender(team, workingPlay):
    """
    The offensive attacker!
    """
    workingPlay.setRole(PBConstants.OFFENDER)
    # RIGHT_WING  if ball is in opp half but not in a corner
    if ((team.brain.ball.x > NogginConstants.CENTER_FIELD_X) and
        (team.brain.ball.y < NogginConstants.CENTER_FIELD_Y)):
        SubRoles.pRightWing(team, workingPlay)
    # LEFT_WING otherwise
    else:
        SubRoles.pLeftWing(team, workingPlay)

def rGoalie(team, workingPlay):
    """
    The Goalie
    """
    workingPlay.setRole(PBConstants.GOALIE)
    if (team.brain.gameController.currentState == 'gameReady' or
        team.brain.gameController.currentState =='gameSet'):
        SubRoles.pGoalieReady(team, workingPlay)
    if team.goalieShouldChase():
        SubRoles.pGoalieChaser(team, workingPlay)
    else:
        SubRoles.pGoalieNormal(team, workingPlay)

def rDefensiveMiddie(team, workingPlay):
    """
    Midfielder who plays on the defensive side
    """
    #HACK -courtesy of Tucker
    workingPlay.setRole(PBConstants.DEFENDER)
    SubRoles.pDefensiveMiddie(team, workingPlay)

def rOffensiveMiddie(team, workingPlay):
    """
    Midfielder who plays on the offensive side
    """
    #HACK -courtesy of Tucker
    workingPlay.setRole(PBConstants.DEFENDER)
    SubRoles.pOffensiveMiddie(team, workingPlay)
