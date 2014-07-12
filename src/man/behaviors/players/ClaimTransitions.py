# Time elapsed between packets before we deem a claim to be expired
# TODO: determine a reasonable amount of time for this
import math
import RoleConstants as roleConstants

claimExpiration = 5

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
        # approaches a maximum. uses the distance of the close robot
        if player.brain.ball.distance < mate.ballDist:
            closerDistance = player.brain.ball.distance
        else:
            closerDistance = mate.ballDist
        closeWeightDifference = 25 + 150/(1 + math.e**(6.25 - .05*closerDistance))
        if (math.fabs(mateWeight - playerWeight) < closeWeightDifference):
            if roleConstants.isFirstChaser(mate.role):
                player.claimedBall = False
                return True
            elif player.role < mate.role and not roleConstants.isFirstChaser(player.role):
                player.claimedBall = False
                return True
        elif (mateWeight < playerWeight):
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
    if ballHeading > 90:
        distance += distance * (ballBearing-90)**2 / 90**2
    elif ballHeading < -90:
        distance += distance * (ballBearing+90)**2 / 90**2
    return distance
