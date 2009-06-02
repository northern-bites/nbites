"""
Here we house all of the state methods used for kicking the ball
"""

import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
import man.motion.StiffnessModes as StiffnessModes
import KickingConstants as Constants
import ChaseBallConstants
from .. import NogginConstants

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
    if player.kickDecider.sawOwnGoal:
        if Constants.SUPER_SAFE_KICKS:
            return player.goLater('ignoreOwnGoal')

        # We see both posts
        if myLeftPostBearing is not None and myRightPostBearing is not None:
            # Goal in front
            if myRightPostBearing > myLeftPostBearing > 0:
                # kick right
                if Constants.DEBUG_KICKS: print ("\t\tright 1")
                return player.goLater('kickBallRight')
            else:
                # kick left
                if Constants.DEBUG_KICKS: print ("\t\tleft 1")
                return player.goLater('kickBallLeft')

        elif myLeftPostBearing is not None:
            if myLeftPostBearing > 0:
                if Constants.DEBUG_KICKS: print ("\t\tright 2")
                return player.goLater('kickBallRight')
            else:
                if Constants.DEBUG_KICKS: print ("\t\left 2")
                return player.goLater('kickBallLeft')
        elif myRightPostBearing is not None:
            if player.brain.my.h > 0:
                if Constants.DEBUG_KICKS: print ("\t\tright 3")
                return player.goLater('kickBallRight')
            else:
                if Constants.DEBUG_KICKS: print ("\t\tleft 3")
                return player.goLater('kickBallLeft')

            if myRightPostBearing > 0:
                if Constants.DEBUG_KICKS: print ("\t\tleft 4")
                return player.goLater('kickBallLeft')
            else:
                if Constants.DEBUG_KICKS: print ("\t\tright 4")
                return player.goLater('kickBallRight')
        else:
            # don't do anything
            return player.goLater('ignoreOwnGoal')
    # Saw the opponent goal
    elif player.kickDecider.sawOppGoal:
        if oppLeftPostBearing is not None and oppRightPostBearing is not None:
            if oppLeftPostBearing < 0 and oppRightPostBearing > 0:
                # kick straight
                if Constants.DEBUG_KICKS: print ("\t\t Straight 1")
                return player.goLater('kickBallStraight')
            elif oppLeftPostBearing > 0:
                if Constants.DEBUG_KICKS: print ("\t\t Left 5")
                return player.goLater('kickBallLeft')
            elif oppRightPostBearing < 0:
                if Constants.DEBUG_KICKS: print ("\t\t Right 5")
                return player.goLater('kickBallRight')
        elif oppLeftPostBearing is not None:
            if oppLeftPostBearing > 0:
                if Constants.DEBUG_KICKS: print ("\t\t Left 6")
                return player.goLater('kickBallLeft')
            else:
                if Constants.DEBUG_KICKS: print ("\t\t Straight 2")
                return player.goLater('kickBallStraight')
        elif oppRightPostBearing is not None:
            if oppRightPostBearing < 0:
                if Constants.DEBUG_KICKS: print ("\t\t Right 6")
                return player.goLater('kickBallRight')
            else:
                if Constants.DEBUG_KICKS: print ("\t\t Straight 3")
                return player.goLater('kickBallStraight')
        else:
            if Constants.DEBUG_KICKS: print ("\t\t Straight 4")
            return player.goLater('kickBallStraight')

    else:
        # use localization for kick
        if player.brain.my.h < -Constants.MAX_FORWARD_KICK_ANGLE:
            if Constants.DEBUG_KICKS: print "\t\t Left 7"
            return player.goLater('kickBallLeft')
        elif player.brain.my.h > Constants.MAX_FORWARD_KICK_ANGLE:
            if Constants.DEBUG_KICKS: print "\t\t Right 7"
            return player.goLater('kickBallRight')
        else:
            if Constants.DEBUG_KICKS: print "\t\t Straight 5"
            return player.goLater('kickBallStraight')

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
        if player.currentGait != ChaseBallConstants.NORMAL_GAIT:
            player.brain.CoA.setRobotTurnGait(player.brain.motion)
            player.currentGait = ChaseBallConstants.NORMAL_GAIT
        player.brain.tracker.trackBall()
        player.setSteps(0,3.0,0,1)
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

        self.sawOwnGoal = False
        self.sawOppGoal = False

    def collectData(self, info):
        """
        Collect info on any observed goals
        """
        if info.myGoalLeftPost.on:
            self.sawOwnGoal = True
            if info.myGoalLeftPost.certainty == NogginConstants.SURE:
                self.myGoalLeftPostBearings.append(info.myGoalLeftPost.bearing)


        if info.myGoalRightPost.on:
            self.sawOwnGoal = True
            if info.myGoalRightPost.certainty == NogginConstants.SURE:
                self.myGoalRightPostBearings.append(info.myGoalRightPost.bearing)


        if info.oppGoalLeftPost.on:
            self.sawOppGoal = True
            if info.oppGoalLeftPost.certainty == NogginConstants.SURE:
                self.oppGoalLeftPostBearings.append(info.oppGoalLeftPost.bearing)

        if info.oppGoalRightPost.on:
            self.sawOppGoal = True
            if info.oppGoalRightPost.certainty == NogginConstants.SURE:
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
