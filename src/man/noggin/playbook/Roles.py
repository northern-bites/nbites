from .. import NogginConstants
from . import PBConstants
from . import SubRoles

def rChaser(team, workingPlay):
    workingPlay.setRole(PBConstants.CHASER)
    workingPlay.setSubRole(PBConstants.CHASE_NORMAL)
    pos = (team.brain.my.x,team.brain.my.y, team.brain.ball.heading)
    workingPlay.setPosition(pos)

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

def rMiddie(team, workingPlay):

    workingPlay.setRole(PBConstants.MIDDIE)
    SubRoles.pDefensiveMiddie(team, workingPlay)

def rDefenderDubD(team, workingPlay):

    workingPlay.setRole(PBConstants.DEFENDER_DUB_D)
     # Figure out who isn't penalized with you
    other_teammate = team.getOtherActiveTeammate()

    leftPos = PBConstants.LEFT_DEEP_BACK_POS
    rightPos = PBConstants.RIGHT_DEEP_BACK_POS
    # Figure out who should go to which position
    pos = team.getLeastWeightPosition((leftPos,rightPos), other_teammate)

    if pos == leftPos:
        SubRoles.pLeftDeepBack(team, workingPlay)
    else: #if pos == rightPos
        SubRoles.pRightDeepBack(team, workingPlay)
