

class BirthCertificate:
    '''Defines all the per-robot configuraations that may need to be
    defined.  Each robot has an instance created in the robots package
    initialization module.  Holds information like name, walk config,
    etc..  Each new instance will receive an id number, but these are
    dependent on the order of initialization (usually alphabetical
    order) so should not be used for comparison.  Use
      if brain.robot is robots.slarti
    for example.'''

    # Class-static variable.  Increments with each new instance
    next_id = 0

    def __init__(self, name, long_name, tts_name=None,
                  doc='', gait=None, dribble_gait=None, slow_gait=None):
        self.id = BirthCertificate.next_id
        BirthCertificate.next_id += 1

        self.name = name
        self.long_name = long_name
        self.tts_name = name
        if tts_name is not None:
            self.tts_name = tts_name
        self.__doc__ = doc
        self.gait = gait
        self.dribble_gait = dribble_gait
        self.slow_gait = slow_gait

    def setRobotGait(self, motion_interface):
        if self.gait is not None:
            print '\033[32m'+ "BirthCertificates - Setting regular gait"+'\033[0m'
            motion_interface.setGait(self.gait)

    def setRobotDribbleGait(self, motion_interface):
        if self.dribble_gait is not None:
            print '\033[32m' + "BirthCertificates - Setting dribble gait" + '\033[0m'
            motion_interface.setGait(self.dribble_gait)
    def setRobotSlowGait(self, motion_interface):
        if self.slow_gait is not None:
            print '\033[32m' + "BirthCertificates - Setting slow gait" + '\033[0m'
            motion_interface.setGait(self.slow_gait)

    def __str__(self):
        s = "CoA: " + self.long_name
        if self.__doc__:
            s += '\nCoA:   ' + self.__doc__
        return s

