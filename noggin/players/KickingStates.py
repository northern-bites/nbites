"""
Here we house all of the state methods used for kicking the ball
"""

import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
import man.motion.StiffnessModes as StiffnessModes
import KickingConstants as constants
from math import fabs

def decideKick(player):
    """
    Decides which kick to use
    """
    if player.firstFrame():
        player.stopWalking()
        player.currentChaseWalkX = 0
        player.currentChaseWalkY = 0
        player.currentChaseWalkTheta = 0
        player.sawOwnGoal = False
        player.sawOppGoal = False
        player.brain.tracker.switchTo('stopped')
        player.brain.motion.stopHeadMoves()
        player.executeMove(HeadMoves.KICK_SCAN)
        player.oppGoalLeftPostBearings = []
        player.oppGoalRightPostBearings = []
        player.myGoalLeftPostBearings = []
        player.myGoalRightPostBearings = []

    if (player.stateTime < SweetMoves.getMoveTime(HeadMoves.KICK_SCAN)):
        if player.brain.myGoalLeftPost.on:
            player.sawOwnGoal = True
            player.myGoalLeftPostBearings.append(player.brain.myGoalLeftPost.bearing)
        if player.brain.myGoalRightPost.on:
            player.sawOwnGoal = True
            player.myGoalRightPostBearings.append(player.brain.myGoalRightPost.bearing)
        if player.brain.oppGoalLeftPost.on:
            player.sawOppGoal = True
            player.oppGoalLeftPostBearings.append(player.brain.oppGoalRightPost.bearing)
        if player.brain.oppGoalRightPost.on:
            player.sawOppGoal = True
            player.oppGoalRightPostBearings.append(player.brain.oppGoalRightPost.bearing)
        return player.stay()
    elif player.sawOwnGoal:
        player.brain.tracker.trackBall()
        if constants.SUPER_SAFE_KICKS:
            return player.goLater('ignoreOwnGoal')
        myLeftPostBearing = None
        myRightPostBearing = None
        oppLeftPostBearing = None
        oppRightPostBearing = None
        if len(player.myGoalLeftPostBearings) > 0:
            myLeftPostBearing = (sum(player.myGoalLeftPostBearings) /
                                 len(player.myGoalLeftPostBearings))
        if len(player.myGoalRightPostBearings) > 0:
            myRightPostBearing = (sum(player.myGoalRightPostBearings) /
                                  len(player.myGoalRightPostBearings))
        if len(player.oppGoalLeftPostBearings) > 0:
            oppLeftPostBearing = (sum(player.oppGoalLeftPostBearings) /
                                 len(player.oppGoalLeftPostBearings))
        if len(player.oppGoalRightPostBearings) > 0:
            oppRightPostBearing = (sum(player.oppGoalRightPostBearings) /
                                   len(player.oppGoalRightPostBearings))

        if myLeftPostBearing is not None:
            player.printf("My left post bearing is: " + str(myLeftPostBearing),'cyan')
        if myRightPostBearing is not None:
            player.printf("My right post bearing is: " + str(myRightPostBearing),'cyan')
        if oppLeftPostBearing is not None:
            player.printf("Opp left post bearing is: " + str(oppLeftPostBearing),'cyan')
        if oppRightPostBearing is not None:
            player.printf("Opp right post bearing is: " + str(oppRightPostBearing),'cyan')

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
    elif player.sawOppGoal:
        return player.goLater('kickBallStraight')
        myLeftPostBearing = None
        myRightPostBearing = None
        oppLeftPostBearing = None
        oppRightPostBearing = None
        player.brain.tracker.trackBall()
        if len(player.myGoalLeftPostBearings) > 0:
            myLeftPostBearing = (sum(player.myGoalLeftPostBearings) /
                                 len(player.myGoalLeftPostBearings))
        if len(player.myGoalRightPostBearings) > 0:
            myRightPostBearing = (sum(player.myGoalRightPostBearings) /
                                  len(player.myGoalRightPostBearings))
        if len(player.oppGoalLeftPostBearings) > 0:
            oppLeftPostBearing = (sum(player.oppGoalLeftPostBearings) /
                                 len(player.oppGoalLeftPostBearings))
        if len(player.oppGoalRightPostBearings) > 0:
            oppRightPostBearing = (sum(player.oppGoalRightPostBearings) /
                                   len(player.oppGoalRightPostBearings))
        if myLeftPostBearing is not None:
            player.printf("My left post bearing is: " + str(myLeftPostBearing),'cyan')
        if myRightPostBearing is not None:
            player.printf("My right post bearing is: " + str(myRightPostBearing),'cyan')
        if oppLeftPostBearing is not None:
            player.printf("Opp left post bearing is: " + str(oppLeftPostBearing),'cyan')
        if oppRightPostBearing is not None:
            player.printf("Opp right post bearing is: " + str(oppRightPostBearing),'cyan')

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

    return player.goLater("chase")

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
