package edu.bowdoin.robocup.TOOL.WorldController;

import edu.bowdoin.robocup.TOOL.TOOL;
import edu.bowdoin.robocup.TOOL.Image.WorldControllerViewer;

/*import edu.bowdoin.robocup.TOOL.WorldController.Field;
import edu.bowdoin.robocup.TOOL.WorldController.UDPServer;
*/
import edu.bowdoin.robocup.TOOL.Data.Field;
import edu.bowdoin.robocup.TOOL.Data.AiboField2008;
import edu.bowdoin.robocup.TOOL.Data.NaoField2008;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.event.*;
import java.util.Date;
import java.util.Vector;
import java.util.Iterator;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.*;
import java.lang.Process;

//import java.util.*;

public class WorldController extends JPanel implements KeyListener,
						       ActionListener,
						       ChangeListener
{
    //@todo integrate lines into simulation filter
    //@todo force the simulator to restart when a new simulation made is
    // selected
    // DEBUG SWITCHES
    private boolean DEBUG_RECV_ODOMETRY = false;
    private boolean DEBUG_RECV_VISION = false;
    private boolean DEBUG_RECV_LOCALIZATION = false;

    // Instance of the physical field
    Field the_field;
    AiboField2008 aiboField;
    NaoField2008 naoField;

    // Paints on the field
    WorldControllerPainter painter;
    // Displays the field + the painted info
    WorldControllerViewer viewer;

    // Instance of the DebugViewer Class
    DebugViewer debugViewer;

    // To handle recorder log input, parsing, and passing to display
    LogHandler log;

    // Object to handle the log 2.0 version of recording EKF off of robots
    LogHandler ekfLog;

    // Listen to live UDP data, used to visualize multiple dogs' world states
    // simultaneously
    UDPServer udp_server;

    // Modes
    public static final int DO_NOTHING = -1;
    public static final int SIMULATE_OFFLINE = 0;
    public static final int SIMULATE_WITH_DOG_DATA = 1;
    public static final int VIEW_DOG_EKF = 2;
    public static final int VIEW_DOG_LOG = 3;
    public static final int VIEW_UDP_PACKETS = 4;
    public static final int VIEW_EKF_LOG = 5;

    // Speed paramaters
    static final double MAX_SPEED_VARIABLE = 13.;
    static final double MIN_SPEED_VARIABLE = -13.;
    static final double MAX_SPEED_FORWARD = 43.;
    static final double MAX_SPEED_BACKWARD = 17.;
    static final double MAX_SPEED_LATERAL = 25.;
    static final double MAX_SPEED_ROTATE = 162.;

    public static final int ROBOT_FPS = 30;
    public static final int DRAWING_FPS = ROBOT_FPS;
    public static final int LOG_FPS = 25;
    public static final int IO_FPS = 400;
    private static final int CHECK_INTERVAL = 100; // in milliseconds
    private static final int NUM_TICKS_FOR_UNSUCCESSFUL = 5;

    // slider for playback constants
    public static final int FPS_SLIDE_MIN = 1;
    public static final int FPS_SLIDE_MAX = 60;
    public static final int FPS_SLIDE_INIT = 25;
    public static final int NUM_DISPLAY_ESTIMATES_SLIDE_MIN = 1;
    public static final int NUM_DISPLAY_ESTIMATES_SLIDE_MAX = 30;
    public static final int NUM_DISPLAY_ESTIMATES_SLIDE_INIT = 5;

    // TCP Constants
    // data coming off dog via tcp.
    static final int TCP_NUM_LOC_DATA = 21;

    // Loc Debug Data Indexes
    public final static int TCP_LOC_MY_X = 0;
    public final static int TCP_LOC_MY_Y = 1;
    public final static int TCP_LOC_MY_H = 2;
    public final static int TCP_LOC_MY_UNCERT_X = 3;
    public final static int TCP_LOC_MY_UNCERT_Y = 4;
    public final static int TCP_LOC_MY_UNCERT_H = 5;
    public final static int TCP_LOC_BALL_X = 6;
    public final static int TCP_LOC_BALL_Y = 7;
    public final static int TCP_LOC_BALL_UNCERT_X = 8;
    public final static int TCP_LOC_BALL_UNCERT_Y = 9;
    public final static int TCP_LOC_BALL_VEL_X = 10;
    public final static int TCP_LOC_BALL_VEL_Y = 11;
    public final static int TCP_LOC_BALL_UNCERT_VEL_X = 12;
    public final static int TCP_LOC_BALL_UNCERT_VEL_Y = 13;
    public final static int TCP_LOC_BALL_DIST = 14;
    public final static int TCP_LOC_BALL_BEARING = 15;
    public final static int TCP_LOC_ODO_X = 16;
    public final static int TCP_LOC_ODO_Y = 17;
    public final static int TCP_LOC_ODO_H = 18;
    public final static int TCP_LOC_TEAM_COLOR = 19;
    public final static int TCP_LOC_PLAYER_NUMBER = 20;


    static final int NUM_LANDMARKS = 21;

    // Landmark Constants
    public final static int LANDMARK_CORNER = -1;
    public final static int LANDMARK_MY_GOAL_LEFT_POST = 0;
    public final static int LANDMARK_MY_GOAL_RIGHT_POST = 1;
    public final static int LANDMARK_OPP_GOAL_LEFT_POST = 2;
    public final static int LANDMARK_OPP_GOAL_RIGHT_POST = 3;
    public final static int LANDMARK_LEFT_BEACON = 4;
    public final static int LANDMARK_RIGHT_BEACON = 5;
    public final static int LANDMARK_BALL = 6;
    public final static int LANDMARK_MY_CORNER_LEFT_L = 7;
    public final static int LANDMARK_MY_CORNER_RIGHT_L = 8;
    public final static int LANDMARK_MY_GOAL_LEFT_T = 9;
    public final static int LANDMARK_MY_GOAL_RIGHT_T = 10;
    public final static int LANDMARK_MY_GOAL_LEFT_L = 11;
    public final static int LANDMARK_MY_GOAL_RIGHT_L = 12;
    public final static int LANDMARK_CENTER_LEFT_T = 13;
    public final static int LANDMARK_CENTER_RIGHT_T = 14;
    public final static int LANDMARK_OPP_CORNER_LEFT_L = 15;
    public final static int LANDMARK_OPP_CORNER_RIGHT_L = 16;
    public final static int LANDMARK_OPP_GOAL_LEFT_T = 17;
    public final static int LANDMARK_OPP_GOAL_RIGHT_T = 18;
    public final static int LANDMARK_OPP_GOAL_LEFT_L = 19;
    public final static int LANDMARK_OPP_GOAL_RIGHT_L = 20;

    // Landmark Tcp Message Format
    public final static int NUM_LANDMARK_VALUES = 5;
    public final static int LANDMARK_ID_INDEX = 0;
    public final static int LANDMARK_X_INDEX = 1;
    public final static int LANDMARK_Y_INDEX = 2;
    public final static int LANDMARK_DIST_INDEX = 3;
    public final static int LANDMARK_BEARING_INDEX = 4;

    // Button Strings
    public final static String PROGRAM_LABEL_STRING = "Options:";
    public final static String PROGRAM_SPECIFIC_LABEL_STRING="Option Specific:";
    public final static String SIMULATION_LABEL_STRING = "Simulation:";
    public final static String UDP_LABEL_STRING = "UDP Viewer:";
    public final static String CONNECTION_LABEL_STRING = "Connection:";
    public final static String FPS_LABEL_STRING = "FPS Playback Speed:";
    public final static String NUM_DISPLAY_ESTIMATES_LABEL_STRING = "# Estimates Displayed";
    public final static String SWITCH_FIELDS_STRING = "Switch Fields";
    public final static String SWITCH_COLORS_STRING = "Switch Colors";
    public final static String SWITCH_TEAMS_STRING = "Switch Teams";
    public final static String CLEAR_EKF_STRING = "Clear EKF";
    public final static String CLEAR_FIELD_STRING = "Clear Field";
    public final static String DRAW_EST_STRING = "Start/Stop Draw Est";
    public final static String DRAW_REAL_STRING = "Start/Stop Draw Real";
    public final static String RELOAD_LOG_STRING = "Reload Log";
    public final static String CONNECT_STRING = "Connect";
    public final static String DISCONNECT_STRING = "Disconnect";
    public final static String QUIT_LOG_STRING = "Quit Log";
    public final static String CONNECT_QUESTION = "Address?";
    public final static String CONNECT_TITLE = "Connect to robot";
    public final static String CONNECT_MESSAGE = "Connecting...";
    public final static String CONNECT_TO_MESSAGE = "Connecting to host";
    public final static String CONNECTED_MESSAGE = "Connected!";

    // Button Action Commands

    public static final String SWITCH_FIELDS_ACTION = "switchfields";
    public static final String SWITCH_COLORS_ACTION = "switchcolors";
    public static final String SWITCH_TEAMS_ACTION = "switchteams";
    public static final String CLEAR_EKF_ACTION = "clearekf";
    public static final String DRAW_REAL_ACTION = "drawreal";
    public static final String DRAW_EST_ACTION = "drawest";
    public static final String RELOAD_LOG_ACTION = "reloadlog";
    public static final String RELOAD_EKF_LOG_ACTION = "reloadekflog";
    public static final String QUIT_LOG_ACTION = "quitlog";
    public static final String QUIT_EKF_LOG_ACTION = "quitekflog";
    public static final String CONNECT_ACTION = "connectme";
    public static final String DISCONNECT_ACTION = "disconnectme";
    public static final String CLEAR_FIELD_ACTION = "clearfield";

    // Menu Strings
    public static final String MODE_STRING = "Mode";
    public static final String SIMULATE_OFFLINE_STRING = "Simulate Offline";
    public static final String SIMULATE_WITH_DOG_DATA_STRING = "Simulate with Robot Data";
    public static final String VIEW_DOG_EKF_STRING = "View Robot EKF";
    public static final String VIEW_ROBOT_MCL_STRING = "View Robot Particles";
    public static final String VIEW_DOG_LOG_STRING = "View Robot Log";
    public static final String VIEW_EKF_LOG_STRING = "View EKF Log";
    public static final String VIEW_UDP_PACKETS_STRING = "View UDP Packets";
    public static final String CIRCLE_LINE_GOALIE_STRING = "Circle/Line/Goalie";
    public static final String GO_TO_XY_NORMAL_STRING = "Go To XY Normal";
    public static final String GO_TO_XY_LOOK_AT_POINT_STRING = "Go To XY, Look at Point";

    // Menu Action Commands
    public static final String SIMULATE_OFFLINE_ACTION = "simulate offlineme";
    public static final String SIMULATE_WITH_DOG_DATA_ACTION = "simulate robot data";
    public static final String VIEW_DOG_EKF_ACTION = "view robot ekfs";
    public static final String VIEW_ROBOT_MCL_ACTION = "view robot mcl";
    public static final String VIEW_DOG_LOG_ACTION = "viewrobotlog";
    public static final String VIEW_EKF_LOG_ACTION = "viewekflog";
    public static final String VIEW_UDP_PACKETS_ACTION = "viewudpaction";
    public static final String CIRCLE_LINE_GOALIE_ACTION = "circlelinegoalie";
    public static final String GO_TO_XY_NORMAL_ACTION = "gotoxy";
    public static final String GO_TO_XY_LOOK_AT_POINT_ACTION = "gotoxy lookatpoint";

    // Values for the grid bag layout -- affects proportions of field and button
    // area
    final static int FIELD_WEIGHT = 10;
    final static int BUTTON_AREA_WEIGHT = 1;
    final static int BUTTON_AREA_WIDTH = 75;

    // AiboConnect connection stuff
    private Aibo aibo;

    // Connect thread
    private Thread connect_thread;
    private JDialog connect_dialog;

    // Viewing dog EKF thread
    private Thread viewDogEKFThread;

    // simulation getters
    private double thinks_looking_at_x, thinks_looking_at_y;
    private double real_x,real_y,real_heading;
    private double real_looking_at_x, real_looking_at_y;

    // for streamed vision
    private double received_pb_dist, received_pb_heading;
    private double received_bp_dist, received_bp_heading;
    private double received_yg_dist, received_yg_heading;
    private double received_bg_dist, received_bg_heading;

    // for streamed odometry
    private double x_change, y_change, a_change;

    // controls total playback_fps
    private int playback_fps;

    // for the connection counter we need to remember how many ticks have passed
    // since we started waiting for a successful dog connection
    private Timer waitToConnect;
    private int numWaitTimerTicks;

    // button declarations
    private JPanel button_area;
    private JLabel program_label;
    private JLabel program_specific_label;
    private JLabel connection_label;
    private JLabel fps_label;
    private JLabel num_display_estimates_label;
    private JButton button_switch_fields;
    private JButton button_view_dog_ekf;
    private JButton button_view_robot_mcl;
    private JButton button_view_udp_packets;
    private JButton button_view_dog_log;
    private JButton button_view_ekf_log;
    private JButton button_one;
    private JButton button_two;
    private JButton button_three;
    private JButton connect_button;
    private JButton draw_real_button;
    private JButton draw_est_button;
    private JSlider fps_slide;
    private JSlider num_display_estimates_slide;

    // for key listener
    private boolean key_pressed;

    private int mode;

    public WorldController(TOOL t)
    {
        aibo = new Aibo();

        aiboField = new AiboField2008();
        naoField = new NaoField2008();

        the_field = naoField;
        viewer = new WorldControllerViewer(the_field);
        painter = viewer.getPainter();

        debugViewer = new DebugViewer(this, (int)(the_field.getPreferredSize().
                                                  getWidth() +
                                                  BUTTON_AREA_WIDTH + 150), 0);
        log = new LogHandler(this, painter, debugViewer);
        ekfLog = new LogHandler(this, painter, debugViewer);
        udp_server = new UDPServer();
        udp_server.addDogListener(painter);

        setSize((int) (the_field.getPreferredSize().getWidth() +
                       BUTTON_AREA_WIDTH),
                (int) (the_field.getPreferredSize().getHeight()));

        // The GUI will use a GridBag layout to manipulate the visible objects
        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        setLayout(gridbag);

        // Field will fill all the space alotted
        c.fill = GridBagConstraints.BOTH;
        // And take up all vertical space
        c.gridheight = GridBagConstraints.REMAINDER;
        // Field will be weighted in the horizontal greater than button area
        c.weightx = FIELD_WEIGHT;
        c.weighty = FIELD_WEIGHT;

        // Set the constraints for the Field and add it to the frame
        gridbag.setConstraints(viewer,c);
        add(viewer);

        // Create a new panel to hold the buttons, dubbed the buttonArea
        button_area = new JPanel();
        button_area.setLayout(new GridLayout(20,1));
        // Button area will be weighted less than the Field
        //c.weightx = BUTTON_AREA_WEIGHT;
        //c.weighty = BUTTON_AREA_WEIGHT;

        c.weightx = GridBagConstraints.REMAINDER;
        c.weighty = GridBagConstraints.REMAINDER;

        initButtons();
        // Set the constraints for the button area and add it to the frame
        add(button_area);
        gridbag.setConstraints(button_area,c);

        playback_fps = ROBOT_FPS;

        setFocusable(true);
        requestFocusInWindow();

        addKeyListener(this);

        setVisible(true);

        // Initialize mode
        mode = DO_NOTHING;

        /* Setup a timer that will wait for a connection from a dog.
           It will check every tick, and if it waits for more than some
           number of ticks, it will call the connection unsuccessful */
        numWaitTimerTicks = 0;
        // This will be the method called when the timer ticks.
        ActionListener taskPerformer = new ActionListener () {
                public void actionPerformed (ActionEvent evt)
                {
                    // Check if we have successfully connected
                    if (aibo.isConnected()) {
                        System.out.println("Aibo is connected");
                        connect_dialog.setVisible(false);
                        numWaitTimerTicks = 0;
                        waitToConnect.stop();
                        startDogEKFThread();
                    } else {
                        numWaitTimerTicks++;
                        if (numWaitTimerTicks > NUM_TICKS_FOR_UNSUCCESSFUL) {
                            // It took too long to establish connection.
                            // Give up.
                            numWaitTimerTicks = 0;
                            waitToConnect.stop();

                            JOptionPane.showMessageDialog
                                (null,
                                 Aibo.ERROR_MSGS[aibo.getStatus()],
                                 CONNECT_TITLE,JOptionPane.ERROR_MESSAGE);
                            aibo.disconnect();
                        }
                    }
                }
            };
        waitToConnect = new Timer(CHECK_INTERVAL, taskPerformer);
    } // end constructor


    // This is how WorldController used to work standalone
    // This method would get called forever and the methods inside would
    // sleep in order to keep the processor from being used 100%. I'm
    // keeping this here for now, as a reference.
    public void run()
    {
        while (true) {
            // calls thread sleep for fps millis
            delayFor(ROBOT_FPS);
            if (mode == DO_NOTHING) {

            }
            else if (mode == SIMULATE_WITH_DOG_DATA) {

            }
            else if (mode == VIEW_DOG_EKF) {
                viewDogEKF();
            }
            else if (mode == VIEW_DOG_LOG) {
                log.viewFromLog();
            }
            else if (mode == VIEW_UDP_PACKETS) {
                // server is on its own thread, should work on its own
                // We just need to tell the field that a frame has passed
                // and that it should redraw accordingly, since it has no
                // other way to understand the passage of time when its just
                //listens in to the udp packets
                painter.reportEndFrame();
            }
            else if (mode == VIEW_EKF_LOG) {
                ekfLog.viewFromLog();
            }
        }
    }


    /**
     * Method switches between AIBO and Nao fields dimensions and landmarks
     */
    public void switchFields()
    {
        if (the_field == aiboField) {
            the_field = naoField;
        }
        else if (the_field == naoField) {
            the_field = aiboField;
        }
        // Takes care of sending info to the painter as well
        viewer.setField(the_field);
    }


    /**
     * Method required by ActionListener to deal with the various actions
     * invoked by users
     *
     * @param ActionEvent e The action event associated with the current user
     * action
     */
    public void actionPerformed(ActionEvent e)
    {
        String cmd = e.getActionCommand();

        // BUTTON ACTIONS
        if (cmd.equals(CLEAR_EKF_ACTION)) {
            //test_filter.initialize();
        } else if (cmd.equals(SWITCH_FIELDS_ACTION)) {
            switchFields();
        } else if (cmd.equals(DISCONNECT_ACTION)) {
            // Currently listening on TCP
            if (mode == VIEW_DOG_EKF) {
                disconnect();
            //UDP
            } else if (mode == VIEW_UDP_PACKETS) {
                udp_server.setReceiving(false);
            }
            startDoNothing();
        }
        else if (cmd.equals(CLEAR_FIELD_ACTION)) {
            painter.clearSimulationHistory();
        }
        else if (cmd.equals(QUIT_LOG_ACTION)) {
            log.quitDogLog();
            startDoNothing();
        } else if (cmd.equals(QUIT_EKF_LOG_ACTION)) {
            ekfLog.quitDogLog();
            startDoNothing();
        } else if (cmd.equals(DRAW_REAL_ACTION)) {
            if (painter.getDrawReal()) {
                painter.setDrawReal(false);
                draw_real_button.setText("Draw Real");
            } else {
                painter.setDrawReal(true);
                draw_real_button.setText("Stop Draw Real");
            }
        } else if (cmd.equals(DRAW_EST_ACTION)) {
            if (painter.getDrawEst()) {
                painter.setDrawEst(false);
                //draw_est_button.setText("Draw Est");
            } else {
                painter.setDrawEst(true);
                //draw_est_button.setText("Stop Draw Est");
            }
        } else if (cmd.equals(RELOAD_LOG_ACTION)) {
            startDogLog();
            System.out.println("RELOADED DOG LOG");
        } else if (cmd.equals(RELOAD_EKF_LOG_ACTION)) {
            startEKFLog();
            System.out.println("RELOADED EKF LOG");
        } else if (cmd.equals(SIMULATE_WITH_DOG_DATA_ACTION)) {
            mode = SIMULATE_WITH_DOG_DATA;
            if (!aibo.isConnected()) connect();
        } else if (cmd.equals(VIEW_DOG_EKF_ACTION)) {
            startDogEKF();
        }
        else if (cmd.equals(VIEW_DOG_LOG_ACTION)) {
            startDogLog();
            //this.setVisible(true); // keeps focus
        } else if (cmd.equals(VIEW_UDP_PACKETS_ACTION)) {
            startDogUDP();
        } else if (cmd.equals(VIEW_EKF_LOG_ACTION)) {
            startEKFLog();
        }

        // keeps keyboard focus
        setFocusable(true);
        requestFocusInWindow();
    }

    /**
     * Start the TCP thread listenting to the Robot
     */
    private void startDogEKFThread()
    {
        startDogEKF();
        viewDogEKFThread = new Thread(new Runnable() {
                public void run() {
                    while (aibo.isConnected()) {
                        viewDogEKF();
                        delayFor(ROBOT_FPS);
                    }
                }
            });
        viewDogEKFThread.start();
    }

    /** Connect to dog, have it stream localization info, just use this program
     *  as a display
     *
     */
    public void viewDogEKF() {

        String loc_receive = aibo.sendCommand("sendLandmarks");
        //System.out.println("loc_receive: " + loc_receive);

        if (loc_receive != null) {
            //if (loc_receive ==

            // split string into an array of values
            String[] response_values = loc_receive.split(" ");
            // check to see if length is less than min (and error if so)
            if (response_values.length < TCP_NUM_LOC_DATA) {
                System.out.println("viewDogEKF() received " +
                                   response_values.length +
                                   " values, expected more than " +
                                   TCP_NUM_LOC_DATA + " string is: " +
                                   loc_receive);
                return;
            }
            debugViewer.removeLandmarks();

            // parse loc data
            Double my_x = new Double(response_values[TCP_LOC_MY_X]);
            Double my_y = new Double(response_values[TCP_LOC_MY_Y]);
            Double my_h = new Double(response_values[TCP_LOC_MY_H]);
            Double my_pan = 0.;
            Double my_uncert_x =
                new Double(response_values[TCP_LOC_MY_UNCERT_X]);
            Double my_uncert_y =
                new Double(response_values[TCP_LOC_MY_UNCERT_Y]);
            Double my_uncert_h =
                new Double(response_values[TCP_LOC_MY_UNCERT_H]);
            Double ball_x = new Double(response_values[TCP_LOC_BALL_X]);
            Double ball_y = new Double(response_values[TCP_LOC_BALL_Y]);
            Double ball_uncert_x =
                new Double(response_values[TCP_LOC_BALL_UNCERT_X]);
            Double ball_uncert_y =
                new Double(response_values[TCP_LOC_BALL_UNCERT_Y]);
            Double ball_vel_x = new Double(response_values[TCP_LOC_BALL_VEL_X]);
            Double ball_vel_y = new Double(response_values[TCP_LOC_BALL_VEL_Y]);
            Double ball_uncert_vel_x =
                new Double(response_values[TCP_LOC_BALL_UNCERT_VEL_X]);
            Double ball_uncert_vel_y =
                new Double(response_values[TCP_LOC_BALL_UNCERT_VEL_Y]);
            Double ball_dist = new Double(response_values[TCP_LOC_BALL_DIST]);
            Double ball_bearing =
                new Double(response_values[TCP_LOC_BALL_BEARING]);
            Double odo_x = new Double(response_values[TCP_LOC_ODO_X]);
            Double odo_y = new Double(response_values[TCP_LOC_ODO_Y]);
            Double odo_h = new Double(response_values[TCP_LOC_ODO_H]);
            Double dbl_color = new Double(response_values[TCP_LOC_TEAM_COLOR]);
            int team_color = dbl_color.intValue();
            Double dbl_number =
                new Double(response_values[TCP_LOC_PLAYER_NUMBER]);
            int player_number = dbl_number.intValue();

            // update the debug viewer with loc data
            debugViewer.myX.setText("" + my_x);
            debugViewer.myY.setText("" + my_y);
            debugViewer.myH.setText("" + my_h);
            debugViewer.myUncertX.setText("" + my_uncert_x);
            debugViewer.myUncertY.setText("" + my_uncert_y);
            debugViewer.myUncertH.setText("" + my_uncert_h);
            debugViewer.ballX.setText("" + ball_x);
            debugViewer.ballY.setText("" + ball_y);
            debugViewer.ballUncertX.setText("" + ball_uncert_x);
            debugViewer.ballUncertY.setText("" + ball_uncert_y);
            debugViewer.ballVelX.setText("" + ball_vel_x);
            debugViewer.ballVelY.setText("" + ball_vel_y);
            double absBallVelocity = Math.sqrt
                (Math.pow(ball_vel_x.doubleValue(),2.0)
                 + Math.pow(ball_vel_y.doubleValue(),2.0));
            debugViewer.ballVelAbs.setText("" + absBallVelocity);
            debugViewer.ballVelUncertX.setText("" + ball_uncert_vel_x);
            debugViewer.ballVelUncertY.setText("" + ball_uncert_vel_y);
            debugViewer.odoX.setText("" + odo_x);
            debugViewer.odoY.setText("" + odo_y);
            debugViewer.odoH.setText("" + odo_h);

            // parse rest of string for landmark info
            for (int i = TCP_NUM_LOC_DATA;
                 i < response_values.length; i+= NUM_LANDMARK_VALUES) {
                Double id = new Double(response_values[i+LANDMARK_ID_INDEX]);
                Double x = new Double(response_values[i+LANDMARK_X_INDEX]);
                Double y = new Double(response_values[i+LANDMARK_Y_INDEX]);
                Double dist =
                    new Double(response_values[i+LANDMARK_DIST_INDEX]);
                Double bearing =
                    new Double(response_values[i+LANDMARK_BEARING_INDEX]);

                // add landmarks to DebugViewer
                debugViewer.addLandmark(id.intValue(),dist,bearing);
                // add landmarks to Field viewer
                painter.sawLandmark(x.intValue(),y.intValue(), team_color);
            }

            if (ball_dist > 0) {
                debugViewer.addLandmark(LANDMARK_BALL,ball_dist,ball_bearing);
            }

            // update ball data structure
            LocalizationPacket current_ball_localization_info =
                LocalizationPacket.
                makeBallEstimateAndUncertPacket(ball_x,
                                                ball_y,
                                                ball_uncert_x,
                                                ball_uncert_y,
                                                ball_vel_x,
                                                ball_vel_y);
            painter.
                reportUpdatedBallLocalization(current_ball_localization_info,
                                              team_color, player_number);

            // update self localization data structure
            LocalizationPacket current_localization_info =
                LocalizationPacket.
                makeEstimateAndUncertPacket(my_x,
                                            my_y,
                                            my_h,
                                            my_pan,
                                            my_uncert_x,
                                            my_uncert_y,
                                            my_uncert_h);
            painter.reportUpdatedLocalization(current_localization_info,
                                              team_color, player_number);
            painter.reportEndFrame();

        }
        repaint();
    }

    ///###### JAVA FRAME METHODS %%%%%%%/

    public void initButtons()
    {
        /* BUTTON AREA */

        // program label
        program_label = new JLabel(PROGRAM_LABEL_STRING, JLabel.CENTER);
        program_label.setAlignmentX(Component.CENTER_ALIGNMENT);
        button_area.add(program_label);


        button_switch_fields = new JButton(SWITCH_FIELDS_STRING);
        button_switch_fields.setActionCommand(SWITCH_FIELDS_ACTION);
        button_switch_fields.addActionListener(this);
        button_area.add(button_switch_fields);

        // setup dog ekf (tcp) buttons
        button_view_dog_ekf = new JButton(VIEW_DOG_EKF_STRING);
        button_view_dog_ekf.setActionCommand(VIEW_DOG_EKF_ACTION);
        button_view_dog_ekf.addActionListener(this);
        button_area.add(button_view_dog_ekf);

        // setup udp packets button
        button_view_udp_packets = new JButton(VIEW_UDP_PACKETS_STRING);
        button_view_udp_packets.setActionCommand(VIEW_UDP_PACKETS_ACTION);
        button_view_udp_packets.addActionListener(this);
        button_area.add(button_view_udp_packets);

        // setup dog log button
        button_view_dog_log = new JButton(VIEW_DOG_LOG_STRING);
        button_view_dog_log.setActionCommand(VIEW_DOG_LOG_ACTION);
        button_view_dog_log.addActionListener(this);
        button_area.add(button_view_dog_log);

        // setup ekf log button
        button_view_ekf_log = new JButton(VIEW_EKF_LOG_STRING);
        button_view_ekf_log.setActionCommand(VIEW_EKF_LOG_ACTION);
        button_view_ekf_log.addActionListener(this);
        button_area.add(button_view_ekf_log);

        // program specific label
        program_specific_label = new JLabel(PROGRAM_SPECIFIC_LABEL_STRING,
					    JLabel.CENTER);
        program_specific_label.setAlignmentX(Component.CENTER_ALIGNMENT);
        button_area.add(program_specific_label);

        button_one = new JButton(DISCONNECT_STRING);
        //button_one.setActionCommand(DISCONNECT_ACTION);
        button_one.addActionListener(this);
        button_area.add(button_one);
        button_one.setVisible(false);

        button_two = new JButton(DISCONNECT_STRING);
        //button_two.setActionCommand(DISCONNECT_ACTION);
        button_two.addActionListener(this);
        button_area.add(button_two);
        button_two.setVisible(false);

        button_three = new JButton(DISCONNECT_STRING);
        //button_three.setActionCommand(DISCONNECT_ACTION);
        button_three.addActionListener(this);
        button_area.add(button_three);
        button_three.setVisible(false);

        // fps playback
        fps_label = new JLabel(FPS_LABEL_STRING, JLabel.CENTER);
        fps_label.setAlignmentX(Component.CENTER_ALIGNMENT);
        fps_label.setVisible(false);
        button_area.add(fps_label);
        fps_slide = new JSlider(JSlider.HORIZONTAL, FPS_SLIDE_MIN,
                                FPS_SLIDE_MAX,
                                FPS_SLIDE_INIT);
        fps_slide.addChangeListener(this);
        button_area.add(fps_slide);
        fps_slide.setVisible(false);

        // estimates slide
        num_display_estimates_label = new JLabel
            (NUM_DISPLAY_ESTIMATES_LABEL_STRING,
             JLabel.CENTER);
        num_display_estimates_label.setAlignmentX(Component.CENTER_ALIGNMENT);
        num_display_estimates_label.setVisible(false);
        button_area.add(num_display_estimates_label);
        num_display_estimates_slide = new JSlider
            (JSlider.HORIZONTAL,
             NUM_DISPLAY_ESTIMATES_SLIDE_MIN,
             NUM_DISPLAY_ESTIMATES_SLIDE_MAX,
             NUM_DISPLAY_ESTIMATES_SLIDE_INIT);
        num_display_estimates_slide.addChangeListener(this);
        button_area.add(num_display_estimates_slide);
        num_display_estimates_slide.setVisible(false);
    }

    public void nothingButtons()
    {
        button_one.setVisible(false);
        button_two.setVisible(false);
        button_three.setVisible(false);
        fps_label.setVisible(false);
        fps_slide.setVisible(false);
        num_display_estimates_label.setVisible(false);
        num_display_estimates_slide.setVisible(false);
    }

    public void dogEKFButtons()
    {
        button_one.setText(DISCONNECT_STRING);
        button_one.setActionCommand(DISCONNECT_ACTION);
        button_one.setVisible(true);
        button_two.setText(CLEAR_FIELD_STRING);
        button_two.setActionCommand(CLEAR_FIELD_ACTION);
        button_two.setVisible(true);
    }

    public void dogLogButtons()
    {
        button_one.setText(DRAW_EST_STRING);
        button_one.setActionCommand(DRAW_EST_ACTION);
        button_one.setVisible(true);
        button_two.setText(RELOAD_LOG_STRING);
        button_two.setActionCommand(RELOAD_LOG_ACTION);
        button_two.setVisible(true);
        button_three.setText(QUIT_LOG_STRING);
        button_three.setActionCommand(QUIT_LOG_ACTION);
        button_three.setVisible(true);
        fps_label.setVisible(true);
        fps_slide.setVisible(true);
        num_display_estimates_label.setVisible(true);
        num_display_estimates_slide.setVisible(true);
    }
    public void ekfLogButtons()
    {
        button_one.setText(DRAW_EST_STRING);
        button_one.setActionCommand(DRAW_EST_ACTION);
        button_one.setVisible(true);
        button_two.setText(RELOAD_LOG_STRING);
        button_two.setActionCommand(RELOAD_EKF_LOG_ACTION);
        button_two.setVisible(true);
        button_three.setText(QUIT_LOG_STRING);
        button_three.setActionCommand(QUIT_EKF_LOG_ACTION);
        button_three.setVisible(true);
        fps_label.setVisible(true);
        fps_slide.setVisible(true);
        num_display_estimates_label.setVisible(true);
        num_display_estimates_slide.setVisible(true);
    }
    public void udpButtons()
    {
        button_one.setText(DISCONNECT_STRING);
        button_one.setActionCommand(DISCONNECT_ACTION);
        button_one.setVisible(true);
        button_two.setText(CLEAR_FIELD_STRING);
        button_two.setActionCommand(CLEAR_FIELD_ACTION);
        button_two.setVisible(true);
    }


    // master method for getting dog log working
    public void startDogLog()
    {
        if(!log.initDogLog()) {
            return;
        }
        nothingButtons(); // nulls out all buttons (in case switching options)
        dogLogButtons();
        mode = VIEW_DOG_LOG;
    }

    public void startEKFLog()
    {
        if(!ekfLog.initEKFLog()) {
            return;
        }
        nothingButtons();
        ekfLogButtons();
        mode = VIEW_EKF_LOG;
    }

    // master method for getting dog tcp (ekf) working
    public void startDogEKF()
    {
        // check connection, and if not, connect
        if (!aibo.isConnected()) {
            System.out.println("Aibo not connected. Connecting...");
            connect();
        }
        // check connection again, if so, go into dog ekf
        if (aibo.isConnected()) {
            System.out.println("Connected to Aibo");
            nothingButtons(); // nulls out all buttons
            dogEKFButtons();
            mode = VIEW_DOG_EKF;
            debugViewer.setVisible(true);
        } else {
            // else go back to DO_NOTHING
            mode = DO_NOTHING;
        }
    }

    // master method for getting dog udp working
    public void startDogUDP()
    {
        nothingButtons();
        udpButtons();
        painter.prepareForUDP();
        udp_server.setReceiving(true);
        mode = VIEW_UDP_PACKETS;
        painter.setPositionsToDraw(1);
    }

    public void startDoNothing()
    {
        nothingButtons(); // clears buttons
        painter.clearSimulationHistory(); // clears field display
        mode = DO_NOTHING;
    }

    public String openDialog(String title, String dir, int mode)
    {
        FileDialog fd = new FileDialog(new Frame(),title,mode);
        fd.setDirectory(dir);
        fd.setVisible(true);
        String fullpath = fd.getDirectory() + fd.getFile();
        fd.dispose();
        if (fd.getFile() == null) return null;
        return fullpath;
    }

    public void delayFor(long fps)
    {
        // slows down updates to a DRAWING_FPS.
        try {
            Thread.sleep((long)(1000./fps));
        }catch (InterruptedException e) {
        }
    }

    // returns millis from start up
    public long getTime() { return new Date().getTime(); }

    //#######
    // CONNECTION TO AIBO METHODS
    //#######
    ///////////////////////////////
    // Stable connection methods //
    ///////////////////////////////

    /**
     * Method displays a joption pane asksing for a robot to connect to.
     * Connects if the specified robot is active and currently disconnected.
     */
    public void connect()
    {
        if (checkConnecting()) {
            return;
        }
        disconnect();

        final JOptionPane optionPane = new JOptionPane(CONNECT_QUESTION,
                                                 JOptionPane.QUESTION_MESSAGE,
                                                 JOptionPane.OK_CANCEL_OPTION);
        optionPane.setWantsInput(true);
        //connect_dialog = new JDialog(this,CONNECT_TITLE,true);
        connect_dialog = optionPane.createDialog(null, CONNECT_TITLE);
        connect_dialog.setContentPane(optionPane);
        connect_dialog.setDefaultCloseOperation(JDialog.DO_NOTHING_ON_CLOSE);
        optionPane.addPropertyChangeListener(
                                             new PropertyChangeListener() {
                    public void propertyChange(PropertyChangeEvent e) {
                        String prop = e.getPropertyName();
                        Object value = e.getNewValue();

                        if (prop.equals("value")) {
                            if (value.equals(JOptionPane.OK_OPTION)) {
                                connect_dialog.setTitle(CONNECT_MESSAGE);

                                if (!aibo.isConnecting()) {
				    startConnecting(
						    optionPane.getInputValue().toString());
				    // start waiting for a connection
				    // timer defined in WorldController constructor
				    waitToConnect.start();
				}
                            }else {
                                aibo.stopConnecting();
                                connect_dialog.setVisible(false);
                            }
                        }
                    }
                });

	connect_dialog.pack();
        int x = (int)(getLocation().getX() + getWidth()/2 -
                connect_dialog.getWidth()/2);
        int y = (int)(getLocation().getY() + getHeight()/2 -
                connect_dialog.getHeight()/2);
        connect_dialog.setLocation(x,y);
        connect_dialog.setVisible(true);
    }

    private void startConnecting(String host) {
        final String address = host;
        //dogName = host;
        connect_thread = new Thread(new Runnable() {
            public void run() {
                aibo.connectTo(address);
                if (connect_dialog.isVisible()) {
                    connect_dialog.setTitle(CONNECTED_MESSAGE);
                    connect_dialog.setVisible(false);
                }
            }
        });
        connect_thread.start();
    }

    public boolean checkConnecting() {
        boolean result = isConnecting();
        if (result)
            JOptionPane.showMessageDialog(this,"The previous connection " +
                    "attempt's Socket has not closed yet");
        return result;
    }

    public boolean isConnecting() {
        if (connect_thread!=null && connect_thread.isAlive())
            return true;
        return false;
    }

    public boolean ensureConnection() {
        if (!aibo.isConnected()) {
            connect();
            while (aibo.isConnecting()) {
                try {
                    Thread.sleep(100);
                }catch (InterruptedException e) {}
            }
            return aibo.isConnected();
        }else
            return true;
    }

    public void disconnect() {
        aibo.disconnect();
    }



    public void keyPressed(KeyEvent k)
    {
        int key = k.getKeyCode();

        if (mode == DO_NOTHING) {
            switch (key) {
            case KeyEvent.VK_L:
                startDogLog();
                this.setVisible(true);
                break;
            case KeyEvent.VK_V:
                startDogEKF();
                break;
            case KeyEvent.VK_U:
                startDogUDP();
                break;
            }
        }
        else if (mode == VIEW_DOG_LOG) {
            switch (key) {
            case KeyEvent.VK_R:
                startDogLog();
                System.out.println("RELOADED LOG FILE");
                break;
            case KeyEvent.VK_P:
                if (log.getPaused()) {
                    log.logPlay();
                }
                else {
                    log.logPause();
                }
                break;
            case KeyEvent.VK_LEFT:
                log.logLastFrame();
                break;
            case KeyEvent.VK_RIGHT:
                log.logNextFrame();
                break;
            }
        }
        else if (mode == VIEW_EKF_LOG) {
            switch (key) {
            case KeyEvent.VK_R:
                startEKFLog();
                System.out.println("RELOADED LOG FILE");
                break;
            case KeyEvent.VK_P:
                if (ekfLog.getPaused()) {
                    ekfLog.logPlay();
                } else {
                    ekfLog.logPause();
                } break;
            case KeyEvent.VK_LEFT:
                ekfLog.logLastFrame();
                break;
            case KeyEvent.VK_RIGHT:
                ekfLog.logNextFrame();
                break;
            }
        }
    }

    /** Listen to the slider. */
    public void stateChanged(ChangeEvent e)
    {
        JSlider source = (JSlider)e.getSource();
        if (!source.getValueIsAdjusting() && source == fps_slide) {
            int value = (int)source.getValue();
            System.out.println("fps set to: " + value);
            setPlaybackFps(value);
        }
        if (!source.getValueIsAdjusting() &&
            source == num_display_estimates_slide) {
            int value = (int)source.getValue();
            System.out.println("num estimates set to: " + value);
            painter.setPositionsToDraw(value);
        }
    }

    //keyboard methods
    public void keyTyped(KeyEvent e) {}
    public void keyReleased(KeyEvent e)
    {
        aibo.sendCommand("motion 0 0 0");
        key_pressed = false;
        aibo.sendCommand("assertPythonControl");
    }

    public void setPlaybackFps(int _fps)
    {
        playback_fps = _fps;
        // maybe the redraw timer is currently running, so we have to
        // manually inform it that the fps has changed
        log.setPlaybackFps(_fps);
    }
    public int getPlaybackFps() { return playback_fps; }
    public void setMode(int _mode) { mode = _mode; }
    public int getMode() { return mode; }
}




