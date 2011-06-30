from .. import NogginConstants as Constants
from ..util.MyMath import (sub180Angle)
from .Location import RobotLocation

class MyInfo(RobotLocation):
    """
    Class just to store important information about me, containing
    -x -- x coordinate on the field, according to ekf
    -y -- y coordinate on the field, according to ekf
    -h -- heading on the field, according to ekf
    -uncertX -- uncertainty in the x axis, accoridng to ekf
    -uncertY -- uncertainty in the y axis, accoridng to ekf
    -uncertH -- uncertainty in heading, accoridng to ekf
    -name -- integer relating to one of the 12 nBites Aibos (see Constants.py)
    -number -- player number on team
    -teamNumber -- team number according to GameController
    -teamColor -- team color , either TEAM_BLUE or TEAM_RED (see Constants.py)
    -locScore -- GOOD_LOC, OK_LOC, BAD_LOC; how good is our localization
    """
    def __init__(self):
        RobotLocation.__init__(self)
        self.uncertX = 0.0
        self.uncertY = 0.0
        self.uncertH = 0.0
        self.playerNumber = 3
        self.teamColor = Constants.TEAM_BLUE
        self.locScoreXY = Constants.BAD_LOC
        self.locScoreTheta = Constants.BAD_LOC
        self.locScoreFramesBad = 0

    def updateLoc(self, loc):
        if self.teamColor == Constants.TEAM_BLUE:
            self.x = loc.x
            self.y = loc.y
            self.h = loc.h
        else:
            self.x = Constants.FIELD_GREEN_WIDTH - loc.x
            self.y = Constants.FIELD_GREEN_HEIGHT - loc.y
            self.h = sub180Angle(loc.h - 180)
        self.uncertX = loc.xUncert
        self.uncertY = loc.yUncert
        self.uncertH = loc.hUncert
        self.locScoreTheta = self.updateLocScoreTheta()
        self.locScoreXY = self.updateLocScoreXY()
        self.locScore = min(self.locScoreTheta, self.locScoreXY)

        if self.locScore == Constants.BAD_LOC:
            self.locScoreFramesBad += 1
        else :
            self.locScoreFramesBad = 0

    def updateLocScoreTheta(self):
        if self.uncertH < Constants.GOOD_LOC_XY_UNCERT_THRESH:
            return Constants.GOOD_LOC
        elif self.uncertH < Constants.OK_LOC_XY_UNCERT_THRESH:
            return Constants.OK_LOC
        else :
            return Constants.BAD_LOC

    def updateLocScoreXY(self):
        if self.uncertX < Constants.GOOD_LOC_XY_UNCERT_THRESH and \
                self.uncertY < Constants.GOOD_LOC_XY_UNCERT_THRESH:
            return Constants.GOOD_LOC
        elif self.uncertX < Constants.OK_LOC_XY_UNCERT_THRESH and \
                self.uncertY < Constants.OK_LOC_XY_UNCERT_THRESH:
            return Constants.OK_LOC
        else :
            return Constants.BAD_LOC

    def __str__(self):
        return ("#%d on team color: %s @ (%g,%g,%g) uncert: (%g,%g,%g)" %
                (self.playerNumber,
                 Constants.teamColorDict[self.teamColor], self.x, self.y,
                 self.h, self.uncertX, self.uncertY, self.uncertH))
