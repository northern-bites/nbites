import GoTeam

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
        self.defaultChaser = self.pb.me.isDefaultChaser()
        self.defaultGoalie = self.pb.me.isDefaultGoalie()

    def update(self):
        '''
        Runs the playbook (calls the run method of GoTeam)
        '''
        self.pb.run()
        self.storeUsedValues()
        return self.pb.play

    def isDefaultChaser(self):
        ''' true if player is chaser at game start '''
        return self.defaultChaser

    def isDefaultGoalie(self):
        '''true if player is goalie at game start'''
        return self.defaultGoalie

    def getPosition(self):
        return self.position

    def subRoleChanged(self):
        return (self.subRole != self.lastSubRole)

    def storeUsedValues(self):
        play = self.pb.play
        self.lastSubRole = self.subRole
        self.subRole = play.subRole
