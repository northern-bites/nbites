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
        self.role = None
        self.lastRole = None

    def update(self, play):
        '''
        Runs the playbook (calls the run method of GoTeam)
        '''
        self.pb.run(play)
        self.lastSubRole = self.subRole
        self.lastRole = self.role
        self.subRole = play.subRole
        self.role = play.role

    def subRoleUnchanged(self):
        return (self.subRole == self.lastSubRole)

    def roleChanged(self):
        return (self.role!= self.lastRole)
