
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
        pos = PBConstants.READY_NON_KICKOFF_LEFT_POSITION
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
    # CENTER STRIKER if ball is in the opponent goalbox
    if (team.brain.ball.y > NogginConstants.OPP_GOALBOX_BOTTOM_Y and
          NogginConstants.OPP_GOALBOX_LEFT_X < team.brain.ball.x <
          NogginConstants.OPP_GOALBOX_RIGHT_X):
        # Add buffers for wings
        """
        if (team.currentSubRole == PBConstants.LEFT_WING and
            team.brain.ball.y < NogginConstants.OPP_GOALBOX_BOTTOM_Y +
            PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE and
            team.brain.ball.x > NogginConstants.CENTER_FIELD_X -
            PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        elif (team.currentSubRole == PBConstants.RIGHT_WING and
              team.brain.ball.y < NogginConstants.OPP_GOALBOX_BOTTOM_Y +
              PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE and
              team.brain.ball.x < NogginConstants.CENTER_FIELD_X +
              PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer moving from strikers
        elif (team.currentSubRole == PBConstants.LEFT_STRIKER and
              team.brain.ball.x > NogginConstants.OPP_GOALBOX_RIGHT_X -
              PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        elif (team.currentSubRole == PBConstants.RIGHT_STRIKER and
              team.brain.ball.x < NogginConstants.OPP_GOALBOX_LEFT_X +
              PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        
        else:"""
        return [PBConstants.OFFENDER] + \
            SubRoles.pCenterStriker(team)
    
    # LEFT STRIKER if the ball is in the left corner
    elif (team.brain.ball.y > PBConstants.OPP_CORNERS_Y and
          team.brain.ball.x < NogginConstants.OPP_GOALBOX_LEFT_X):
        """# if in left wing, have a bigger buffer to switch to right striker
        if (team.currentSubRole == PBConstants.LEFT_WING and
            team.brain.ball.y < PBConstants.OPP_CORNERS_Y +
            PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # have a larget buffer for the sideline sub roles
        elif (team.currentSubRole == PBConstants.RIGHT_SIDELINE_O and
              team.brain.ball.y < PBConstants.OPP_CORNERS_Y +
              PBConstants.SIDELINE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer moving from center striker
        elif (team.currentSubRole == PBConstants.CENTER_STRIKER and
              team.brain.ball.x > NogginConstants.OPP_GOALBOX_LEFT_X -
              PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        else:"""
        return [PBConstants.OFFENDER] + SubRoles.pLeftStriker(team)
    
    # RIGHT STRIKER if the ball is in the right corner
    elif (team.brain.ball.y > PBConstants.OPP_CORNERS_Y and
          team.brain.ball.x > NogginConstants.OPP_GOALBOX_RIGHT_X):
        """# Buffer moving from the wing and sideline
        if (team.currentSubRole == PBConstants.RIGHT_WING and
            team.brain.ball.y < PBConstants.OPP_CORNERS_Y +
            PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # have a larget buffer for the sideline sub roles
        elif (team.currentSubRole == PBConstants.LEFT_SIDELINE_O and
              team.brain.ball.y < PBConstants.OPP_CORNERS_Y +
              PBConstants.SIDELINE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer moving from center striker
        elif (team.currentSubRole == PBConstants.CENTER_STRIKER and
              team.brain.ball.x < NogginConstants.OPP_GOALBOX_RIGHT_X +
              PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        else:"""
        return [PBConstants.OFFENDER] + SubRoles.pRightStriker(team)
    
    # RIGHT_SIDELINE_OFFENDER if ball is near right sideline
    elif (team.brain.ball.y > NogginConstants.CENTER_FIELD_Y and
          team.brain.ball.x >=
          PBConstants.RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X):
        # Buffer with striker
        """if (team.currentSubRole == PBConstants.LEFT_STRIKER and
            team.brain.ball.y > PBConstants.OPP_CORNERS_Y -
            PBConstants.SIDELINE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer with wing
        elif (team.currentSubRole == PBConstants.LEFT_WING and
              team.brain.ball.x <
              PBConstants.RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X +
              PBConstants.SIDELINE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer with forward middie
        elif (team.currentSubRole == PBConstants.FORWARD_MIDFIELD and
              (team.brain.ball.y < NogginConstants.CENTER_FIELD_Y +
               PBConstants.SIDELINE_BUFFER_SIZE or
               team.brain.ball.x <
               PBConstants.RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X
               + PBConstants.SIDELINE_BUFFER_SIZE)):
            return [team.currentRole, team.currentSubRole]
        else:"""
        return [PBConstants.OFFENDER] +  SubRoles.pRightSidelineO(team)
    
    # LEFT_SIDELINE_OFFENDER
    elif (team.brain.ball.y > NogginConstants.CENTER_FIELD_Y and
          team.brain.ball.x <
          PBConstants.LEFT_SIDELINE_OFFENDER_BALL_THRESH_X):
        # Buffer with striker
        """if (team.currentSubRole == PBConstants.RIGHT_STRIKER and
            team.brain.ball.y > PBConstants.OPP_CORNERS_Y -
            PBConstants.SIDELINE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer with wing
        elif (team.currentSubRole ==
              PBConstants.RIGHT_WING and team.brain.ball.x >
              PBConstants.RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X -
              PBConstants.SIDELINE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer with forward middie
        elif (team.currentSubRole == PBConstants.FORWARD_MIDFIELD and
              (team.brain.ball.y < NogginConstants.CENTER_FIELD_Y +
               PBConstants.SIDELINE_BUFFER_SIZE or
               team.brain.ball.x >
               PBConstants.LEFT_SIDELINE_OFFENDER_BALL_THRESH_X
               - PBConstants.SIDELINE_BUFFER_SIZE)):
            return [team.currentRole, team.currentSubRole]
        else:"""
        return [PBConstants.OFFENDER] + SubRoles.pLeftSidelineO(team)
    
    # RIGHT_WING  if ball is in opp half but not in a corner
    elif team.brain.ball.x < NogginConstants.CENTER_FIELD_X:
        
        """# Don't switch from left wing if the ball is close to center field
        if (team.currentSubRole == PBConstants.LEFT_WING and
            team.brain.ball.x > NogginConstants.CENTER_FIELD_X -
            PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Don't switch out of striker if ball just left the corner
        elif (team.currentSubRole == PBConstants.RIGHT_STRIKER and
              team.brain.ball.y > PBConstants.OPP_CORNERS_Y -
              PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer with center striker
        elif (team.currentSubRole == PBConstants.CENTER_STRIKER and
              team.brain.ball.y > NogginConstants.OPP_GOALBOX_BOTTOM_Y -
              PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer with left sideline
        elif (team.currentSubRole == PBConstants.LEFT_SIDELINE_O and
              team.brain.ball.x >
              PBConstants.LEFT_SIDELINE_OFFENDER_BALL_THRESH_X
              - PBConstants.SIDELINE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer with forward middie
        elif (team.currentSubRole == PBConstants.FORWARD_MIDFIELD and
              team.brain.ball.y <
              PBConstants.FORWARD_MIDFIELD_BALL_THRESH_Y +
              PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        else:"""
        return [PBConstants.OFFENDER] + SubRoles.pRightWing(team)
    
    # LEFT_WING otherwise
    else:
        # Don't switch from right wing if the ball is close to center field
        """if (team.currentSubRole == PBConstants.RIGHT_WING and
            team.brain.ball.x < NogginConstants.CENTER_FIELD_X +
            PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        
        # Don't switch out of striker if ball just left the corner
        elif (team.currentSubRole == PBConstants.LEFT_STRIKER and
              team.brain.ball.y > PBConstants.OPP_CORNERS_Y -
              PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer with center striker
        elif (team.currentSubRole == PBConstants.CENTER_STRIKER and
              team.brain.ball.y > NogginConstants.OPP_GOALBOX_BOTTOM_Y -
              PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer with right sideline
        elif (team.currentSubRole == PBConstants.RIGHT_SIDELINE_O and
              team.brain.ball.x <
              PBConstants.RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X
              + PBConstants.SIDELINE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        # Buffer with forward middie
        elif (team.currentSubRole == PBConstants.FORWARD_MIDFIELD and
              team.brain.ball.y <
              PBConstants.FORWARD_MIDFIELD_BALL_THRESH_Y +
              PBConstants.OFFENDER_SUB_ROLE_BUFFER_SIZE):
            return [team.currentRole, team.currentSubRole]
        else:"""
        return [PBConstants.OFFENDER] + SubRoles.pLeftWing(team)

def rMiddie(team):
    '''gets position for the middie'''
    # Assume the SubRoles of the defender and attacker
    if PBConstants.MIDFIELD_ZONE_MIN_Y < team.brain.ball.y < \
            PBConstants.MIDFIELD_ZONE_MAX_Y:
        if team.brain.ball.x < NogginConstants.MIDFIELD_X:
            return [PBConstants.MIDDIE] + SubRoles.pRightOMiddie(team)
        else:
            return [PBConstants.MIDDIE] + SubRoles.pLeftOMiddie(team)
    elif team.brain.ball.y < PBConstants.MIDFIELD_ZONE_MIN_Y:
        return [PBConstants.MIDDIE] + SubRoles.pCenterOMidfield(team)
    return [PBConstants.MIDDIE] + SubRoles.pDefensiveMidfield(team)
