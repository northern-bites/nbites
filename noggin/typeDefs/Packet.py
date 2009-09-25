from ..NogginConstants import NUM_PACKET_ELEMENTS

class Packet:
    """
    Class for one single packet, contains:
    -teamHeader -- header string for nBites (should only receive our own)
    -teamNumber -- team number (should only receive our own)
    -playerNumber -- player number of the owner of the packet
    -timeStamp -- milliseconds since gameController switched state to STATE_PLAY
    -playerX -- x in cm of the packet owner's position on the field
    -playerY -- y in cm of the packet owner's position on the field
    -playerH -- heading of packet owner in degrees, range == [-179..0..180]
    -uncertX -- uncertainty in x direction, self
    -uncertY -- uncertainty in y direction, self
    -uncertH -- uncertainty in heading, self
    -ballX -- estimate of ball x coordinate in cm on field
    -ballY -- estimate of ball y coordinate in cm on field
    -ballUncertX -- estimate of uncertainty in x-axis of the ball's position
    -ballUncertY -- estimate of uncertainty in y-axis of the ball's position
    -ballDist -- distance from player to ball in cm.  set to special values for
    kicking, grabbing, etc
    -role -- current role used in special situations in playbook and Coop
    -subrole -- current sub role used in special situations in playbook and Coop

    can pass packet list into constructor, else it'll default to all zeros
    """
    def __init__(self,new_packet=[0]*NUM_PACKET_ELEMENTS):
        (self.teamNumber,
         self.playerNumber,
         self.color,
         self.playerX,
         self.playerY,
         self.playerH,
         self.uncertX,
         self.uncertY,
         self.uncertH,
         self.ballX,
         self.ballY,
         self.ballUncertX,
         self.ballUncertY,
         self.ballDist,
         self.role,
         self.subRole,
         self.chaseTime,
         self.ballVelX,
         self.ballVelY) = new_packet

        self.teamNumber = int(self.teamNumber)
        self.playerNumber = int(self.playerNumber)
        self.color = int(self.color)
        self.playerX = float(self.playerX)
        self.playerY = float(self.playerY)
        self.playerH = float(self.playerH)
        self.uncertX = float(self.uncertX)
        self.uncertY = float(self.uncertY)
        self.uncertH = float(self.uncertH)
        self.ballX = float(self.ballX)
        self.ballY = float(self.ballY)
        self.ballUncertX = float(self.ballUncertX)
        self.ballUncertY = float(self.ballUncertY)
        self.ballDist = float(self.ballDist)
        self.role = float(self.role)
        self.subRole = float(self.subRole)
        self.chaseTime = float(self.chaseTime)
        self.ballVelX = float(self.ballVelX)
        self.ballVelY = float(self.ballVelY)

    def update(self,update_packet=[0]*NUM_PACKET_ELEMENTS):
        """
        update packet with a list of new values
        """
        # error checking if passed list is not right size
        if len(update_packet) != NUM_PACKET_ELEMENTS:
            raise ValueError("Packet().update got list of size:" +
                     str(len(update_packet)) + " but should have size " +
                             NUM_PACKET_ELEMENTS)
        # else, update values
        (self.teamNumber,
         self.playerNumber,
         self.color,
         self.playerX,
         self.playerY,
         self.playerH,
         self.uncertX,
         self.uncertY,
         self.uncertH,
         self.ballX,
         self.ballY,
         self.ballUncertX,
         self.ballUncertY,
         self.ballDist,
         self.role,
         self.calledSubRole,
         self.chaseTime,
         self.ballVelX,
         self.ballVelY) = update_packet

    def __str__(self):
        """returns string with all important values"""
        return ("#%d role: %d subRole: %d chaseTime: %g; loc: (%g,%g,%g) uncert: (%g,%g,%g) ball - loc: (%g,%g) uncert: (%g,%g) dist: (%g)" %
            (self.playerNumber,
             self.role, self.calledSubRole, self.chaseTime,
             self.playerX, self.playerY, self.playerH,
             self.uncertX, self.uncertY, self.uncertH,
             self.ballX, self.ballY, self.ballUncertX,
             self.ballUncertY, self.ballDist, self.ballVelX,
             self.ballVelY))
