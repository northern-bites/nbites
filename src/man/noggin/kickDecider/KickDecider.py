from .. import NogginConstants
from . import kicks
from . import KickingConstants as constants
from math import fabs

class KickDecider(object):
    """
    uses current information when called to determine what the best possible
    kick is and where we need to be to execute it
    """

    def __init__(self, brain):
        self.brain = brain
        self.hasKickedOff = False
        self.objDict = { constants.OBJECTIVE_CLEAR:self.kickoff(),
                          constants.OBJECTIVE_CENTER:self.center(),
                          constants.OBJECTIVE_SHOOT:self.shoot(),
                          constants.OBJECTIVE_KICKOFF:self.kickoff() }
        self.currentKick = None

    def decideKick(self):
        """
        using objective and localization determines best kick to make
        """

        objective = self.getObjective()

        # uses dictionary to retrieve and call proper method
        kickDest = self.objDict[objective]
        # take my position and destination of kick to decide which kick
        # need to consider: distance to kick, time needed to align for kick
        # prioritize time to align
        # calculate bearing to dest
        bearing = self.brain.my.getRelativeBearing(kickDest)
        if fabs(bearing) >= 45.0:
            # kick sideways
            # left or right?
            # positive bearing means dest is to my left, so kick right
            if bearing > 0:
                kick = kicks.RIGHT_SIDE_KICK
            else:
                kick = kicks.LEFT_SIDE_KICK

        else:
            #kick straight
            # left or right foot?
            leftFootKick = False
            
            if self.brain.my.y < self.brain.ball.y:
                # right foot
                leftFootKick = False
            else:
                # left
                leftFootKick = True

            # short, far, big?
            dist = self.brain.my.distTo(kickDest)
            if dist < 2.0:
                if leftFootKick:
                    kick = kicks.LEFT_SHORT_KICK
                else:
                    kick = kicks.RIGHT_SHORT_KICK
            elif dist < 4.0:
                if leftFootKick:
                    kick = kicks.LEFT_FAR_KICK
                else:
                    kick = kicks.RIGHT_FAR_KICK
            else:
                if leftFootKick:
                    kick = kicks.LEFT_BIG_KICK
                else:
                    kick = kicks.RIGHT_BIG_KICK

        kick.heading = self.brain.ball.headingTo(kickDest)
        self.currentKick = kick

    def getObjective(self):
        """
        determines what we want to do with the ball
        """
        ball = self.brain.ball

        if not self.hasKickedOff:
            return constants.OBJECTIVE_KICKOFF

        # if ball on our side, get it out!
        if ball.x < NogginConstants.CENTER_FIELD_X:
            # clear (or maybe pass?)
            return constants.OBJECTIVE_CLEAR

        # if deep in opponent corner, center the ball
        elif (ball.x > NogginConstants.OPP_GOALBOX_LEFT_X and 
              (ball.y > NogginConstants.OPP_GOALBOX_TOP_Y or
               ball.y < NogginConstants.OPP_GOALBOX_BOTTOM_Y)):
            return constants.OBJECTIVE_CENTER

        return constants.OBJECTIVE_SHOOT

    def kickoff(self):
        """returns a destination for kickoff kick """
        self.hasKickedOff = True
        return constants.LEFT_KICKOFF_POINT

    def clear(self):
        """chooses whether to use left or right clear destination """
        ball = self.brain.ball
        if ball.y < NogginConstants.CENTER_FIELD_Y:
            return constants.LEFT_CLEAR_POINT
        else:
            return constants.RIGHT_CLEAR_POINT

    def center(self):
        """ returns point to center ball"""
        return constants.CENTER_BALL_POINT

    def shoot(self):
        """ returns best location to shoot at"""
        # TODO: use vision info to find and shoot at gaps

        ball = self.brain.ball
        if ball.y < NogginConstants.CENTER_FIELD_Y:
            return constants.SHOOT_AT_LEFT_AIM_POINT
        else:
            return constants.SHOOT_AT_RIGHT_AIM_POINT
