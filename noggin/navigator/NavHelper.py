from . import NavConstants as constants
import man.motion as motion

class NavHelper(object):

    def __init__(self, nav):
        self.brain = nav.brain
        self.nav = nav

    def getOmniWalkParam(self, destX, destY, destH):
        my = self.brain.my
        bearing = radians(MyMath.getRelativeBearing(my.x, my.y, my.h, destX, destY))

        distToDest = MyMath.dist(my.x, my.y, destX, destY)

        # calculate forward speed
        forwardGain = constants.OMNI_GOTO_X_GAIN * distToDest* \
            cos(bearing)
        sX = constants.OMNI_GOTO_FORWARD_SPEED * forwardGain
        sX = MyMath.clip(sX,
                         constants.OMNI_MIN_X_SPEED,
                         constants.OMNI_MAX_X_SPEED)
        if fabs(sX) < constants.OMNI_MIN_X_MAGNITUDE:
            sX = 0

        # calculate sideways speed
        strafeGain = constants.OMNI_GOTO_Y_GAIN * distToDest* \
            sin(bearing)
        sY = constants.OMNI_GOTO_STRAFE_SPEED  * strafeGain
        sY = MyMath.clip(sY,
                         constants.OMNI_MIN_Y_SPEED,
                         constants.OMNI_MAX_Y_SPEED,)
        if fabs(sY) < constants.OMNI_MIN_Y_MAGNITUDE:
            sY = 0

        if NavMath.atDestinationCloser(nav):
            sX = sY = 0.0

        # calculate spin speed
        spinGain = constants.GOTO_SPIN_GAIN
        spinDir = MyMath.getSpinDir(my.h, destH)
        sTheta = spinDir * fabs(my.h - destH) * spinGain
        sTheta = MyMath.clip(sTheta,
                             constants.OMNI_MIN_SPIN_SPEED,
                             constants.OMNI_MAX_SPIN_SPEED)
        if fabs(sTheta) < constants.OMNI_MIN_SPIN_MAGNITUDE:
            sTheta = 0.0

        if NavMath.atHeading(nav):
            sTheta = 0.0

        if DEBUG: nav.printf("sX: %g  sY: %g  sTheta: %g" %
                   (sX, sY, sTheta))

        return (sX, sY, sTheta)

    def setSpeed(self, x, y, theta):
        """
        Wrapper method to easily change the walk vector of the robot
        """
        walk = motion.WalkCommand(x=x,y=y,theta=theta)
        self.brain.motion.setNextWalkCommand(walk)

    def step(self, x, y, theta, numSteps):
        """
        Wrapper method to easily change the walk vector of the robot
        """
        steps = motion.StepCommand(x=x,y=y,theta=theta,numSteps=numSteps)
        self.brain.motion.sendStepCommand(steps)

    def executeMove(self, sweetMove):
        """
        Method to enqueue a SweetMove
        Can either take in a head move or a body command
        (see SweetMove files for descriptions of command tuples)
        """

        for position in sweetMove:
            if len(position) == 7:
                move = motion.BodyJointCommand(position[4], #time
                                               position[0], #larm
                                               position[1], #lleg
                                               position[2], #rleg
                                               position[3], #rarm
                                               position[6], # Chain Stiffnesses
                                               position[5], #interpolation type
                                               )

            elif len(position) == 5:
                move = motion.BodyJointCommand(position[2], # time
                                               position[0], # chainID
                                               position[1], # chain angles
                                               position[4], # chain stiffnesses
                                               position[3], # interpolation type
                                               )

            else:
                nav.printf("What kind of sweet ass-Move is this?")

            self.brain.motion.enqueue(move)

