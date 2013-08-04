
from . import SoccerFSA
from . import MakeKickStates
from man import motion

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.brain = brain
        self.addStates(MakeKickStates)
        self.setName('pMakeKick')

        self.penaltyKicking = False


    # def executeMove(self,sweetMove):
    #     """
    #     Method to enqueue a SweetMove
    #     Can either take in a head move or a body command
    #     (see SweetMove files for descriptions of command tuples)
    #     """
    #     for position in sweetMove:
    #         if len(position) == 6:
    #             move = motion.BodyJointCommand(position[4], #time
    #                                            position[0], #larm
    #                                            position[1], #lleg
    #                                            position[2], #rleg
    #                                            position[3], #rarm
    #                                            position[5], #interpolation type
    #                                            )
    #             self.brain.motion.enqueue(move)

    #         elif len(position) == 3:
    #             move = motion.HeadJointCommand(position[1],#time
    #                                            position[0],#head pos
    #                                            position[2],#interpolation type
    #                                                )
    #             self.brain.motion.enqueue(move)

    #         else:
    #             self.printf("What kind of sweet ass-Move is this?")
