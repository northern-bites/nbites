from ..playbook.PBConstants import (INIT_STRATEGY,
                                    INIT_FORMATION,
                                    INIT_ROLE,
                                    INIT_SUB_ROLE,
                                    STRATEGIES,
                                    FORMATIONS,
                                    ROLES,
                                    SUB_ROLES)
from Location import RobotLocation

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
        self.position = RobotLocation(0,0,0)

    def setStrategy(self, strategy):
        self.strategy = strategy

    def setFormation(self, formation):
        self.formation = formation

    def setRole(self, role):
        self.role = role

    def setSubRole(self, subRole):
        self.subRole = subRole

    def setPosition(self, position):
        self.position.x = position[0]
        self.position.y = position[1]
        self.position.h = position[2]

    def isSubRole(self, subRoleToTest):
        return (self.subRole == subRoleToTest)

    def isRole(self, roleToTest):
        return (self.role == roleToTest)

    def getPosition(self):
        return self.position

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
