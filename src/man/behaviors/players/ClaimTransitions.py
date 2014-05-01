# Time elapsed between packets before we deem a claim to be expired
# TODO: determine a reasonable amount of time for this
claimExpiration = 5


def shouldCedeClaim(player):
    for mate in player.brain.teamMembers:
        if (mate.playerNumber == player.brain.playerNumber):
            continue
        if not mate.claimedBall or not mate.active:
            continue

        # Now we get into actual claims
        if ((player.brain.time - mate.claimTime) > claimExpiration):
            print "claim expired"
            continue # That claim has expired (Comm is probably lagging)

        # TODO: make weighted function that uses both distance and heading
        if (mate.ballDist < player.brain.ball.distance):
            player.claimedBall = False
            return True

        if mate.inKickingState:
            player.claimedBall = False
            return True

    player.claimedBall = True

    return False
