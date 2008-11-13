package edu.bowdoin.robocup.TOOL.WorldController;

import java.io.*;
import java.util.Iterator;
import java.util.Vector;
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

    // LOG constants
    public final static int TIME_INDEX = 0;
    public final static int FRAME_INDEX = 1;
    public final static int TEAM_COLOR_INDEX = 2;
    public final static int PLAYER_NUMBER_INDEX = 3;
    public final static int MY_X_INDEX = 4;
    public final static int MY_Y_INDEX = 5;
    public final static int MY_H_INDEX = 6;
    public final static int UNCERT_X_INDEX = 7;
    public final static int UNCERT_Y_INDEX = 8;
    public final static int UNCERT_H_INDEX = 9;
    public final static int BALL_X_INDEX = 10;
    public final static int BALL_Y_INDEX = 11;
    public final static int BALL_UNCERT_X_INDEX = 12;
    public final static int BALL_UNCERT_Y_INDEX = 13;
    public final static int BALL_VELOCITY_X_INDEX = 14;
    public final static int BALL_VELOCITY_Y_INDEX = 15;
    public final static int BALL_VELOCITY_UNCERT_X_INDEX = 16;
    public final static int BALL_VELOCITY_UNCERT_Y_INDEX = 17;
    public final static int BALL_DIST_INDEX = 18;
    public final static int BALL_BEARING_INDEX = 19;
    public final static int ODO_X_INDEX = 20;
    public final static int ODO_Y_INDEX = 21;
    public final static int ODO_H_INDEX = 22;
    public final static int NUM_LOG_LOC_INDEXES = 23;

    // MCL Indices
    public final static int MCL_TEAM_COLOR_INDEX = 0;
    public final static int MCL_PLAYER_NUM_INDEX = 1;

    // log strings
    public static final String LOG_DEBUG_STRING = "debug:";
    public static final String LOG_CAMERA_STRING = "camera:";
    private static final String AIBO_OUT_FILE = "./logreader/aibo.tmp.log";
    private static final String NAO_OUT_FILE = "./logreader/nao.tmp.log";
    private static final String LOG_OUT_FILE = "/out.tmp.log";
    private static final String PYTHON_COMMAND =
        "./TOOL/WorldController/logreader/outputEKF.py ";

    // log types
    public static final String NAO_LOG_TYPE = "NAO";
    public static final String AIBO_LOG_TYPE = "AIBO";


    //////////////////// VARIABLES /////////////

    // log file variables
    private int log_num_frames;
    private String logFile;
    private String logType;
    private long log_playback_fps;
    private Vector<String> log_strings;
    private Vector<String> log_debug_strings;
    private Vector<String> ekf_results;
    private Vector<Vector> log_debugs;
    private Vector<String> log_camera;
    private boolean log_pause, log_played, log_last_frame, log_next_frame;
    private int log_marker, last_log_marker;

    // log variables
    int team_color, player_number;
    Double thinks_x, thinks_y, thinks_h;
    Double uncertainty_x, uncertainty_y, uncertainty_h;
    Double odo_x, odo_y, odo_h;
    public Double left_beacon_dist, left_beacon_bearing;
    public Double right_beacon_dist, right_beacon_bearing;
    public Double my_goal_left_post_dist, my_goal_left_post_bearing;
    public Double my_goal_right_post_dist, my_goal_right_post_bearing;
    public Double opp_goal_left_post_dist, opp_goal_left_post_bearing;
    public Double opp_goal_right_post_dist, opp_goal_right_post_bearing;
    public Double ball_dist, ball_bearing;

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
        log_debugs = new Vector<Vector>();
        log_camera = new Vector<String>();
        log_playback_fps = wc.ROBOT_FPS;

        // create the timer that will automatically grab the next log
        // frame if the Log Viewer is in 'play' mode.
        ActionListener taskPerformer = new ActionListener () {
                public void actionPerformed(ActionEvent evt) {
                    if (log_marker < log_num_frames-1) {
                        log_marker++;
                    } else {
                        // else we've run out of frames, so stop
                        playTimer.stop();
                    }
                    viewFromLog();
                }
            };
        playTimer = new Timer((int)1000./wc.ROBOT_FPS, taskPerformer);
    }

    /**
     * Initializes the classic localization log for an AIBO
     *
     * @return true if the file succesfully loads, otherwise false.
     */
    public boolean initDogLog()
    {
        // We will run the outputekf library on the correct log
        logFile = wc.openDialog("Locate Log File to Load",
                                (System.getProperty("user.dir") +
                                 "/logreader/"),FileDialog.LOAD);
        if (logFile == null) {
            System.out.println("log file not chosen");
            wc.setMode(wc.DO_NOTHING);
            return false;
        } else if (!loadUnprocessedLog(logFile)) {
            System.out.println("log file unable to load");
            wc.setMode(wc.DO_NOTHING);
            return false;
        }

        // Run the appropriate output ekf script on the file
        logBox.slide.setMaximum(log_num_frames);
        logBox.setVisible(true);
        debugViewer.setVisible(true);
        logStartFrame();
        return true;
    }

    /**
     * Initializes the newer EKF localization log for an AIBO, which replays
     * what the robot estimated during use
     *
     * @return true if the file succesfully loads, otherwise false.
     */
    public boolean initEKFLog()
    {
        logFile = wc.openDialog("Locate Log File to Load",
                                (System.getProperty("user.dir") +
                                 "/matrixlib/"),FileDialog.LOAD);

        if (logFile == null) {
            System.out.println("log file not chosen");
            wc.setMode(wc.DO_NOTHING);
            return false;
        } else if (!loadLog(logFile)) {
            System.out.println("log file not unable to load");
            return false;
        }

        logBox.slide.setMaximum(log_num_frames);
        logBox.setVisible(true);
        debugViewer.setVisible(true);
        logStartFrame();
        return true;
    }

    public void quitDogLog()
    {
        logBox.setVisible(false);
        debugViewer.setVisible(false);
        logStartFrame();
        logBox.slide.setMaximum(log_num_frames);
        painter.clearSimulationHistory();
    }

    public void viewFromLog()
    {
        if (log_strings.size() > 0 /*log_frame_strings != null*/) {
            // set frame total in the log box
            logBox.frameTotal.setText("" + (log_num_frames-1));
            debugViewer.frameTotal.setText("" + (log_num_frames-1));

            // set frame number in log box & debug viewer
            logBox.frameNumber.setText("" + log_marker);
            debugViewer.frameNumber.setText("" + log_marker);

            // checks log_strings if current frame exists
            if (log_strings != null && log_marker < log_strings.size()) {

                // print debug strings if there are any
                for (int i = 0; i < log_debugs.get(log_marker).size();
                     i++) {
                    System.out.println(log_debugs.get(log_marker).get(i));
                }
                // get camera data if any
                if (log_camera.get(log_marker) != null) {
                    parseCameraLine(log_camera.get(log_marker));
                }

                parseLogLine(log_strings.get(log_marker));

                if (DEBUG_LOG_DATA) {
                    System.out.println("thinks_x: " + thinks_x +
                                       " thinks_y: " + thinks_y +
                                       " thinks_heading: " + thinks_h +
                                       " uncertainty_x: " +
                                       uncertainty_x +
                                       " uncertainty_y: " +
                                       uncertainty_y +
                                       " uncertainty_h: " +
                                       uncertainty_h);
                }
                // store ball estimates in a 'LocalizationPacket'
                LocalizationPacket ball_loc_info =
                    LocalizationPacket.
                    makeBallEstimateAndUncertPacket(ball_thinks_x,
                                                    ball_thinks_y,
                                                    ball_uncert_x,
                                                    ball_uncert_y,
                                                    ball_velocity_x,
                                                    ball_velocity_y);
                painter.
                    reportUpdatedBallLocalization(ball_loc_info,
                                                  team_color,
                                                  player_number);

                // store my estimates in a 'LocalizationPacket'
                LocalizationPacket current_localization_info =
                    LocalizationPacket.
                    makeEstimateAndUncertPacket(thinks_x,
                                                thinks_y,
                                                thinks_h,
                                                0.0,
                                                uncertainty_x,
                                                uncertainty_y,
                                                uncertainty_h);
                painter.reportUpdatedLocalization(current_localization_info,
                                                  team_color,
                                                  player_number);
                painter.reportEndFrame();

                //painter.sawBluePost(log_by_dist,log_by_bearing);
                //painter.sawYellowPost(log_yb_dist,log_yb_bearing);

            }
            //logBox.slide.setValue(log_marker);

            logBox.play_pause.setText(logBox.PLAY_STRING);

        }
    }

    public void logPlay()
    {
        // re-loop if end of queue
        if (log_marker == log_num_frames || log_marker == log_num_frames-1) log_marker = 1;
        //System.out.println("logPlay: PAUSE_STRING");
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
            viewFromLog();
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
            viewFromLog();
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
            viewFromLog();
        }
        playTimer.stop();
    }
    public void logEndFrame() {
        painter.clearSimulationHistory();
        if( wc.getMode() == wc.VIEW_MCL_LOG) {
            log_marker = log_num_frames;
        } else {
            log_marker = log_num_frames-1;
        }
        logBox.play_pause.setText(logBox.PLAY_STRING);
        logBox.slide.setValue(log_marker);
        if (wc.getMode() == wc.VIEW_MCL_LOG) {
            viewFromMCLLog();
        } else {
            viewFromLog();
        }

        playTimer.stop();
    }

    /**
     * Method to first run python processing of a localization log
     * Then loads the log as a process log
     *
     * @param fileName the name of the file to process and load
     * @return Success or failure at loading the file
     */
    public boolean loadUnprocessedLog(String fileName)
    {
        String pyCommand = PYTHON_COMMAND;
        pyCommand = pyCommand.concat(fileName);
        Process pythonProc = null;
        try {
            pythonProc = Runtime.getRuntime().exec(pyCommand);
            pythonProc.waitFor();
        } catch(IOException e) {
            System.err.println("IO error in EKF process");
            System.err.println(e.getMessage());
        }
        catch (java.lang.InterruptedException f) {
            System.err.println("EKF log creation was aborted");
            System.err.println(f.getMessage());
        }

        // Load our processed log
        System.out.println("..: " +
                           System.getProperty("user.dir").concat(LOG_OUT_FILE));
       	return loadLog(System.getProperty("user.dir").concat(LOG_OUT_FILE));
    }

    public boolean loadLog(String fileName) {

        BufferedReader file_input = null;

        System.out.println("Loading log file: " + fileName + "... ");

        try {
            file_input = new BufferedReader(new FileReader(fileName));
        } catch (FileNotFoundException e) {
            System.err.println(e.getMessage());
            return false;
        }

        // Default to aibo type
        logType = AIBO_LOG_TYPE;
        log_strings.clear();
        log_debugs.clear();
        log_camera.clear();
        Vector<String> one_debug = new Vector<String>();
        Vector<String> one_camera = new Vector<String>();

        try {
            while (file_input.ready()) {
                String read_line = file_input.readLine();
                // Check if we are aibo or nao
                // Set the field here...
                if (read_line.equals(AIBO_LOG_TYPE)) {
                    logType = AIBO_LOG_TYPE;
                    if (wc.the_field == wc.naoField ) {
                        wc.switchFields();
                    }
                    continue;
                } else if (read_line.equals(NAO_LOG_TYPE)) {
                    logType = NAO_LOG_TYPE;
                    if (wc.the_field == wc.aiboField ) {
                        wc.switchFields();
                    }
                    continue;
                }

                // if the log line is a debug line, add to vector
                if (read_line.substring(0,6).equals(LOG_DEBUG_STRING)) {
                    one_debug.add(read_line);
                }
                else if (read_line.substring(0,7).equals(LOG_CAMERA_STRING)) {
                    log_camera.add(read_line);
                }
                else {
                    if (one_debug != null) {
                        log_debugs.add(one_debug);
                        one_debug = new Vector<String>();
                    }
                    log_camera.add(null);
                    log_strings.add(read_line);
                }
            }
            log_num_frames = log_strings.size();
        }catch (IOException e2) {
            System.err.println(e2.getMessage());
            return false;
        }finally {
            try {
                file_input.close();
            }catch (IOException e3) {
                System.err.println(e3.getMessage());
                return false;
            }
            return true;
        }
    }

    // parses line of text from log
    public void parseLogLine(String log_line) {
        System.out.println("\n----------New frame----------\n");
        // splits line into an array of string
        String[] current_frame = log_line.split(" ");
        // gets length
        int log_length = current_frame.length;

        // experimental
        //for (int i = 0; i < log_length; i++) {
        //    Double value = new Double(current_frame[i]);
        //    debugViewer.setLabel(i,value);
        //}

        Integer color = new Integer(current_frame[TEAM_COLOR_INDEX]);
        team_color = color.intValue();
        Integer number = new Integer(current_frame[PLAYER_NUMBER_INDEX]);
        player_number = number.intValue();


        // stores locally all the new variables from the log
        thinks_x = new Double(current_frame[MY_X_INDEX]);
        thinks_y = new Double(current_frame[MY_Y_INDEX]);
        thinks_h = new Double(current_frame[MY_H_INDEX]);
        uncertainty_x = new Double(current_frame[UNCERT_X_INDEX]);
        uncertainty_y = new Double(current_frame[UNCERT_Y_INDEX]);
        uncertainty_h = new Double(current_frame[UNCERT_H_INDEX]);
        ball_thinks_x = new Double(current_frame[BALL_X_INDEX]);
        ball_thinks_y = new Double(current_frame[BALL_Y_INDEX]);
        ball_uncert_x = new Double(current_frame[BALL_UNCERT_X_INDEX]);
        ball_uncert_y = new Double(current_frame[BALL_UNCERT_Y_INDEX]);
        ball_velocity_x = new Double(current_frame[BALL_VELOCITY_X_INDEX]);
        ball_velocity_y = new Double(current_frame[BALL_VELOCITY_Y_INDEX]);
        ball_velocity_uncert_x =
            new Double(current_frame[BALL_VELOCITY_UNCERT_X_INDEX]);
        ball_velocity_uncert_y =
            new Double(current_frame[BALL_VELOCITY_UNCERT_Y_INDEX]);
        ball_dist = new Double(current_frame[BALL_DIST_INDEX]);
        ball_bearing = new Double(current_frame[BALL_BEARING_INDEX]);
        odo_x = new Double(current_frame[ODO_X_INDEX]);
        odo_y = new Double(current_frame[ODO_Y_INDEX]);
        odo_h = new Double(current_frame[ODO_H_INDEX]);

        // put all the loc values into debugViewer
        debugViewer.myX.setText("" +thinks_x);
        debugViewer.myY.setText("" + thinks_y);
        debugViewer.myH.setText("" + thinks_h);
        debugViewer.myUncertX.setText("" + uncertainty_x);
        debugViewer.myUncertY.setText("" + uncertainty_y);
        debugViewer.myUncertH.setText("" + uncertainty_h);
        debugViewer.ballX.setText("" + ball_thinks_x);
        debugViewer.ballY.setText("" + ball_thinks_y);
        debugViewer.ballUncertX.setText("" + ball_uncert_x);
        debugViewer.ballUncertY.setText("" + ball_uncert_y);
        debugViewer.ballVelX.setText("" + ball_velocity_x);
        debugViewer.ballVelY.setText("" + ball_velocity_y);
        double absBallVelocity = Math.sqrt(
                                           Math.pow(ball_velocity_x.doubleValue(),2.0)
                                           + Math.pow(ball_velocity_y.doubleValue(),2.0));
	    debugViewer.ballVelAbs.setText("" + absBallVelocity);
        debugViewer.ballVelUncertX.setText("" + ball_velocity_uncert_x);
        debugViewer.ballVelUncertY.setText("" + ball_velocity_uncert_y);
        debugViewer.odoX.setText("" + odo_x);
        debugViewer.odoY.setText("" + odo_y);
        debugViewer.odoH.setText("" + odo_h);

        if (debugViewer.getNumLandmarks() > 0)
            debugViewer.removeLandmarks();

        if (ball_dist > 0) {
            debugViewer.addLandmark(wc.LANDMARK_BALL, ball_dist, ball_bearing);
        }

        // cycle through list of filter 'used' landmarks, adding to
        // debugViewer and painter
        for (int i = NUM_LOG_LOC_INDEXES;
             i < current_frame.length;
             i+= wc.NUM_LANDMARK_VALUES) {

            Double id = new Double(current_frame[i+wc.LANDMARK_ID_INDEX]);

            Double x = new Double(current_frame[i+wc.LANDMARK_X_INDEX]);
            Double y = new Double(current_frame[i+wc.LANDMARK_Y_INDEX]);
            Double dist =
                new Double(current_frame[i+wc.LANDMARK_DIST_INDEX]);
            Double bearing =
                new Double(current_frame[i+wc.LANDMARK_BEARING_INDEX]);

            // add landmarks to DebugViewer
            debugViewer.addLandmark(id.intValue(),dist,bearing);
            // add landmarks to Painter viewer
            painter.sawLandmark(x.intValue(),y.intValue(), team_color);
        }

    }

    public void parseCameraLine(String camera_line) {
        String[] current_frame = camera_line.split(" ");
        Double camera_thinks_x = new Double(current_frame[MY_X_INDEX]);
        Double camera_thinks_y = new Double(current_frame[MY_Y_INDEX]);
        Double camera_thinks_h = new Double(current_frame[MY_H_INDEX]);
        System.out.println("cam thinks x: " + camera_thinks_x +
                           " y: "+ camera_thinks_y + " h: " + camera_thinks_h);

        //LocalizationPacket current_loc = LocalizationPacket.makeDogLocation(camera_thinks_x, camera_thinks_y, camera_thinks_h, 0.);

        //painter.reportUpdatedLocalization(current_loc);
        //painter.reportEndFrame();
    }

    public String formatDoubleForDisplay(Double debug_double) {
        return String.valueOf(debug_double.intValue());
    }
    public int getLogNumFrames() { return log_num_frames; }
    public void setLogMarker(int mark) { log_marker = mark; }
    public int getLogMarker() { return log_marker; }
    public boolean getPaused() { return !playTimer.isRunning(); }
    public void setPlaybackFps(int fps) {
        playTimer.setDelay((int)1000./fps);
    }


    // Begin Monte Carlo stuff, hopefully this will be nicer than the things
    // Which are above and kind of suck.
    /**
     * Initializes  MCL log for super duper Nao fun!
     *
     * @return true if the file succesfully loads, otherwise false.
     */
    public boolean initMCLLog()
    {
        logFile = wc.openDialog("Locate Log File to Load",
                                (System.getProperty("user.dir") +
                                 "/matrixlib/"),FileDialog.LOAD);

        if (logFile == null) {
            System.out.println("log file not chosen");
            wc.setMode(wc.DO_NOTHING);
            return false;
        } else if (!loadMCLLog(logFile)) {
            System.out.println("log file not unable to load");
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
    public boolean loadMCLLog(String logFile)
    {
        // Make sure we have a Nao field and not an Aibo field
        if (wc.the_field == wc.aiboField ) {
            wc.switchFields();
        }

        BufferedReader dataIn = null;
        System.out.println("Loading MCL log file: " + logFile + "... ");
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

        // Strip the team color and player number from the front of the line
        team_color = Integer.parseInt(lineValues[MCL_TEAM_COLOR_INDEX]);
        player_number = Integer.parseInt(lineValues[MCL_PLAYER_NUM_INDEX]);
        // Pull off the data 4 items at a time for each particle
        // The items are (x,y,h,weight)
        for(int i = 2; i < lineValues.length; i += 4) {
            particles.add(new MCLParticle(Float.parseFloat(lineValues[i]),
                                          Float.parseFloat(lineValues[i+1]),
                                          Float.parseFloat(lineValues[i+2]),
                                          Float.parseFloat(lineValues[i+3])));
        }
        return particles;
    }

    /**
     * View the current frame from the log...
     */
    public void viewFromMCLLog()
    {
        Vector< MCLParticle> particles;
        if (log_strings.size() > 0) {
            // set frame total in the log box
            logBox.frameTotal.setText("" + log_num_frames);
            debugViewer.frameTotal.setText("" + log_num_frames);

            // set frame number in log box & debug viewer
            logBox.frameNumber.setText("" + log_marker);
            debugViewer.frameNumber.setText("" + log_marker);

            // checks log_strings if current frame exists
            if (log_strings != null && log_marker < log_strings.size()) {

                // Update the debug viewer

                // Paint the particles on the screen
                particles = parseParticleLine(log_strings.get(log_marker));
                painter.updateParticleSet(particles, team_color, player_number);
                painter.reportEndFrame();
            }
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
        painter.clearSimulationHistory();
    }

}