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
        self.role = None
        self.lastRole = None

    def update(self, play):
        '''
        Runs the playbook (calls the run method of GoTeam)
        '''
        self.pb.run(play)
        self.lastRole = self.role
        self.role = play.role

    def roleUnchanged(self):
        return (self.role == self.lastRole)

    def roleChanged(self):
        return not self.roleUnchanged()
