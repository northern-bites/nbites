# Time elapsed between packets before we deem a claim to be expired
# TODO: determine a reasonable amount of time for this
import math
import RoleConstants as roleConstants

claimExpiration = 5
widthOfField = 600
lengthOfField = 900

def shouldCedeClaim(player):
    """
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
                player.roleOfClaimer =  mate.role
                player.claimedBall = False
                return True
            elif player.role < mate.role and not roleConstants.isFirstChaser(player.role):
                player.roleOfClaimer =  mate.role
                player.claimedBall = False
                return True
        elif (mateWeight < playerWeight):
            player.roleOfClaimer =  mate.role
            player.claimedBall = False
            return True

    player.claimedBall = True

    """
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


def ballY(my_y,my_h, ballBearing,distance):
    ballHeading = my_h + ballBearing
    ball_y_to_me = distance * math.fabs(math.sin(math.radians(ballHeading)))
    ball_y =  ball_y_to_me + my_y
    return ball_y

# The function calculates the distance between your mate and the ball based
# on the mandatory information given for dropin
def mateBallDist(my_x, my_y, my_h, distance, ballBearing, mate_x, mate_y): 
    ballHeading = my_h + ballBearing
    ball_x_to_me = distance * math.cos(math.radians(ballHeading))
    ball_x = ball_x_to_me + my_x
    ball_y = ballY(my_y,my_h, ballBearing, distance)
    ball_dist_to_mate =((ball_x - mate_x)**2 + (ball_y - mate_y)**2)**0.5
    return ball_dist_to_mate

# The function calculates the ball heading for the mate
def mateBallHeading(ball_y, mate_y, mate_h, distance):
    if distance == 0.0:
        mate_ballHeading = mate_h
    else:
        dy = mate_y - ball_y
        ball_dist_heading = math.degrees(math.asin(float(dy)/float(distance)))
        mate_ballHeading = mate_h - ball_dist_heading
    return mate_ballHeading
