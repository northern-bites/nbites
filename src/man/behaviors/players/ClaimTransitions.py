# Time elapsed between packets before we deem a claim to be expired
# TODO: determine a reasonable amount of time for this
import math
claimExpiration = 5
headingWeight = .5
claimDistance = 50

def shouldCedeClaim(player):
    if not player.useClaims:
        return False
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
        # TODO: think more about comm lag/check comm lag
        if (mateWeight < playerWeight):
            if mate.ballDist < claimDistance:
                player.claimedBall = False
                return True

        if mate.inKickingState:
            if mate.ballDist < claimDistance:
                player.claimedBall = False
                return True

    player.claimedBall = True

    return False

#TODO: make this make use of amount of orbit necessary
def weightedDistAndHeading(distance, heading, ballBearing):
    if heading > 180:
        heading -= 360
    if heading < -180:
        heading += 360

    ballHeading = heading + ballBearing
    if math.fabs(ballHeading) > 90:
        distance += distance * headingWeight * math.fabs(math.cos(math.radians(ballHeading)))
    return distance
