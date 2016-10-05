import vision
import man.motion.HeadMoves as HeadMoves
from objects import RelRobotLocation

'''
Jack Truskowski - Robotics Assignment to make a player walk to an end line
'''

def gameSet(player):
    if player.firstFrame():
        player.brain.nav.stand()
    return player.stay

def gamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.track.lookToAngleFixedPitch(0)

    while(not player.brain.motion.calibrated()):
        return player.stay()

    return player.goNow('walkToBall')


def walkToBall(player):
    ball = player.brain.ball

    if(not ball.vis.on):
        print "no ball"
        dest = RelRobotLocation(10,0,0)
        player.brain.nav.goTo(dest)
        return player.stay()

    elif(bal.vis.dist < 30):
        print "saw the ball, stopping"
        return player.goNow('gameSet')

    else:
        print "see ball, going towards it"
        player.brain.nav.goTo(ball.loc)
        return player.stay()


#walks a straight line and stops when a field line is adequately close
def walkToFieldLine(player):
    print "starting to walk to the field line"
    arrivedAtLine = False
    
    while not arrivedAtLine:
        #check to see if the robot has arrived at the line
        for i in range(0, player.brain.visionLines.line_size()):
            if(player.brain.visionLines.line(i).wz0 < 20):
                arrivedAtLine = True
                break

        #if not, continue walking straight
        if not arrivedAtLine:
            player.brain.nav.walkTo(RelRobotLocation(20,0,0), speeds.SPEED_SEVEN) #move straight ahead        

    print "stopped at the field line"
    return player.stay()
