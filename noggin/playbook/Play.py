from . import PBConstants

class Play:
    def __init__(self,
                 strategy = PBConstants.INIT_STRATEGY,
                 formation = PBConstants.INIT_FORMATION,
                 role = PBConstants.INIT_ROLE,
                 subRole = PBConstants.INIT_SUB_ROLE,
                 position = [0,0]
                 ):
        self.strategy = strategy
        self.formation = formation
        self.role = role
        self.subRole = subRole
        self.position = position

    def setStrategy(self, strategy):
        self.strategy = strategy

    def setFormation(self, formation):
        self.formation = formation

    def setRole(self, role):
        self.role = role

    def setSubRole(self, subRole):
        self.subRole = subRole

    def setPosition(self, position):
        self.position = position

    def getStrategy(self):
        return self.strategy

    def getFormation(self):
        return self.formation

    def getRole(self):
        return self.role

    def getSubRole(self):
        return self.subRole

    def getPosition(self):
        return self.position

    def equals(self, otherPlay):
        '''compares two plays for equality by value'''
        if( ( self.subRole != otherPlay.getSubRole() ) or
            ( self.role != otherPlay.getRole() ) or
            ( self.formation != otherPlay.getFormation() ) or
            ( self.strategy != otherPlay.getStrategy() ) ):
            return False

        else:
            return True

    def __str__(self):
        return ("Strategy: " + PBConstants.STRATEGIES[self.strategy] +
                "  Formation: " + PBConstants.FORMATIONS[self.formation] +
                "  Role:" + PBConstants.ROLES[self.role] +
                "  SubRole:" + PBConstants.SUB_ROLES[self.subRole])
