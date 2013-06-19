from ..playbook.PBConstants import (INIT_ROLE,
                                    ROLES,
                                    CHASER)
from objects import Location, RobotLocation

class Play:
    def __init__(self,
                 role = INIT_ROLE,
                 position = RobotLocation(0,0,0)
                 ):
        self.role = role
        self.position = position
        self.changed = True

    def setRole(self, role):
        if self.role != role:
            self.role = role
            self.changed = True

    def setPosition(self, position):
        if isinstance(position, RobotLocation):
            self.position = position
        else:
            self.position.x = position[0]
            self.position.y = position[1]
            self.position.h = position[2]

    def isRole(self, roleToTest):
        return (self.role == roleToTest)

    def isChaser(self):
        return (self.role == CHASER)

    def getPosition(self):
        return self.position

    def getPositionCoord(self):
        return Location(self.position.x, self.position.y)

    def __eq__(self, other):
        '''compares two plays for equality by value'''

        return self.role == other.role

    def __str__(self):
        return ("Role:" + ROLES[self.role])
