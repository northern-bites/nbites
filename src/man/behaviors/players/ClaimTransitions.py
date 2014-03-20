claimExpiration = 10

# TODO: figure out how to deal with oscillations between approachBall
#            and positionAtHome. Maybe don't transition to approachBall
#            if someone else has a better claim?
def shouldCedeClaim(player):
    for mate in player.brain.teamMembers:
        if (mate.playerNumber == player.brain.playerNumber):
            continue
        if not mate.claimedBall or not mate.active:
            continue

        # Now we get into actual claims
        if (int(player.brain.time * 1000) - mate.timestamp > claimExpiration):
            continue # That claim has expired (Comm is probably lagging)

        if (mate.ball_dist < player.brain.ball.distance):
            player.brain.claimedBall = False
            return True

        if mate.in_kicking_state:
            player.brain.claimedBall = False
            return True

    return False
