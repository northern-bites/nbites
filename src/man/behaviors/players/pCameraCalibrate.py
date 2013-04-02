
from . import SoccerFSA
from . import CameraCalibrateStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(CameraCalibrateStates)
        self.setName('pCameraCalibrate')
