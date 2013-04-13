import PositionConstants as constants
from math import fabs

####### POSITIONING STUFF ##############
def shouldKickAtPosition(player):
    """
    Is the ball close enough that we should kick
    """
    return (player.brain.ball.vis.on and
            player.brain.ball.dist < constants.AT_POSITION_KICK_DIST)

def leavingTheField(player):
    """
    I am looking off the field and really close to edge!
    """

    fieldEdge = player.brain.vision.fieldEdge
    total = 0
    count = 0
    average = None

    # print "maxDist: " + str(fieldEdge.maxDist)
    # print "shape: " + str(fieldEdge.shape)

    if (fabs(degrees(self.tracker.brain.interface.joints.head_yaw))
        < constants.LOOK_FORWARD_THRESH):
        total = total + fieldEdge.maxDist
        count += 1
        average = total/count

        if (average < constants.CLOSE_TO_FIELD_EDGE):
            return True

    return False
