#!/usr/bin/env python
import Constants
from JTeam import *
from Coop import SubRoles
import sys

DEBUG = False

class PBReadIn():
    # Class Constants
    SUPER_NEG_VALUE = -1000000
    SUPER_POS_VALUE = 1000000

    def __init__(self):
        self.team = JTeam()
        self.subRoles = []

    def readInSubRoles(self):
        # Setup our fake data

        for f in SubRoles.positions:
            sb = SubRole()
            sb.name = f.__name__
            if DEBUG:
                print sb.name
            # First determine the min values
            self.team.brain.ball.x = self.SUPER_NEG_VALUE
            self.team.brain.ball.y = self.SUPER_NEG_VALUE
            ins = f(self.team)
            sb.xMin = ins[0]
            sb.yMin = ins[1]

            if sb.xMin < Constants.FIELD_WHITE_LEFT_SIDELINE_X:
                sb.xMin = Constants.FIELD_WHITE_LEFT_SIDELINE_X
            if sb.yMin < Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y:
                sb.yMin = Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y

            # Determine the max values
            self.team.brain.ball.x = self.SUPER_POS_VALUE
            self.team.brain.ball.y = self.SUPER_POS_VALUE
            ins = f(self.team)
            sb.xMax = ins[0]
            sb.yMax = ins[1]

            if sb.xMax > Constants.FIELD_WHITE_RIGHT_SIDELINE_X:
                sb.xMax = Constants.FIELD_WHITE_RIGHT_SIDELINE_X
            if sb.yMax > Constants.FIELD_WHITE_TOP_SIDELINE_Y:
                sb.yMax = Constants.FIELD_WHITE_TOP_SIDELINE_Y

            # Determine x and y offsets
            self.team.brain.ball.x = (sb.xMin + sb.xMax) / 2
            self.team.brain.ball.y = (sb.yMin + sb.yMax) / 2
            ins = f(self.team)
            xMid = ins[0]
            yMid = ins[1]
            sb.xOff = xMid - self.team.brain.ball.x
            sb.yOff = yMid - self.team.brain.ball.y

            sb.ballRepulsor = self.team.moveBall
            self.subRoles.append(sb)

    def outputSubRoleInfos(self):
        for sb in self.subRoles:
            if sb.ballRepulsor:
                br = 1
            else:
                br = 0
            print sb.name
            print sb.xMin, sb.xMax, sb.yMin, sb.yMax, sb.xOff, sb.yOff, br

def main(*args):
    reader = PBReadIn()
    reader.readInSubRoles()

    # Output the stuff
    outfile = open("srData.txt", 'w')
    sys.stdout = outfile
    reader.outputSubRoleInfos()
    outfile.close()

if __name__=='__main__':
    sys.exit(main(*sys.argv[1:]))
