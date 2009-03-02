
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
NAO_HEADER_ID = "NAO"

class NaoOutput:
    def __init__(self, brain):
        """
        class constructor
        """
        self.brain = brain
        self.frameCounter = 0 # Used by saveFrame
        self.locLogCount = 0
        self.loggingLoc = False

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

        # The second line holds the current loc values needed for self init
        initLine = ("%g %g %g %g %g %g %g %g %g %g %g %g %g %g"
                    % ( self.brain.loc.x,
                        self.brain.loc.y,
                        self.brain.loc.h,
                        self.brain.loc.xUncert,
                        self.brain.loc.yUncert,
                        self.brain.loc.hUncert,
                        self.brain.loc.ballX,
                        self.brain.loc.ballY,
                        self.brain.loc.ballVelX,
                        self.brain.loc.ballVelY,
                        self.brain.loc.ballXUncert,
                        self.brain.loc.BallYUncert,
                        self.brain.loc.ballVelXUncert,
                        self.brain.loc.BallVelYUncert))

        # Write our first line
        self.locLog.writeLine(headerLine)
        self.locLog.writeLine(initLine)

    def logLoc(self):
        """
        Writes the next line of the log file
        """
        if not self.loggingLoc:
            return

        odometry = [0,0,0]#self.brain.motion.getLastOdometry()
        # Follow the line format
        locLine = "%ld %ld %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g" % (
            self.brain.nao.getSimulatedTime(), #TIME
            #FRAME COUNT
            self.locLog.frame,
            #ODOMETRY dF, dL, dA
            odometry[0], odometry[1], odometry[2],
            #YGLP DIST BEARING
            self.brain.yglp.dist, self.brain.yglp.bearing,
            #YGRP DIST BEARING
            self.brain.ygrp.dist, self.brain.ygrp.bearing,
            #BGLP DIST BEARING
            self.brain.bglp.dist, self.brain.bglp.bearing,
            #BGRP DIST BEARING
            self.brain.bgrp.dist, self.brain.bgrp.bearing,
            #BALL DIST BEARING
            self.brain.ball.dist, self.brain.ball.bearing)

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
