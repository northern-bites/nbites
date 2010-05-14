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

    def update(self, play):
        '''
        Runs the playbook (calls the run method of GoTeam)
        '''
        self.pb.run(play)
        self.storeUsedValues(play)

    def subRoleChanged(self):
        return (self.subRole != self.lastSubRole)

    def storeUsedValues(self, play):
        self.lastSubRole = self.subRole
        self.subRole = play.subRole
