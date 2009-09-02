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
        #self.role = None
        #self.subRole = None

    def run(self):
        '''
        Runs the playbook (calls the run method of GoTeam)
        '''
        self.pb.run()
        #self.role = self.pb.play.getRole()
        #self.subRole = self.pb.play.getSubRole()

    def isDefaultChaser(self):
        ''' true if player is chaser at game start '''
        return self.pb.me.isDefaultChaser()

    def isDefaultGoalie(self):
        '''true if player is goalie at game start'''
        return self.pb.me.isDefaultGoalie()

    def isSubRole(self, subRoleToTest):
        '''true if player is the given subrole'''
        return (self.pb.play.getSubRole() == subRoleToTest)

    def isRole(self, roleToTest):
        '''true if player is the given role'''
        return (self.pb.play.getRole() == roleToTest)

    def getPosition(self):
        return self.pb.play.getPosition()

    def subRoleChanged(self):
        return (self.pb.play.getSubRole() != self.pb.lastPlay.getSubRole())
