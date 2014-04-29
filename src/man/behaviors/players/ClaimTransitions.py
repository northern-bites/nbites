# Time elapsed between packets before we deem a claim to be expired
# TODO: determine a reasonable amount of time for this
claimExpiration = 10


def shouldCedeClaim(player):
    for mate in player.brain.teamMembers:
        if (mate.playerNumber == player.brain.playerNumber):
            continue
        if not mate.claimedBall or not mate.active:
            continue

        print "mate number: ", mate.playerNumber, " has claimed the ball"
        # Now we get into actual claims
        if (int(player.brain.time * 1000) - mate.timestamp > claimExpiration):
            continue # That claim has expired (Comm is probably lagging)

        # TODO: make weighted function that uses both distance and heading
        if (mate.ball_dist < player.brain.ball.distance):
            player.brain.claimedBall = False
            return True

        if mate.in_kicking_state:
            player.brain.claimedBall = False
            return True

    return False
