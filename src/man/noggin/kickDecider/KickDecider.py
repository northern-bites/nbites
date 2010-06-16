from .. import NogginConstants
from . import kicks
from . import KickingConstants as constants
from math import fabs
from ..util import MyMath

class KickDecider(object):
    """
    uses current information when called to determine what the best possible
    kick is and where we need to be to execute it
    """

    def __init__(self, brain):
        self.brain = brain
        self.hasKickedOff = True
        self.objDict = { constants.OBJECTIVE_CLEAR:self.clear,
                         constants.OBJECTIVE_CENTER:self.center,
                         constants.OBJECTIVE_SHOOT:self.shoot,
                         constants.OBJECTIVE_KICKOFF:self.kickoff }
        self.kickDest = None
        self.destDist = 0.
        self.currentObj = None
        self.currentKick = None

    def getSweetMove(self):
        """
        returns the proper sweet move to execute to kick
        """
        if self.currentKick == kicks.LEFT_DYNAMIC_STRAIGHT_KICK or \
               self.currentKick == kicks.RIGHT_DYNAMIC_STRAIGHT_KICK:
            ball = self.brain.ball
            if self.currentObj == constants.OBJECTIVE_SHOOT:
                dist = 500.
            else:
                dist = self.destDist
            return self.currentKick.sweetMove(ball.relY, dist)
        else:
            return self.currentKick.sweetMove

    def decideKick(self):
        """
        using objective and localization determines best kick to make
        """

        self.currentObj= self.getObjective()

        # uses dictionary to retrieve and call proper method
        self.kickDest = self.objDict[self.currentObj]()
        # take my position and destination of kick to decide which kick
        # need to consider: distance to kick, time needed to align for kick
        # prioritize time to align
        # calculate bearing to dest
        bearing = MyMath.sub180Angle(self.brain.ball.headingTo(self.kickDest) -
                                     self.brain.my.h)
        print "bearing: %g" % bearing
        if fabs(bearing) >= 75.:
            print "kick sideways"
            # kick sideways
            # left or right?
            # positive bearing means dest is to my left, so kick right
            if bearing > 0:
                kick = kicks.RIGHT_SIDE_KICK
                kick.heading = MyMath.sub180Angle(
                    self.brain.ball.headingTo(self.kickDest) - 90.)
            else:
                kick = kicks.LEFT_SIDE_KICK
                kick.heading = MyMath.sub180Angle(
                    self.brain.ball.headingTo(self.kickDest) + 90.)

        else:
            print "kick straight"
            #kick straight
            # if my left foot is closer
            #kick = kicks.LEFT_DYNAMIC_STRAIGHT_KICK
            # if my right foot is closer
            kick = kicks.RIGHT_DYNAMIC_STRAIGHT_KICK
            kick.heading = self.brain.ball.headingTo(self.kickDest)

        self.destDist = self.brain.ball.distTo(self.kickDest)
        self.currentKick = kick
        print "kickDest:", self.kickDest
        print "my.x:%g my.y:%g my.h:%g" % (self.brain.my.x, self.brain.my.y, self.brain.my.h)

        self.brain.out.printf(self.currentKick)

    def getObjective(self):
        """
        determines what we want to do with the ball
        """
        ball = self.brain.ball

        if self.brain.gameController.ownKickOff and \
                self.brain.player.isChasing and \
                not self.hasKickedOff:
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
        print "kickoff"
        self.hasKickedOff = True
        return constants.LEFT_KICKOFF_POINT

    def clear(self):
        """chooses whether to use left or right clear destination """
        ball = self.brain.ball
        if ball.y < NogginConstants.CENTER_FIELD_Y:
            print "clear left"
            return constants.LEFT_CLEAR_POINT
        else:
            print "clear right"
            return constants.RIGHT_CLEAR_POINT

    def center(self):
        """ returns point to center ball"""
        print "center the ball"
        return constants.CENTER_BALL_POINT

    def shoot(self):
        """ returns best location to shoot at"""
        # TODO: use vision info to find and shoot at gaps

        ball = self.brain.ball
        if ball.y < NogginConstants.CENTER_FIELD_Y:
            print "shoot left"
            return constants.SHOOT_AT_LEFT_AIM_POINT
        else:
            print "shoot right"
            return constants.SHOOT_AT_RIGHT_AIM_POINT
