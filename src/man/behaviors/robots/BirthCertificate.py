#from man.motion import SweetMoves as moves

class BirthCertificate:
    '''Defines all the per-robot configurations that may need to be
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
                 doc='', gait=None, dribble_gait=None, backwards_gait=None,
                 slow_gait=None, spin_gait=None, back_standup=None):
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
        self.backwards_gait = backwards_gait
        self.slow_gait = slow_gait
        self.spin_gait = spin_gait
        self.current_gait = None
        self.back_standup = back_standup

    # Remove this functionality from python
    # def setRobotGait(self, motion_interface):
    #     if self.gait is not None and self.current_gait is not self.gait:
    #         print '\033[32m'+ "Birth Certificates - Setting regular gait"+'\033[0m'
    #         self.current_gait = self.gait
    #         motion_interface.setGait(self.gait)

    # def setRobotDribbleGait(self, motion_interface):
    #     if self.dribble_gait is not None and \
    #                 self.current_gait is not self.dribble_gait:
    #         print '\033[32m' + "BirthCertificates - Setting dribble gait" + '\033[0m'
    #         self.current_gait = self.dribble_gait
    #         motion_interface.setGait(self.dribble_gait)

    # def setRobotSlowGait(self, motion_interface):
    #     if self.slow_gait is not None and \
    #                 self.current_gait is not self.slow_gait:
    #             print '\033[32m' + "BirthCertificates - Setting slow gait" + '\033[0m'
    #             self.current_gait = self.slow_gait
    #             motion_interface.setGait(self.slow_gait)

    # Remove this functionality from python
    # def getBackStandup(self):
    #     if self.back_standup is None:
    #         return moves.STAND_UP_BACK
    #     else:
    #         return self.back_standup

    # Remove this functionality from python
    # def setRobotSpinGait(self, motion_interface):
    #     if self.spin_gait is not None and \
    #             self.current_gait is not self.spin_gait:
    #         print '\033[32m' + "BirthCertificates - Setting spin gait" + '\033[0m'
    #         self.current_gait = self.spin_gait
    #         motion_interface.setGait(self.spin_gait)

    # def setRobotBackwardsGait(self, motion_interface):
    #     if self.backwards_gait is not None and \
    #             self.current_gait is not self.backwards_gait:
    #         print '\033[32m' + "BirthCertificates - Setting backwards gait" + '\033[0m'
    #         self.current_gait = self.backwards_gait
    #         motion_interface.setGait(self.backwards_gait)

    # def setRobotDynamicGait(self, motion_interface, dynGait):
    #     print '\033[32m' + "BirthCertificates - Setting DYNAMIC gait" + '\033[0m'
    #     self.current_gait = dynGait
    #     motion_interface.setGait(dynGait)

    # def __str__(self):
    #     s = "CoA: " + self.long_name
    #     if self.__doc__:
    #         s += '\nCoA: ' + self.__doc__
    #     return s

