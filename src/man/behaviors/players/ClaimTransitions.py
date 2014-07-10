# Time elapsed between packets before we deem a claim to be expired
# TODO: determine a reasonable amount of time for this
import math
claimExpiration = 5
headingWeight = .5
#TODO 75?
claimDistance = 7500000
#TODO play with this
SMALL_WEIGHT_DIFFERENCE = 25
SMALL_WEIGHT_DIFFERENCE_FAR = 75

FAR_DIST = 150

def shouldCedeClaim(player):
    if not player.useClaims:
        return False

    playerWeight = weightedDistAndHeading(player.brain.ball.distance, \
                                              player.brain.loc.h, player.brain.ball.bearing_deg)
    for mate in player.brain.teamMembers:
        if (mate.playerNumber == player.brain.playerNumber):
            continue
        if not mate.claimedBall or not mate.active or mate.fallen:
            continue

        # Now we get into actual claims
        if ((player.brain.time - mate.claimTime) > claimExpiration):
            print "claim expired"
            continue # That claim has expired (Comm is probably lagging)

        mateWeight = weightedDistAndHeading(mate.ballDist, mate.h, mate.ballBearing)

        # sigmoid function so that the difference increases slowly at close distances but
        # grows quickly at mid-range to far distances and at very far distances, asymptotically
        # approaches a maximum
        closeWeightDifference = 30 + 150/(1 + math.e**(6.25 - .05*player.brain.ball.distance))
        # if ((math.fabs(mateWeight - playerWeight) < SMALL_WEIGHT_DIFFERENCE and 
        #     player.brain.ball.distance < FAR_DIST) or 
        #     (math.fabs(mateWeight - playerWeight) < SMALL_WEIGHT_DIFFERENCE_FAR and
        #     player.brain.ball.distance >= FAR_DIST)):
        if (math.fabs(mateWeight - playerWeight) < closeWeightDifference):
            if player.role < mate.role:
                player.claimedBall = False
                return True

        # TODO: think more about comm lag/check comm lag
        elif (mateWeight < playerWeight):
            if mate.ballDist < claimDistance:
                player.claimedBall = False
                return True

        elif mate.inKickingState:
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
    #TODO
    if math.fabs(ballHeading) > 90:
        distance += distance * headingWeight * math.fabs(math.cos(math.radians(ballHeading)))
    return distance
