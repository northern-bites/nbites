package TOOL.WorldController;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.FileReader;
import java.io.FileNotFoundException;
import java.util.Iterator;
import java.util.Vector;
import java.util.StringTokenizer;
import java.awt.FileDialog;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.Timer;

public class LogHandler
{
    ///////////////// DEBUG SWITCHES //////////////
    private boolean DEBUG_LOG_DATA = false;

    ///////////////// CLASS INSTANCES /////////////

    // passed instantiations from wc
    private WorldControllerPainter painter;
    private WorldController wc;
    private DebugViewer debugViewer;

    // LogHandler's own instances
    // Process and input/output for simulating EKF python calculations
    private Process ekf_process;
    private BufferedReader ekf_input;
    private BufferedWriter ekf_output;

    // Instance of the LogBox Class
    private LogBox logBox;

    ////////////////// CONSTANTS //////////////

    // MCL Indices
    // Raw data stuff
    public final static int MCL_TEAM_COLOR_INDEX = 0;
    public final static int MCL_PLAYER_NUM_INDEX = 1;

    public final static int MCL_MY_X_INDEX = 0;
    public final static int MCL_MY_Y_INDEX = 1;
    public final static int MCL_MY_H_INDEX = 2;
    public final static int MCL_UNCERT_X_INDEX = 3;
    public final static int MCL_UNCERT_Y_INDEX = 4;
    public final static int MCL_UNCERT_H_INDEX = 5;

	// Obs info
	public final static int MCL_BALL_X_INDEX = 0;
    public final static int MCL_BALL_Y_INDEX = 1;
    public final static int MCL_BALL_UNCERT_X_INDEX = 2;
    public final static int MCL_BALL_UNCERT_Y_INDEX = 3;
    public final static int MCL_BALL_VELOCITY_X_INDEX = 4;
    public final static int MCL_BALL_VELOCITY_Y_INDEX = 5;
    public final static int MCL_BALL_VELOCITY_UNCERT_X_INDEX = 6;
    public final static int MCL_BALL_VELOCITY_UNCERT_Y_INDEX = 7;
    public final static int MCL_ODO_X_INDEX = 8;
    public final static int MCL_ODO_Y_INDEX = 9;
    public final static int MCL_ODO_H_INDEX = 10;

    //////////////////// VARIABLES /////////////

    // log file variables
    private int log_num_frames;
    private String logFile;
    private long log_playback_fps;
    private Vector<String> log_strings;
    private Vector<String> log_debug_strings;
    private boolean log_pause, log_played, log_last_frame, log_next_frame;
    private int log_marker, last_log_marker;
    private int ambiguousLandmarkCount;

    // log variables
    int team_color, player_number;
    Double thinks_x, thinks_y, thinks_h;
    Double uncertainty_x, uncertainty_y, uncertainty_h;
    Double ball_thinks_x, ball_thinks_y;
    Double ball_uncert_x, ball_uncert_y;
    Double ball_velocity_x, ball_velocity_y;
    Double ball_velocity_uncert_x, ball_velocity_uncert_y;

    public Timer playTimer;

    public LogHandler(WorldController _wc, WorldControllerPainter _painter,
                      DebugViewer _debugViewer)
    {
        // store WorldController,Field class instances locally
        wc = _wc;
        painter = _painter;
        debugViewer = _debugViewer;

        logBox = new LogBox(this, painter, 0, (int) (painter.getPreferredSize().
                                                     getHeight() + 70));

        // Variable initialization
        log_marker = 1;
        log_strings = new Vector<String>();
        log_playback_fps = wc.ROBOT_FPS;

        // create the timer that will automatically grab the next log
        // frame if the Log Viewer is in 'play' mode.
        ActionListener taskPerformer = new ActionListener () {
                public void actionPerformed(ActionEvent evt) {
                    if (log_marker < log_num_frames) {
                        log_marker++;
                    } else {
                        // else we've run out of frames, so stop
                        playTimer.stop();
                    }
                    if (wc.getMode() == wc.VIEW_MCL_LOG) {
                        viewFromMCLLog();
                    } else {
                        viewFromLog(false);
                    }
                    logBox.slide.setValue(log_marker);
                }
            };
        playTimer = new Timer((int)1000./wc.ROBOT_FPS, taskPerformer);
    }

    public void logPlay()
    {
        // Don't play if at end
        if (log_marker >= log_num_frames) return;

        logBox.play_pause.setText(logBox.PAUSE_STRING);
        playTimer.setDelay((int)1000./wc.getPlaybackFps());
        playTimer.start();
    }

    public void logPause()
    {
        //System.out.println("logPause: PLAY_STRING");
        logBox.play_pause.setText(logBox.PLAY_STRING);
        playTimer.stop();
    }
    public void logLastFrame()
    {
        // make sure you don't do array out of bounds
        if (log_marker > 1) {
            log_marker--;
        }
        logBox.play_pause.setText(logBox.PLAY_STRING);
        logBox.slide.setValue(log_marker);
        if (wc.getMode() == wc.VIEW_MCL_LOG) {
            viewFromMCLLog();
        } else {
            viewFromEKFLog();
        }
        playTimer.stop();
    }
    public void logNextFrame()
    {
        // make sure you don't array out of bounds
        if ((log_marker < log_num_frames &&
             wc.getMode() == wc.VIEW_MCL_LOG) ||
            log_marker < log_num_frames-1) {
            log_marker++;
        }

        logBox.play_pause.setText(logBox.PLAY_STRING);
        logBox.slide.setValue(log_marker);
        if (wc.getMode() == wc.VIEW_MCL_LOG) {
            viewFromMCLLog();
        } else {
            viewFromEKFLog();
        }

        playTimer.stop();
    }
    public void logStartFrame() {
        painter.clearSimulationHistory();
        log_marker = 1;

        logBox.play_pause.setText(logBox.PLAY_STRING);
        logBox.slide.setValue(log_marker);

        if (wc.getMode() == wc.VIEW_MCL_LOG) {
            viewFromMCLLog();
        } else {
            viewFromEKFLog();
        }
        playTimer.stop();
    }
    public void logEndFrame() {
        painter.clearSimulationHistory();
        if( wc.getMode() == wc.VIEW_MCL_LOG) {
            log_marker = log_num_frames;
        } else {
            log_marker = log_num_frames;
        }
        logBox.play_pause.setText(logBox.PLAY_STRING);
        logBox.slide.setValue(log_marker);
        if (wc.getMode() == wc.VIEW_MCL_LOG) {
            viewFromMCLLog();
        } else {
            viewFromEKFLog();
        }

        playTimer.stop();
    }

    public String formatDoubleForDisplay(Double debug_double) {
        return String.valueOf(debug_double.intValue());
    }
    public int getLogNumFrames() { return log_num_frames; }
    public void setLogMarker(int mark) { log_marker = mark; }
    public int getLogMarker() { return log_marker; }
    public boolean getPaused() { return !playTimer.isRunning(); }
    public void setPlaybackFps(int fps)
    {
        playTimer.setDelay((int)1000./fps);
    }


    public boolean reloadLog()
    {
        System.out.println("Attempting to reload log file");
        if(loadLog(logFile) ) {
            logBox.slide.setMaximum(log_num_frames);
            logBox.setVisible(true);
            debugViewer.setVisible(true);
            logStartFrame();
            return true;
        } else {
            return false;
        }

    }


    // Begin Monte Carlo stuff, hopefully this will be nicer than the things
    // Which are above and kind of suck.
    /**
     * Initializes  MCL or EKF log for super duper Nao fun!
     *
     * @return true if the file succesfully loads, otherwise false.
     */
    public boolean initLog()
    {
        // Save the last used directory
        logFile = wc.openDialog("Locate Log File to Load",
                                wc.t.wcLastDirectory,
                                FileDialog.LOAD);

        if (logFile == null) {
            System.out.println("log file not chosen");
            wc.setMode(wc.DO_NOTHING);
            return false;
        } else if (!loadLog(logFile)) {
            System.out.println("log file unable to load");
            return false;
        }

        logBox.slide.setMaximum(log_num_frames);
        logBox.setVisible(true);
        debugViewer.setVisible(true);
        logStartFrame();
        return true;
    }

    /**
     * Method to load in a MCL log file
     *
     * @return True if succesfully loads the log, otherwise false
     */
    public boolean loadLog(String logFile)
    {

        painter.updateParticleSet(new Vector<MCLParticle>(), team_color,
                                  player_number);
        // Clear the uncertainty ellipses
        painter.updateUncertainytInfo(0,0,0,0,0,0);
        painter.updateEstPoseInfo(painter.NO_DATA_VALUE,painter.NO_DATA_VALUE,
                                        painter.NO_DATA_VALUE);
        painter.updateRealPoseInfo(painter.NO_DATA_VALUE, painter.NO_DATA_VALUE,
                                   painter.NO_DATA_VALUE);
        painter.updateRealBallInfo(painter.NO_DATA_VALUE,painter.NO_DATA_VALUE,
                                   painter.NO_DATA_VALUE,painter.NO_DATA_VALUE);

        BufferedReader dataIn = null;
        if( wc.getMode() == wc.VIEW_MCL_LOG) {
            System.out.println("Loading MCL log file: " + logFile + "... ");
        } else {
            System.out.println("Loading EKF log file: " + logFile + "... ");
        }
        log_strings.clear();

        // Read in the passed in file
        try {
            dataIn = new BufferedReader(new FileReader(logFile));
        } catch (FileNotFoundException e) {
            System.err.println(e.getMessage());
            return false;
        }

        // Begin parsing the file for particle data
        try {
            // Pull out the lines one by one
            while(dataIn.ready()) {
                log_strings.add(dataIn.readLine());
            }
            log_num_frames = log_strings.size();

        } catch (IOException e2) {
            System.err.println(e2.getMessage());
            return false;
        }finally {
            // Close up before moving along
            try {
                dataIn.close();
            }catch (IOException e3) {
                System.err.println(e3.getMessage());
                return false;
            }
			logBox.setLogName(logFile);
            return true;
        }
    }

    /**
     * Creates a vector of particles from the given line of an MCL log file
     *
     * @param line The line of the log file to be parsed into particles
     * @return A vector of all the particles from the read in line
     */
    public Vector< MCLParticle > parseParticleLine(String line)
    {
        Vector< MCLParticle > particles = new Vector<MCLParticle>();
        String[] lineValues = line.split(" ");

        // Pull off the data 4 items at a time for each particle
        // The items are (x,y,h,weight)
        for(int i = 0; i < lineValues.length; i += 4) {
            try{
                particles.add(new
                              MCLParticle(Float.parseFloat(lineValues[i]),
                                          Float.parseFloat(lineValues[i+1]),
                                          Float.parseFloat(lineValues[i+2]),
                                          Float.parseFloat(lineValues[i+3])));
            } catch (NumberFormatException nfe) {
                //System.out.println(nfe.getMessage());
                System.out.println("Attempted to add nan particle at index " +
                                   i + " in line " + log_marker);
                return particles;
            } catch (ArrayIndexOutOfBoundsException obe) {
                System.out.println("Wrong number of elements for particle " +
                                   " parse");
                System.out.println("Have read in " + i / 4 + " particles");
                return particles;
            }
        }
        return particles;
    }

    /**
     * View the current frame from the log...
     * MCL Log lines contain three groups of data separated by colons (:)
     * The first group of information is information about the particles
     */
    public void viewFromMCLLog()
    {
        viewFromLog(true);
    }


    public void viewFromEKFLog()
    {
        viewFromLog(false);
    }

    public void viewFromLog(boolean hasParticles)
    {
        // Method variables
        Vector< MCLParticle> particles;
        String particleInfo, debugInfo, landmarkInfo, realPoseInfo;
        StringTokenizer t;

        // Make sure the line is not blank
        if (!log_strings.isEmpty()) {
            // set frame total in the log box
            logBox.frameTotal.setText("" + log_num_frames);
            debugViewer.frameTotal.setText("" + log_num_frames);

            // set frame number in log box & debug viewer
            logBox.frameNumber.setText("" + log_marker);
            debugViewer.frameNumber.setText("" + log_marker);

            // checks log_strings if current frame exists
            if (log_strings != null && log_marker < log_strings.size()) {

                // Split the log at the :
                t = new StringTokenizer(log_strings.get(log_marker-1),":");

                // Paint the particles on the screen
                if (hasParticles) {
                    particleInfo = t.nextToken();
                    particles = parseParticleLine(particleInfo);
                } else {
                    particles = new Vector<MCLParticle>();
                }

                // Update the debug viewer
                debugInfo = t.nextToken();
                Vector<LocalizationPacket> locModels =
					processDebugInfo(debugInfo);

				LocalizationPacket actualEst = locModels.get(0);
				locModels.remove(0);
                if(!hasParticles) {

                    // Draw the robot position if there aren'y any particles
                    painter.updateEstPoseInfo(
											  (float)actualEst.getXEst(),
											  (float)actualEst.getYEst(),
											  (float)actualEst.getHeadingEst());
                }

                // Draw the uncertainty ellipses on the screen
				painter.updateUncertainytInfo( actualEst.getXEst(),
											   actualEst.getYEst(),
											   actualEst.getHeadingEst(),
											   actualEst.getXUncert(),
											   actualEst.getYUncert(),
											   actualEst.getHUncert());

				painter.updateModels(locModels, team_color);

                // Parse the known robot position
                realPoseInfo = t.nextToken();
                processRobotPose(realPoseInfo);

                // Update the observed landarmks information
                // Check if any landmarks were sighted this frame
                debugViewer.removeLandmarks();
                if (t.hasMoreTokens()) {
                    landmarkInfo = t.nextToken();
                    processObservedLandmarkInfo(landmarkInfo);
                }

                // Update the screen view
                painter.updateParticleSet(particles, team_color,
                                          player_number);
                painter.reportEndFrame();
            }
        }
    }

    /**
     * Method to print data to the debug viewer from an MCL log file
     *
     * @param newInfos The newest set of sensor info to display
     * @return Vector<LocalizationPacket> Vector of loc packets,
     *         each one representing a model in the locsystem
     */
    private Vector<LocalizationPacket> processDebugInfo(String newInfos)
    {
		StringTokenizer robotObsSplit =
			new StringTokenizer(newInfos,"|");
		String playerInfo = robotObsSplit.nextToken();
		String robotInfo = robotObsSplit.nextToken();
		String obsInfo = robotObsSplit.nextToken();

        // Strip the team color and player number from the front of the line
		String[] playerInfos = playerInfo.split(" ");
        team_color = Integer.parseInt(playerInfos[MCL_TEAM_COLOR_INDEX]);
        player_number = Integer.parseInt(playerInfos[MCL_PLAYER_NUM_INDEX]);

		StringTokenizer models = new StringTokenizer(robotInfo, ";");

		Vector<LocalizationPacket> modelPackets =
			new Vector<LocalizationPacket>();


		while (models.hasMoreTokens()){
			String[] robotInfoVals = models.nextToken().split(" ");
			modelPackets.add(LocalizationPacket.makeEstimateAndUncertPacket
							 (
							  new Double(robotInfoVals[MCL_MY_X_INDEX]),
							  new Double(robotInfoVals[MCL_MY_Y_INDEX]),
							  new Double(robotInfoVals[MCL_MY_H_INDEX]),
							  new Double(robotInfoVals[MCL_UNCERT_X_INDEX]),
							  new Double(robotInfoVals[MCL_UNCERT_Y_INDEX]),
							  new Double(robotInfoVals[MCL_UNCERT_H_INDEX]))
							 );
		}

        // Do boring converting from strings to appropriate types...
        // put all the loc values into debugViewer

		// First locpacket is best model
		LocalizationPacket robotLoc = modelPackets.get(0);
        debugViewer.myX.setText(Double.toString(robotLoc.getXEst()));
		debugViewer.myY.setText(Double.toString(robotLoc.getYEst()));
        debugViewer.myH.setText(Double.toString(robotLoc.getHeadingEst()));
        debugViewer.myUncertX.setText(Double.toString(robotLoc.getXUncert()));
        debugViewer.myUncertY.setText(Double.toString(robotLoc.getYUncert()));
        debugViewer.myUncertH.setText(Double.toString(robotLoc.getHUncert()));

        String[] updateInfos = obsInfo.split(" ");
        Double ball_velocity_x, ball_velocity_y, ball_thinks_x,
            ball_thinks_y, ball_uncert_x, ball_uncert_y;
        LocalizationPacket ball_loc_info;

        // Ball information
        ball_thinks_x = new Double(updateInfos[MCL_BALL_X_INDEX]);
        ball_thinks_y = new Double(updateInfos[MCL_BALL_Y_INDEX]);
        ball_uncert_x = new Double(updateInfos[MCL_BALL_UNCERT_X_INDEX]);
        ball_uncert_y = new Double(updateInfos[MCL_BALL_UNCERT_Y_INDEX]);
        ball_velocity_x = new Double(updateInfos[MCL_BALL_VELOCITY_X_INDEX]);
        ball_velocity_y = new Double(updateInfos[MCL_BALL_VELOCITY_Y_INDEX]);

        debugViewer.ballX.setText(ball_thinks_x.toString());
        debugViewer.ballY.setText(ball_thinks_y.toString());
        debugViewer.ballUncertX.setText(ball_uncert_x.toString());
        debugViewer.ballUncertY.setText(ball_uncert_y.toString());
        debugViewer.ballVelX.setText(ball_velocity_x.toString());
        debugViewer.ballVelY.setText(ball_velocity_y.toString());

        double absBallVelocity =
            Math.sqrt( Math.pow(ball_velocity_x.doubleValue(),2.0)
                       + Math.pow(ball_velocity_y.doubleValue(),2.0));
	    debugViewer.ballVelAbs.setText(""+absBallVelocity);
        debugViewer.ballVelUncertX.
            setText(updateInfos[MCL_BALL_VELOCITY_UNCERT_X_INDEX]);
        debugViewer.ballVelUncertY.
            setText(updateInfos[MCL_BALL_VELOCITY_UNCERT_Y_INDEX]);
        debugViewer.odoX.setText(updateInfos[MCL_ODO_X_INDEX]);
        debugViewer.odoY.setText(updateInfos[MCL_ODO_Y_INDEX]);
        debugViewer.odoH.setText(updateInfos[MCL_ODO_H_INDEX]);

        ball_loc_info = LocalizationPacket.
            makeBallEstimateAndUncertPacket(ball_thinks_x.doubleValue(),
                                            ball_thinks_y.doubleValue(),
                                            ball_uncert_x.doubleValue(),
                                            ball_uncert_y.doubleValue(),
                                            ball_velocity_x.doubleValue(),
                                            ball_velocity_y.doubleValue());
        painter.reportUpdatedBallLocalization(ball_loc_info, team_color,
                                              player_number);

		return modelPackets;
    }

    /**
     * Method to print data to the debug viewer about sighted landmarks
     * Circles reported landmarks on the field
     *
     * @param newInfos The newest set of sensor info to display
     */
    private void processObservedLandmarkInfo(String newInfos)
    {
        // Get info into an array
        String[] infos = newInfos.split(" ");

        // Iterate through the landmark IDs getting the correct ones
        ambiguousLandmarkCount = 0;
        for(int i = 0; i < infos.length; i++) {
            int ID = Integer.parseInt(infos[i]);
            float dist = Float.parseFloat(infos[++i]);
            float bearing = Float.parseFloat(infos[++i]);

            // Write sighted landmarks to the debug viewer
            debugViewer.addLandmark(ID,dist,bearing);
            // Circle observed landmarks on the field
            decodeAndDisplayLandmark(ID);
        }
    }

    /**
     * Process and draw a known pose of the robot on the field
     *
     * @param realPoseInfo A string of the form "x y h"
     */
    private void processRobotPose(String realPoseInfo)
    {
        float x, y, h, ballX, ballY, ballVelX, ballVelY;
        String[] infos = realPoseInfo.split(" ");
        x = Float.parseFloat(infos[0]);
        debugViewer.knownX.setText(infos[0]);
        y = Float.parseFloat(infos[1]);
        debugViewer.knownY.setText(infos[1]);
        h = Float.parseFloat(infos[2]);
        debugViewer.knownH.setText(infos[2]);
        ballX = Float.parseFloat(infos[3]);
        debugViewer.knownBallX.setText(infos[3]);
        ballY = Float.parseFloat(infos[4]);
        debugViewer.knownBallY.setText(infos[4]);
        ballVelX = Float.parseFloat(infos[5]);
        ballVelY = Float.parseFloat(infos[6]);
        painter.updateRealPoseInfo(x, y, h);
        painter.updateRealBallInfo(ballX, ballY, ballVelX, ballVelY);
    }

    /**
     * Method to decode a landmark ID to (x,y) coordinates and highlight
     * the landmark appropriately on the screen
     * draws solid circles around distinct landmarks
     * draws dashed circles arond all ambigious observation possibilities
     *
     * @param ID the ID of the landmark to be drawn
     */
    private void decodeAndDisplayLandmark(int ID)
    {
        // Determine the type of landmark and draw it
        // We have a distinct landmark
        if (debugViewer.isDistinctLandmarkID(ID)) {
            painter.sawLandmark((float)debugViewer.objectIDMap.get(ID).x,
                                (float)debugViewer.objectIDMap.get(ID).y,
                                0);
        } else if (ID != debugViewer.BALL_ID) { // We have an ambigious landmark
            // get the list of possible landmarks
            ++ambiguousLandmarkCount;
            for (int pos_id : debugViewer.getPossibleIDs(ID)) {
                painter.sawLandmark((float)
                                    debugViewer.objectIDMap.get(pos_id).x,
                                    (float)
                                    debugViewer.objectIDMap.get(pos_id).y,
                                    ambiguousLandmarkCount);
            }
        } else { // We Have a ball ignore...
        }
    }


    /**
     * Method to cleanup our mess when we leave the log
     */
    public void quitMCLLog()
    {
        logBox.setVisible(false);
        debugViewer.setVisible(false);
        logStartFrame();
        logBox.slide.setMaximum(log_num_frames);
        painter.updateParticleSet(new Vector<MCLParticle>(), team_color,
                                  player_number);
        // Clear the uncertainty ellipses
        painter.updateUncertainytInfo(0,0,0,0,0,0);
        painter.updateEstPoseInfo(painter.NO_DATA_VALUE,painter.NO_DATA_VALUE,
                                        painter.NO_DATA_VALUE);
        painter.updateRealPoseInfo(painter.NO_DATA_VALUE, painter.NO_DATA_VALUE,
                                   painter.NO_DATA_VALUE);
        painter.updateRealBallInfo(painter.NO_DATA_VALUE,painter.NO_DATA_VALUE,
                                   painter.NO_DATA_VALUE,painter.NO_DATA_VALUE);
        painter.reportEndFrame();
    }
}