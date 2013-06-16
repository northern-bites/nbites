from ..playbook.PBConstants import (INIT_STRATEGY,
                                    INIT_FORMATION,
                                    INIT_ROLE,
                                    INIT_SUB_ROLE,
                                    STRATEGIES,
                                    FORMATIONS,
                                    ROLES,
                                    SUB_ROLES,
                                    CHASER)
from objects import Location, RobotLocation

class Play:
    def __init__(self,
                 strategy = INIT_STRATEGY,
                 formation = INIT_FORMATION,
                 role = INIT_ROLE,
                 subRole = INIT_SUB_ROLE,
                 position = RobotLocation(0,0,0)
                 ):
        self.strategy = strategy
        self.formation = formation
        self.role = role
        self.subRole = subRole
        self.position = position
        self.changed = True

    def setStrategy(self, strategy):
        if self.strategy != strategy:
            self.strategy = strategy
            self.changed = True

    def setFormation(self, formation):
        if self.formation != formation:
            self.formation = formation
            self.changed = True

    def setRole(self, role):
        if self.role != role:
            self.role = role
            self.changed = True

    def setSubRole(self, subRole):
        if self.subRole != subRole:
            self.subRole = subRole
            self.changed = True

    def setPosition(self, position):
        if isinstance(position, RobotLocation):
            self.position = position
        else:
            self.position.x = position[0]
            self.position.y = position[1]
            self.position.h = position[2]

    def isSubRole(self, subRoleToTest):
        return (self.subRole == subRoleToTest)

    def isRole(self, roleToTest):
        return (self.role == roleToTest)

    def isChaser(self):
        return (self.role == CHASER)

    def getPosition(self):
        return self.position

    def getPositionCoord(self):
        return Location(self.position.x, self.position.y)

    def equals(self, otherPlay):
        '''compares two plays for equality by value'''
        if( ( self.subRole != otherPlay.subRole ) or
            ( self.role != otherPlay.role ) or
            ( self.formation != otherPlay.formation ) or
            ( self.strategy != otherPlay.strategy ) ):
            return False

        else:
            return True

    def __str__(self):
        return ("Strategy: " + STRATEGIES[self.strategy] +
                "  Formation: " + FORMATIONS[self.formation] +
                "  Role:" + ROLES[self.role] +
                "  SubRole:" + SUB_ROLES[self.subRole])
