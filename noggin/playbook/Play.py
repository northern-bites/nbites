class Play:
    def __init__(self,
                 strategy = PBConstants.INIT_STRATEGY,
                 formation = PBConstants.INIT_FORMATION,
                 role = PBConstants.INIT_ROLE,
                 subRole = PBConstants.INIT_SUBROLE,
                 position = [0,0]
                 ):
        self.strategy = strategy
        self.formation = formation
        self.role = role
        self.subRole = subRole
        self.position = position

    def setStrategy(strategy):
        self.strategy = strategy

    def setFormation(formation):
        self.formation = formation

    def setRole(role):
        self.role = role

    def setSubRole(subRole):
        self.subRole = subRole

    def setPosition(position):
        self.position = position

    def __str__(self):
        return "Strategy: " + self.strategy + "  Formation: " + self.formation\
            + "  Role:" + self.role + "  SubRole:" + self.subRole
