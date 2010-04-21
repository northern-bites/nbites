from . import GoTeam

class PBInterface:
    '''
    This is the class that provides access to information about the playbook
    to any outside class
    '''
    def __init__(self, brain):
        '''
        Initializes the playbook
        '''
        self.pb = GoTeam.GoTeam(brain)
        self.subRole = None
        self.lastSubRole = None

    def update(self):
        '''
        Runs the playbook (calls the run method of GoTeam)
        '''
        self.pb.run()
        self.storeUsedValues()
        return self.pb.play

    def subRoleChanged(self):
        return (self.subRole != self.lastSubRole)

    def storeUsedValues(self):
        self.lastSubRole = self.subRole
        self.subRole = self.pb.play.subRole
