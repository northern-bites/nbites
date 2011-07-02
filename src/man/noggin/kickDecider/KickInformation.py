import KickingConstants as constants
import vision

class KickInformation:
    """
    Class to hold all the things we need to decide a kick
    """
    def __init__(self, decider, brain):
        self.decider = decider
        self.brain = brain

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
        self.oppAvgPostBearing = None
        self.myLeftPostBearing = None
        self.myRightPostBearing = None
        self.myAvgPostBearing = None

        self.oppLeftPostDist = 0.0
        self.oppRightPostDist = 0.0
        self.oppAvgPostDist = 0.0
        self.myLeftPostDist = 0.0
        self.myRightPostDist = 0.0
        self.myAvgPostDist = 0.0

        self.sawOwnGoal = False
        self.sawOppGoal = False

        self.haveData = False

        self.kickObjective = None
        self.kick = None
        self.kickDest = None
        self.destDist = 500.

        self.orbitAngle = 170

    def getKickObjective(self):
        """
        Return a kick objective based on what we've observed
        """
        self.calculateDataAverages()

        if self.sawOppGoal:
            self.kickObjective = constants.OBJECTIVE_SHOOT
            return self.kickObjective
        else:
            self.kickObjective = constants.OBJECTIVE_CLEAR
            return self.kickObjective

    def collectData(self):
        """
        Collect info on any observed goals
        """
        self.haveData = True

        if self.brain.myGoalLeftPost.vis.on:
            if self.brain.myGoalLeftPost.vis.certainty ==\
                    vision.certainty._SURE:
                self.sawOwnGoal = True
                self.myGoalLeftPostBearings.\
                    append(self.brain.myGoalLeftPost.vis.bearing)
                self.myGoalLeftPostDists.\
                    append(self.brain.myGoalLeftPost.vis.dist)

        if self.brain.myGoalRightPost.vis.on:
            if self.brain.myGoalRightPost.vis.certainty ==\
                    vision.certainty._SURE:
                self.sawOwnGoal = True
                self.myGoalRightPostBearings.append\
                    (self.brain.myGoalRightPost.vis.bearing)
                self.myGoalRightPostDists.append\
                    (self.brain.myGoalRightPost.vis.dist)

        if self.brain.oppGoalLeftPost.vis.on:
            if self.brain.oppGoalLeftPost.vis.certainty ==\
                    vision.certainty._SURE:
                self.sawOppGoal = True
                self.oppGoalLeftPostBearings.append\
                    (self.brain.oppGoalLeftPost.vis.bearing)
                self.oppGoalLeftPostDists.append\
                    (self.brain.oppGoalLeftPost.vis.dist)

        if self.brain.oppGoalRightPost.vis.on:
            if self.brain.oppGoalRightPost.vis.certainty == \
                    vision.certainty._SURE:
                self.sawOppGoal = True
                self.oppGoalRightPostBearings.append\
                    (self.brain.oppGoalRightPost.vis.bearing)
                self.oppGoalRightPostDists.append\
                    (self.brain.oppGoalRightPost.vis.dist)

    def calculateDataAverages(self):
        """
        calculates averages based on data collected
        """
        if not self.haveData:
            return

        # specific post bearings
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
        # average post bearings
        if self.myLeftPostBearing is not None and self.myRightPostBearing is not None:
            self.myAvgPostBearing = (self.myLeftPostBearing + self.myRightPostBearing)*.5
        elif self.myLeftPostBearing is not None:
            self.myAvgPostBearing = self.myLeftPostBearing + 5. #somewhere in middle
        elif self.myRightPostBearing is not None:
            self.myAvgPostBearing = self.myRightPostBearing - 5. #somewhere in middle
        if self.oppLeftPostBearing is not None and self.oppRightPostBearing is not None:
            self.oppAvgPostBearing = (self.oppLeftPostBearing + self.oppRightPostBearing)*.5
        elif self.oppLeftPostBearing is not None:
            self.oppAvgPostBearing = self.oppLeftPostBearing + 5. #somewhere in middle
        elif self.oppRightPostBearing is not None:
            self.oppAvgPostBearing = self.oppRightPostBearing - 5. #somewhere in middle

        # distance averages
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
        # average post distances
        if self.myLeftPostBearing is not None and self.myRightPostBearing is not None:
            self.myAvgPostDist = (self.myLeftPostDist + self.myRightPostDist)*.5
        elif self.myLeftPostBearing is not None:
            self.myAvgPostDist = self.myLeftPostDist
        elif self.myRightPostBearing is not None:
            self.myAvgPostDist = self.myRightPostDist
        if self.oppLeftPostBearing is not None and self.oppRightPostBearing is not None:
            self.oppAvgPostDist = (self.oppLeftPostDist + self.oppRightPostDist)*.5
        elif self.oppLeftPostBearing is not None:
            self.oppAvgPostDist = self.oppLeftPostDist
        elif self.oppRightPostBearing is not None:
            self.oppAvgPostDist = self.oppRightPostDist


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
            s = "No goal posts observed\n"
        if self.kickObjective == constants.OBJECTIVE_SHOOT:
            s += "Objective is: SHOOT\n"
        if self.kickObjective == constants.OBJECTIVE_CLEAR:
            s += "Objective is: CLEAR\n"
        return s
