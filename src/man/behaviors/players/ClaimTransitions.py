# Time elapsed between packets before we deem a claim to be expired
# TODO: determine a reasonable amount of time for this
import math
claimExpiration = 5
headingWeight = 1

def shouldCedeClaim(player):
    playerWeight = weightedDistAndHeading(player.brain.ball.distance, \
                                              player.brain.loc.h, player.brain.ball.bearing_deg)
    for mate in player.brain.teamMembers:
        if (mate.playerNumber == player.brain.playerNumber):
            continue
        if not mate.claimedBall or not mate.active:
            continue

        # Now we get into actual claims
        if ((player.brain.time - mate.claimTime) > claimExpiration):
            print "claim expired"
            continue # That claim has expired (Comm is probably lagging)

        mateWeight = weightedDistAndHeading(mate.ballDist, mate.h, mate.ballBearing)
        # TODO: make weighted function that uses both distance and heading
        if (mateWeight < playerWeight):
            player.claimedBall = False
            return True

        if mate.inKickingState:
            player.claimedBall = False
            return True

    player.claimedBall = True

    return False


def weightedDistAndHeading(distance, heading, ballBearing):
    ballHeading = heading + ballBearing
    if math.fabs(ballHeading) > 90:
        distance += headingWeight * math.fabs(math.sin(math.radians(ballHeading)))
    return distance
