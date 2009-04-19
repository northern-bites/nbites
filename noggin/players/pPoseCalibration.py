
from . import SoccerFSA
from . import PoseCalibrationStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(CalibrationSnapshotStates)
        self.setName('pPoseCalibration')
