"""
Here we house all of the state methods used for kicking the ball
"""

import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
import man.motion.StiffnessModes as StiffnessModes
import KickingConstants as constants
import ChaseBallConstants
from math import fabs

class KickDecider:
    """
    Class to hold all the things we need to decide a kick
    """

    def __init__(self):
        self.oppGoalLeftPostBearings = []
        self.oppGoalRightPostBearings = []
        self.myGoalLeftPostBearings = []
        self.myGoalRightPostBearings = []

        self.oppLeftPostBearing = None
        self.oppRightPostBearing = None
        self.myLeftPostBearing = None
        self.myRightPostBearing = None

    def collectData(self, info):
        """
        Collect info on any observed goals
        """
        if info.myGoalLeftPost.on:
            self.myGoalLeftPostBearings.append(info.myGoalLeftPost.bearing)
        if info.myGoalRightPost.on:
            self.myGoalRightPostBearings.append(info.myGoalRightPost.bearing)
        if info.oppGoalLeftPost.on:
            self.oppGoalLeftPostBearings.append(info.oppGoalRightPost.bearing)
        if info.oppGoalRightPost.on:
            self.oppGoalRightPostBearings.append(info.oppGoalRightPost.bearing)

    def calculate(self):
        """
        Get usable data from the collected data
        """
        if len(self.myGoalLeftPostBearings) > 0:
            self.myLeftPostBearing = (sum(self.myGoalLeftPostBearings) /
                                      len(self.myGoalLeftPostBearings))
        if len(self.myGoalRightPostBearings) > 0:
            self.myRightPostBearing = (sum(self.myGoalRightPostBearings) /
                                       len(self.myGoalRightPostBearings))
        if len(self.oppGoalLeftPostBearings) > 0:
            self.oppLeftPostBearing = (sum(self.oppGoalLeftPostBearings) /
                                       len(self.oppGoalLeftPostBearings))
        if len(self.oppGoalRightPostBearings) > 0:
            self.oppRightPostBearing = (sum(self.oppGoalRightPostBearings) /
                                        len(self.oppGoalRightPostBearings))

    def sawOwnGoal(self):
        return (len(self.myGoalLeftPostBearings) > 0 or
                len(self.myGoalRightPostBearings) > 0)

    def sawOppGoal(self):
        return (len(self.oppGoalLeftPostBearings) > 0 or
                len(self.oppGoalRightPostBearings) > 0)


    def __str__(self):
        s = ""
        if self.myLeftPostBearing is not None:
            s += ("My left post bearing is: " + str(self.myLeftPostBearing) + "\n")
        if self.myRightPostBearing is not None:
            s += ("My right post bearing is: " + str(self.myRightPostBearing) + "\n")
        if self.oppLeftPostBearing is not None:
            s += ("Opp left post bearing is: " + str(self.oppLeftPostBearing) + "\n")
        if self.oppRightPostBearing is not None:
            s += ("Opp right post bearing is: " + str(self.oppRightPostBearing) + "\n")
        if s == "":
            s = "No goal posts observed"
        return s

def decideKick(player):
    """
    Decides which kick to use
    """
    if player.firstFrame():
        player.brain.tracker.switchTo('stopped')
        player.brain.motion.stopHeadMoves()
        player.executeMove(HeadMoves.KICK_SCAN)

        player.kickDecider = KickDecider()

    # If scanning, then collect data
    if (player.stateTime < SweetMoves.getMoveTime(HeadMoves.KICK_SCAN)):
        player.kickDecider.collectData(player.brain)
        return player.stay()

    # Done scanning time to act
    player.brain.tracker.trackBall()
    player.kickDecider.calculate()

    # Get references to the collected data
    myLeftPostBearing =  player.kickDecider.myLeftPostBearing
    myRightPostBearing = player.kickDecider.myRightPostBearing
    oppLeftPostBearing = player.kickDecider.oppLeftPostBearing
    oppRightPostBearing = player.kickDecider.oppRightPostBearing

    player.printf(player.kickDecider)

    # Things to do if we saw our own goal
    if player.kickDecider.sawOwnGoal():
        if constants.SUPER_SAFE_KICKS:
            return player.goLater('ignoreOwnGoal')

        # We see both posts
        if myLeftPostBearing is not None and myRightPostBearing is not None:
#             if player.brain.my.h > 0:
#                 return player.goLater('kickBallRight')
#             else:
#                 return player.goLater('kickBallLeft')
            # Goal in front
            if myRightPostBearing > myLeftPostBearing > 0:
                # kick right
                return player.goLater('kickBallRight')
            else:
                # kick left
                return player.goLater('kickBallLeft')
        elif oppLeftPostBearing is not None and oppRightPostBearing is not None:
            if oppLeftPostBearing < 0 and oppRightPostBearing > 0:
                # kick straight
                return player.goLater('kickBallStraight')
            else:
                if fabs(oppLeftPostBearing) > fabs(oppRightPostBearing):
                    # kick left
                    return player.goLater('kickBallLeft')
                else:
                    # kick right
                    return player.goLater('kickBallRight')

        elif myLeftPostBearing is not None:
#             if player.brain.my.h > 0:
#                 return player.goLater('kickBallRight')
#             else:
#                 return player.goLater('kickBallLeft')

            if myLeftPostBearing > 0:
                return player.goLater('kickBallRight')
            else:
                return player.goLater('kickBallLeft')
        elif myRightPostBearing is not None:
            if player.brain.my.h > 0:
                return player.goLater('kickBallRight')
            else:
                return player.goLater('kickBallLeft')

            if myRightPostBearing > 0:
                return player.goLater('kickBallLeft')
            else:
                return player.goLater('kickBallRight')

        # don't do anything
        return player.goLater('ignoreOwnGoal')
    elif player.kickDecider.sawOppGoal():

        if oppLeftPostBearing is not None and oppRightPostBearing is not None:
            if oppLeftPostBearing < 0 and oppRightPostBearing > 0:
                # kick straight
                return player.goLater('kickBallStraight')
            else:
                if player.brain.my.h > 0:
                    return player.goLater('kickBallRight')
                else:
                    return player.goLater('kickBallLeft')
        else:
            if player.brain.my.h < -65:
                return player.goLater('kickBallLeft')
            elif player.brain.my.h > 65:
                return player.goLater('kickBallRight')
            else:
                return player.goLater('kickBallStraight')
        # kick straight
        return player.goLater('kickBallStraight')
    else:
        # use localization for kick
        return player.goLater('kickBallStraight')

def afterKick(player):
    """
    State to follow up after a kick.
    Currently exits after one frame.
    """
    # trick the robot into standing up instead of leaning to the side
    player.walkPose()

    if player.brain.nav.isStopped():
        player.brain.CoA.setRobotGait(player.brain.motion)
        player.currentGait = ChaseBallConstants.FAST_GAIT

        return player.goLater("chase")
    else:
        return player.stay()

def kickBallStraight(player):
    """
    Kick the ball forward.  Currently uses the left foot
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.printf("We should kick straight!", 'cyan')
    if player.counter == 2:
        player.executeMove(SweetMoves.LEFT_FAR_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.LEFT_FAR_KICK):
        return player.goNow("afterKick")

    return player.stay()

def kickBallLeft(player):
    """
    Strafe in order to line up to kick the ball to the right
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.setSpeed(0,1.5,0)
    elif player.counter == 10:
        player.stopWalking()
    elif player.brain.nav.isStopped():
        return player.goLater('kickBallLeftExecute')
    return player.stay()

def kickBallRight(player):
    """
    Kick the ball to the right, using the left foot
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.printf("We should kick right!", 'cyan')
    if player.counter == 2:
        # Left side kick, means the sideways kick with the left foot
        # Kicks the ball to the right
        player.executeMove(SweetMoves.LEFT_SIDE_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.LEFT_SIDE_KICK):
        return player.goNow("afterKick")

    return player.stay()

def kickBallLeftExecute(player):
    """
    Kicks the ball to the left, using the right foot
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.printf("We should kick left!", 'cyan')
    if player.counter == 2:
        # Right side kick, means the sideways kick with the right foot
        # Kicks the ball to the left
        player.executeMove(SweetMoves.RIGHT_SIDE_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.RIGHT_SIDE_KICK):
        return player.goNow("afterKick")

    return player.stay()

def kickAtPosition(player):
    """
    Method to simply kick while standing in position
    Used for a very simple goalie behavior
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.executeStiffness(StiffnessModes.LEFT_FAR_KICK_STIFFNESS)
    if player.counter == 2:
        player.executeMove(SweetMoves.LEFT_FAR_KICK)

    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.LEFT_FAR_KICK):
        player.walkPose()

        if player.brain.nav.isStopped():
            player.brain.CoA.setRobotGait(player.brain.motion)
            player.currentGait = ChaseBallConstants.FAST_GAIT
            return player.goLater('atPosition')

    return player.stay()
