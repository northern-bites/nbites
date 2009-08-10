import GoTeam

class PBInterface:
    "
    This is the class that provides access to information about the playbook to
    any outside class
    "
    def __init__(self, brain):
        '''
        Initializes the playbook
        '''
        self.pb = GoTeam.GoTeam(brain)

    def run(self):
        '''
        Runs the playbook (calls the run method of GoTeam)
        '''
        self.pb.run()

    def getSubRole(self):
        '''
        returns the current subRole of the player
        '''
        return self.pb.subRole

    def getRole(self):
        '''
        returns the current role of the player
        '''
        return self.pb.role

    def getFormation(self):
        '''
        return the current formation of the player
        '''
        return self.pb.formation

    def getStrategy(self):
        '''
        return the current strategy of the player
        '''
        return self.pb.strategy

    def isRoleChaser(self):
        '''
        true if player's assigned role is chaser
        '''
        return self.pb.me.isChaser()

    def isRoleMiddie(self):
        '''
        true if player's assigned role is middie
        '''
        return self.pb.me.isMiddie()

    def isRoleDefender(self):
        '''
        true if player's assigned role is defender
        '''
        return self.pb.me.isDefender()

    def isRoleGoalie(self):
        '''
        true if player's assigned role is defender
        '''
        return self.pb.me.isGoalie()

    def isDefaultChaser(self):
        '''
        true if player is chaser at game start
        '''
        return self.pb.me.isDefaultChaser()

    def isDefaultGoalie(self):
        '''true if player is goalie at game start'''
        return self.pb.me.isDefaultGoalie()
