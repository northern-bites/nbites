from .. import NogginConstants
from ..typeDefs import LocationConstants as locConst
from ..typeDefs import Location
import KickingConstants as constants
import kicks
from math import fabs
from ..util import MyMath

SHOT_AIM_POINT = Location.Location(NogginConstants.FIELD_WIDTH,
                                   NogginConstants.MIDFIELD_Y)

class KickInformation:
    """
    Class to hold all the things we need to decide a kick
    """

    def __init__(self,player):
        self.oppGoalLeftPostBearings = []
        self.oppGoalRightPostBearings = []
        self.myGoalLeftPostBearings = []
        self.myGoalRightPostBearings = []

        self.oppGoalLeftPostDists = []
        self.oppGoalRightPostDists = []
        self.myGoalLeftPostDists = []
        self.myGoalRightPostDists = []

        self.oppLeftPostBearing = None
        self.oppRightPostBearing = None
        self.myLeftPostBearing = None
        self.myRightPostBearing = None

        self.oppLeftPostDist = 0.0
        self.oppRightPostDist = 0.0
        self.myLeftPostDist = 0.0
        self.myRightPostDist = 0.0

        self.sawOwnGoal = False
        self.sawOppGoal = False

        self.haveData = False

        self.player = player

        self.kickObjective = None

        self.orbitAngle = 0.0

    def getKick(self):
        self.kickObjective = self.getKickObjective()
        #if self.kickObjective == constants.OBJECTIVE_ORBIT:
            #return None
        return self.decideKick()

    def getKickObjective(self):
        """
        Return a kick objective based on what we've observed
        """
        self.calculateDataAverages()
        self.calculateBooleans()

        if self.sawOwnGoal:
            # kick out of bounds
            MIN_ORBIT_ANGLE = 60

            if self.myLeftPostBearing is not None and \
                    self.myRightPostBearing is not None:
                myPostBearing = min(fabs(self.myLeftPostBearing),
                                fabs(self.myRightPostBearing))
            elif self.myLeftPostBearing is not None:
                myPostBearing = fabs(self.myLeftPostBearing)
            elif self.myRightPostBearing is not None:
                myPostBearing = fabs(self.myRightPostBearing)
            if abs(myPostBearing) < MIN_ORBIT_ANGLE:
                self.orbitAngle = MyMath.sign(myPostBearing) * \
                    (180 - fabs(myPostBearing))
                return constants.OBJECTIVE_ORBIT
            return constants.OBJECTIVE_CLEAR

        elif self.sawOppGoal:
            return constants.OBJECTIVE_SHOOT

        # we don't see any goalposts
        else:
            return constants.OBJECTIVE_ORBIT

    def collectData(self, info):
        """
        Collect info on any observed goals
        """
        self.haveData = True

        if info.myGoalLeftPost.on:
            if info.myGoalLeftPost.certainty == NogginConstants.SURE:
                self.sawOwnGoal = True
                self.myGoalLeftPostBearings.append(info.myGoalLeftPost.visBearing)
                self.myGoalLeftPostDists.append(info.myGoalLeftPost.visDist)

        if info.myGoalRightPost.on:
            if info.myGoalRightPost.certainty == NogginConstants.SURE:
                self.sawOwnGoal = True
                self.myGoalRightPostBearings.append(info.myGoalRightPost.visBearing)
                self.myGoalRightPostDists.append(info.myGoalRightPost.visDist)

        if info.oppGoalLeftPost.on:
            if info.oppGoalLeftPost.certainty == NogginConstants.SURE:
                self.sawOppGoal = True
                self.oppGoalLeftPostBearings.append(info.oppGoalLeftPost.visBearing)
                self.oppGoalLeftPostDists.append(info.oppGoalLeftPost.visDist)

        if info.oppGoalRightPost.on:
            if info.oppGoalRightPost.certainty == NogginConstants.SURE:
                self.sawOppGoal = True
                self.oppGoalRightPostBearings.append(info.oppGoalRightPost.visBearing)
                self.oppGoalRightPostDists.append(info.oppGoalRightPost.visDist)

    def calculateDataAverages(self):
        """
        Get usable data from the collected data
        """
        if not self.haveData:
            return

        if len(self.myGoalLeftPostBearings) > 0:
            self.myLeftPostBearing = (sum(self.myGoalLeftPostBearings) /
                                      len(self.myGoalLeftPostBearings))
        if len(self.myGoalRightPostBearings) > 0:
            self.myRightPostBearing = (sum(self.myGoalRightPostBearings) /
                                       len(self.myGoalRightPostBearings))
        if len(self.oppGoalLeftPostBearings) > 0:
            self.oppLeftPostBearing = (sum(self.oppGoalLeftPostBearings) /
                                       len(self.oppGoalLeftPostBearings))
        if len(self.oppGoalRightPostBearings) > 0:
            self.oppRightPostBearing = (sum(self.oppGoalRightPostBearings) /
                                        len(self.oppGoalRightPostBearings))

        if len(self.myGoalLeftPostDists) > 0:
            self.myLeftPostDist = (sum(self.myGoalLeftPostDists) /
                                   len(self.myGoalLeftPostDists))
        if len(self.myGoalRightPostDists) > 0:
            self.myRightPostDist = (sum(self.myGoalRightPostDists) /
                                    len(self.myGoalRightPostDists))
        if len(self.oppGoalLeftPostDists) > 0:
            self.oppLeftPostDist = (sum(self.oppGoalLeftPostDists) /
                                    len(self.oppGoalLeftPostDists))
        if len(self.oppGoalRightPostDists) > 0:
            self.oppRightPostDist = (sum(self.oppGoalRightPostDists) /
                                     len(self.oppGoalRightPostDists))


    def calculateBooleans(self):
        self.sawOwnGoal = self.haveSeenOwnGoal()
        self.sawOppGoal = self.haveSeenOppGoal()

    def haveSeenOwnGoal(self):
        if not self.haveData:
            return False

        if (self.myLeftPostBearing != None or
            self.myRightPostBearing != None):
            return True

        return False

    def haveSeenOppGoal(self):
        if not self.haveData:
            return False

        if (self.oppLeftPostBearing != None or
            self.oppRightPostBearing != None):
            return True

        return False

    def averageGoalBearing(leftPost, rightPost):
        sum = leftPost + rightPost
        if (leftPost == 0.0 or rightPost == 0.0):
            return sum

        return sum*.5

    def ownGoalNear(self):
        if self.haveSeenOwnGoal():
            if (self.myLeftPostDist < constants.GPOST_NEAR_THRESHOLD or
                self.myRightPostDist < constants.GPOST_NEAR_THRESHOLD):
                return True
        else:
            return False

    def __str__(self):
        s = ""
        if self.myLeftPostBearing is not None:
            s += ("My left post bearing is: " + str(self.myLeftPostBearing) +
                  " dist is: " + str(self.myLeftPostDist) + "\n")
        if self.myRightPostBearing is not None:
            s += ("My right post bearing is: " + str(self.myRightPostBearing) +
                  " dist is: " + str(self.myRightPostDist) +  "\n")
        if self.oppLeftPostBearing is not None:
            s += ("Opp left post bearing is: " + str(self.oppLeftPostBearing) +
                  " dist is: " + str(self.oppLeftPostDist) + "\n")
        if self.oppRightPostBearing is not None:
            s += ("Opp right post bearing is: " + str(self.oppRightPostBearing)
                  + " dist is: " + str(self.oppRightPostDist) +  "\n")
        if s == "":
            s = "No goal posts observed"
        return s

    def decideKick(self):

        print self

        if self.kickObjective == constants.OBJECTIVE_CLEAR:
            return self.clearBall()

        elif self.kickObjective == constants.OBJECTIVE_SHOOT:
            return self.shootBall()

        else :
            return self.clearBall()

    def clearBall(self):
        """
        Get the ball out of our zone!
        """
        # Get references to the collected data
        myLeftPostBearing =  self.myLeftPostBearing
        myRightPostBearing = self.myRightPostBearing
        oppLeftPostBearing = self.oppLeftPostBearing
        oppRightPostBearing = self.oppRightPostBearing

        # Things to do if we saw our own goal
        # Saw the opponent goal
        my = self.player.brain.my
        if oppLeftPostBearing is not None and \
                oppRightPostBearing is not None:

            avgOppBearing = (oppLeftPostBearing + oppRightPostBearing)/2
            if fabs(avgOppBearing) < constants.ALIGN_FOR_KICK_BEARING_THRESH:
                if constants.DEBUG_KICKS: print ("\t\t Straight 1")
                return kicks.LEFT_DYNAMIC_STRAIGHT_KICK

            elif avgOppBearing > constants.ALIGN_FOR_KICK_BEARING_THRESH:
                if constants.DEBUG_KICKS: print ("\t\t Left 5")
                return kicks.RIGHT_SIDE_KICK
            elif avgOppBearing < -constants.ALIGN_FOR_KICK_BEARING_THRESH:
                if constants.DEBUG_KICKS: print ("\t\t Right 5")
                return kicks.LEFT_SIDE_KICK

        elif self.sawOwnGoal:
            if myLeftPostBearing is not None and myRightPostBearing is not None:
                # Goal in front
                avgMyGoalBearing = (myRightPostBearing + myLeftPostBearing)/2

                if avgMyGoalBearing > 0:
                    if constants.DEBUG_KICKS: print ("\t\tright 1")
                    return kicks.LEFT_SIDE_KICK
                else :
                    if constants.DEBUG_KICKS: print ("\t\tleft 1")
                    return kicks.RIGHT_SIDE_KICK
            else :
                postBearing = 0.0
                if myLeftPostBearing is not None:
                    postBearing = myLeftPostBearing
                else :
                    postBearing = myRightPostBearing
                if postBearing > 0:
                    return kicks.LEFT_SIDE_KICK
                else :
                    return kicks.RIGHT_SIDE_KICK
        else:
            # use localization for kick
            my = self.player.brain.my

            if my.inCenterOfField():
                if abs(my.h) <= constants.CLEAR_CENTER_FIELD_STRAIGHT_ANGLE:
                    if constants.DEBUG_KICKS: print ("\t\tcenter1")
                    return kicks.LEFT_DYNAMIC_STRAIGHT_KICK
                elif my.h < -constants.CLEAR_CENTER_FIELD_STRAIGHT_ANGLE:
                    if constants.DEBUG_KICKS: print ("\t\tcenter2")
                    return kicks.RIGHT_SIDE_KICK
                elif my.h > constants.CLEAR_CENTER_FIELD_STRAIGHT_ANGLE:
                    if constants.DEBUG_KICKS: print ("\t\tcenter3")
                    return kicks.LEFT_SIDE_KICK

            elif my.inTopOfField():
                if locConst.FACING_SIDELINE_ANGLE < my.h:
                    if constants.DEBUG_KICKS: print ("\t\ttop1")
                    return kicks.LEFT_SIDE_KICK
                elif my.h < -90:
                    if constants.DEBUG_KICKS: print ("\t\ttop3")
                    return kicks.RIGHT_SIDE_KICK
                else :
                    if constants.DEBUG_KICKS: print ("\t\ttop4")
                    return kicks.LEFT_DYNAMIC_STRAIGHT_KICK

            elif my.inBottomOfField():
                if -locConst.FACING_SIDELINE_ANGLE > my.h:
                    if constants.DEBUG_KICKS: print ("\t\tbottom1")
                    return kicks.RIGHT_SIDE_KICK
                elif my.h > 90:
                    if constants.DEBUG_KICKS: print ("\t\tbottom3")
                    return kicks.LEFT_SIDE_KICK
                else :
                    if constants.DEBUG_KICKS: print ("\t\tbottom4")
                    return kicks.LEFT_DYNAMIC_STRAIGHT_KICK

        return kicks.LEFT_DYNAMIC_STRAIGHT_KICK

    def shootBallFar(self):
        """
        From 3 point range!
        """
        my = self.player.brain.my
        shotAimPoint = SHOT_AIM_POINT
        bearingToGoal = my.getRelativeBearing(shotAimPoint)
        if constants.DEBUG_KICKS: print "bearing to goal is ", bearingToGoal
        if constants.SHOOT_BALL_FAR_SIDE_KICK_ANGLE > abs(bearingToGoal) > \
                constants.SHOOT_BALL_FAR_LOC_ALIGN_ANGLE and \
                not self.player.hasAlignedOnce:
            self.player.angleToAlign = bearingToGoal
            return kicks.LEFT_DYNAMIC_STRAIGHT_KICK
        elif bearingToGoal > constants.SHOOT_BALL_SIDE_KICK_ANGLE:
            return kicks.RIGHT_SIDE_KICK
        elif bearingToGoal < -constants.SHOOT_BALL_SIDE_KICK_ANGLE:
            return kicks.LEFT_SIDE_KICK
        else :
            return kicks.LEFT_DYNAMIC_STRAIGHT_KICK

    def shootBall(self):
        """
        Put it in the hole!
        """
        # Get references to the collected data
        myLeftPostBearing =  self.myLeftPostBearing
        myRightPostBearing = self.myRightPostBearing
        oppLeftPostBearing = self.oppLeftPostBearing
        oppRightPostBearing = self.oppRightPostBearing
        my = self.player.brain.my

        if oppLeftPostBearing is not None and \
                oppRightPostBearing is not None:

            if (oppRightPostBearing < -constants.KICK_STRAIGHT_POST_BEARING and
                oppLeftPostBearing > constants.KICK_STRAIGHT_POST_BEARING):
                return kicks.LEFT_DYNAMIC_STRAIGHT_KICK

            avgOppBearing = (oppLeftPostBearing + oppRightPostBearing)/2
            if fabs(avgOppBearing) < constants.KICK_STRAIGHT_BEARING_THRESH:
                if constants.DEBUG_KICKS: print ("\t\t Straight 1")
                return kicks.LEFT_DYNAMIC_STRAIGHT_KICK

            elif fabs(avgOppBearing) < constants.ALIGN_FOR_KICK_BEARING_THRESH and \
                    not self.player.hasAlignedOnce:
                if constants.DEBUG_KICKS: print ("\t\t Align 1")
                return kicks.LEFT_DYNAMIC_STRAIGHT_KICK

            elif avgOppBearing > constants.ALIGN_FOR_KICK_BEARING_THRESH:
                if constants.DEBUG_KICKS: print ("\t\t Left 5")
                return kicks.RIGHT_SIDE_KICK
            elif avgOppBearing < -constants.ALIGN_FOR_KICK_BEARING_THRESH:
                if constants.DEBUG_KICKS: print ("\t\t Right 5")
                return kicks.LEFT_SIDE_KICK

        elif myLeftPostBearing is not None and myRightPostBearing is not None:

            avgMyGoalBearing = (myRightPostBearing + myLeftPostBearing)/2
            if my.inCenterOfField():
                if constants.DEBUG_KICKS: print ("\t\tcenterfieldkick")
                if avgMyGoalBearing > 0:
                    return kicks.LEFT_SIDE_KICK
                else :
                    return kicks.RIGHT_SIDE_KICK
            elif my.inTopOfField():
                if constants.DEBUG_KICKS: print ("\t\ttopfieldkick")
                if 90 > avgMyGoalBearing > -30:
                    return kicks.LEFT_SIDE_KICK
                elif avgMyGoalBearing < -30:
                    return kicks.RIGHT_SIDE_KICK
                else :
                    return kicks.LEFT_DYNAMIC_STRAIGHT_KICK
            elif my.inBottomOfField():
                if constants.DEBUG_KICKS: print ("\t\tbottomfieldkick")
                if -90 < avgMyGoalBearing < 30:
                    return kicks.LEFT_SIDE_KICK
                elif avgMyGoalBearing > 30:
                    return kicks.RIGHT_SIDE_KICK
                else :
                    return kicks.LEFT_DYNAMIC_STRAIGHT_KICK

        # if somehow we didn't return already with our kick choice,
        # use localization for kick
        if self.kickObjective == constants.OBJECTIVE_SHOOT:
            return self.shootBallFar()
