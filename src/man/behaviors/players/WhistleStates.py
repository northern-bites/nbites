import time

def shouldTrustMateWhistle(player):
    if not player.useMateWhistle:
        return False

    mates_heard = []
    mates_active = []

    print "Current whistle: ", player.brain.whistled
    if player.brain.whistled == True:
        return True
    if player.stateTime > 5:
	    for mate in player.brain.teamMembers:
	        if mate.active == True:
	            mates_active.append(mate)
	        if  mate.shared_whistle and mate.active:
	            mates_heard.append(mate)

	    proportion_of_active_whistle = mates_heard.length / mates_active.length
	    print proportion_of_active_whistle
	    if proportion_of_active_whistle >= 0.5:
	        return True
    return False
