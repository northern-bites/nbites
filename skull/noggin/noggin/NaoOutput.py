"""
NaoOutput.py - File for holding all sorts of output and logging functions
"""
# CONSTANTS
# saveFrame
FRAME_DIR = "picSet/"
RAW_HEADER_SIZE = 0

# Logging
LOG_DIR = "logs/"
# Localization Logs
LOC_LOG_TYPE = "localization"
EKF_LOG_TYPE = "EKF"
NAO_HEADER_ID = "NAO"

class NaoOutput:
    def __init__(self, brain):
        """
        class constructor
        """
        self.brain = brain
        self.frameCounter = 0 # Used by saveFrame
        self.locLogCount = 0
        self.ekfLogCount = 0
        self.loggingLoc = False
        self.loggingEKF = False

    def printf(self,str):
        """
        Prints string to robot_console
        """
        print str

    def saveFrame(self):
        """
        Save image frame for logging
        """
        fileString = FRAME_DIR+str(self.frameCounter)+".NSFRM"
        self.printf("Saved frame to "+fileString)
        f = file(fileString,'w')
        f.write('\0'*RAW_HEADER_SIZE)
        f.write(self.brain.nao.camera.get())
        f.close()
        self.frameCounter+=1

    def printFSR(self):
	fsr = self.brain.nao.fsr.gets()
	fsrSum = 0.
	for x in fsr:
	    fsrSum +=x
	    
	print "Fsr",fsr," sum", fsrSum 

    # Functionality for logging
    def updateLogs(self):
        """
        Called every frame by brain to help update logs
        """
        if self.loggingLoc:
            self.logLoc()
        if self.loggingEKF:
            self.logEKF()


    def newLog(self, logType, logCount):
        """
        Use this method to start a new log file of type logType
        """
        return Log(logType, logCount)

    # functions for localization logging
    def startLocLog(self):
        """
        Log our stuff
        """
        # Do not start a new log if logging
        if self.loggingLoc:
            return

        self.printf("Starting Localization Logging")
        self.loggingLoc = True
        self.locLogCount += 1
        self.locLog = self.newLog(LOC_LOG_TYPE, self.locLogCount)

        # Write the first line holding teamColor and playerNumber
        headerLine = str(self.brain.my.teamColor) + " " + \
            str(self.brain.my.playerNumber) + " " + NAO_HEADER_ID

        # The second line holds the current EKF values needed for self init
        initLine = ("%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g"
                    % ( self.brain.ekf.getXEst(),
                        self.brain.ekf.getYEst(),
                        self.brain.ekf.getHeadingEst(),
                        self.brain.ekf.getXUncert(),
                        self.brain.ekf.getYUncert(),
                        self.brain.ekf.getHeadingUncert(),
                        self.brain.ekf.getBallXEst(),
                        self.brain.ekf.getBallYEst(),
                        self.brain.ekf.getBallXVelocityEst(),
                        self.brain.ekf.getBallYVelocityEst(),
                        self.brain.ekf.getBallXUncert(),
                        self.brain.ekf.getBallYUncert(),
                        self.brain.ekf.getBallXVelocityUncert(),
                        self.brain.ekf.getBallYVelocityUncert(),
                        self.brain.ekf.frames_since_reset))

        # Write our first line
        self.locLog.writeLine(headerLine)
        self.locLog.writeLine(initLine)

    def logLoc(self):
        """
        Writes the next line of the log file
        """
        if not self.loggingLoc:
            return

        odometry = self.brain.motion.getLastOdometry()
        # Follow the line format
        locLine = "%ld %ld %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g" % (#TIME
            self.brain.nao.getSimulatedTime(),
            #FRAME COUNT
            self.locLog.frame,
            #ODOMETRY dF, dL, dA
            odometry[0], odometry[1], odometry[2],
            #BY DIST BEARING
            0, 0,
            #YB DIST BEARING
            0, 0,
            #YGLP DIST BEARING
            self.brain.yglp.dist, self.brain.yglp.bearing,
            #YGRP DIST BEARING
            self.brain.ygrp.dist, self.brain.ygrp.bearing,
            #BGLP DIST BEARING
            self.brain.bglp.dist, self.brain.bglp.bearing,
            #BGRP DIST BEARING
            self.brain.bgrp.dist, self.brain.bgrp.bearing,
            #BALL DIST BEARING
            self.brain.ball.dist, self.brain.ball.bearing,
            #PWM
            1.5,
            # Head Yaw
            self.brain.motion.getHeadYaw())

        for corner in self.brain.corners:
            locLine += " %d %g %g" & (corner.visionId, corner.dist,
                                      corner.bearing)

        self.locLog.writeLine(locLine)

    def stopLocLog(self):
        """
        Method to start logging
        """
        if not self.loggingLoc:
            return
        self.printf("Stopping Localization Logging")
        self.loggingLoc = False
        self.locLog.closeLog()

    def startEKFLog(self):
        """
        This is to start logging of all EKF data for playback offline
        """
        if self.loggingEKF:
            return
        self.printf("Starting EKF Logging")
        self.loggingEKF = True
        self.ekfLog = self.newLog(EKF_LOG_TYPE, self.ekfLogCount)

        # Write header line
        self.ekfLog.writeLine(NAO_HEADER_ID)

    def logEKF(self):
        """
        Method to write a ine of ekf information
        """
        if not self.loggingEKF:
            return
        odo = self.brain.motion.getLastOdometry()
        infoLine = ("%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g" 
                    % ( self.brain.nao.getSimulatedTime(),
                        self.ekfLog.frame,
                        self.brain.my.teamColor,
                        self.brain.my.playerNumber,

                        # Self
                        self.brain.ekf.getXEst(),
                        self.brain.ekf.getYEst(),
                        self.brain.ekf.getHeadingEst(),
                        self.brain.ekf.getXUncert(),
                        self.brain.ekf.getYUncert(),
                        self.brain.ekf.getHeadingUncert(),

                        # Ball
                        self.brain.ekf.getBallXEst(),
                        self.brain.ekf.getBallYEst(),
                        self.brain.ekf.getBallXVelocityEst(),
                        self.brain.ekf.getBallYVelocityEst(),
                        self.brain.ekf.getBallXUncert(),
                        self.brain.ekf.getBallYUncert(),
                        self.brain.ekf.getBallXVelocityUncert(),
                        self.brain.ekf.getBallYVelocityUncert(),
                        self.brain.ball.dist,
                        self.brain.ball.bearing,

                        # Odometry
                        odo[1], # dF
                        odo[0], # dL
                        odo[2]))# dA

        self.ekfLog.writeLine(infoLine)

    def stopEKFLog(self):
        """
        Method to stop EKF logging
        """
        if not self.loggingEKF:
            return
        self.printf("Stopping EKF Logging")
        self.loggingEKF = False
        self.ekfLog.closeLog()

class Log:
    """
    class for making logs
    """
    def __init__(self, logType, count):
        """
        set it up
        """
        self.frame = 0
        self.logType = logType
        logTitle = LOG_DIR + logType + "/out" + str(count) + ".log"
        self.logFile = open(logTitle, 'w')

    def writeLine(self, line):
        """
        write a line to the log file
        """
        self.frame += 1
        try:
            self.logFile.write(line+"\n")
        except Exception, e:
            print "Error writing to logFile:",e

    def closeLog(self):
        """
        Close our log file
        """
        try:
            self.logFile.close()
        except Exception, e:
            print "Error closing logfile:",e
