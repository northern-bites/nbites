
from . import SoccerFSA
from . import LearnMotionStates
from man import motion
from man.motion import StiffnessModes as stiff

MOTION_FRAME_LENGTH = 2


class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.brain = brain
        self.addStates(LearnMotionStates)
        self.setName('pLearnMotion')

    def executeLearnedMove(self, sweetMove):
        """
        Method to enqueue a SweetMove
        Can either take in a head move or a body command
        (see SweetMove files for descriptions of command tuples)
        """

        for position in sweetMove:
            if len(position) == 7:
                move = motion.BodyJointCommand(position[4], #time
                                               position[0], #larm
                                               position[1], #lleg
                                               position[2], #rleg
                                               position[3], #rarm
                                               position[6], # Chain Stiffnesses
                                               position[5], #interpolation type
                                               )

            elif len(position) == 5:
                move = motion.BodyJointCommand(position[2], # time
                                               position[0], # chainID
                                               position[1], # chain angles
                                               position[4], # chain stiffnesses
                                               position[3], # interpolation type
                                               )

            else:
                print("What kind of sweet ass-Move is this?")

        self.brain.motion.enqueue(move)


    def saveMotionFrame(self):
        angles = self.brain.sensors.angles

        self.motionQueue.append(
            (((angles[2], angles[3], angles[4], angles[5]),
              (angles[6],angles[7],angles[8], angles[9],angles[10], angles[11]),
              (angles[12],angles[13],angles[14], angles[15],angles[16], angles[17]),
              (angles[18],angles[19],angles[20],angles[21]), MOTION_FRAME_LENGTH, 0,
              stiff.NORMAL_STIFFNESSES),)
            )

