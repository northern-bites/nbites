from .. import NogginConstants
import KickingConstants as constants
import kicks

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

    def getKickObjective(self):
        """
        Return a kick objective based on what we've observed
        """
        self.calculateDataAverages()
        self.calculateBooleans()

        if self.sawOwnGoal:
            # kick out of bounds
            if self.ownGoalNear():
                return constants.OBJECTIVE_CLEAR

            # clear towards middle
            else:
                return constants.OBJECTIVE_CENTER

        elif self.sawOppGoal:
            return constants.OBJECTIVE_SHOOT

        # we don't see any goalposts
        else:
            return constants.OBJECTIVE_UNCLEAR

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

    def calculateDataAverage(self):
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


    def determineBooleans(self):
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
            if (self.myLeftPostDist < GPOST_NEAR_THRESHOLD or
                self.myRightPostDist < GPOST_NEAR_THRESHOLD):
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
