package TOOL.WorldController;
import TOOL.Data.Field;
import TOOL.Image.ImagePanel;

import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.util.LinkedList;
import java.util.HashMap;
import java.util.Vector;
import java.lang.Exception;
import java.util.ConcurrentModificationException;

/**
 * This class listens to the robots and paints the information on top of a
 * buffered image representation of the field.
 *
 * @author Nicholas Dunn (Modified, did not create)
 * @date  3/18/08
 */

public class WorldControllerPainter implements RobotListener
{
    // Holds a field instance, from which we obtain constants necessary
    // to render the world controller information
    protected Field field;
    // The panel holding the buffered image, on whose graphical context
    // we willd raw
    protected ImagePanel viewer;

    // DEBUG SWITCHES
    static final boolean DEBUG_SEEN_LANDMARKS = false;

    // The maximum number of robots allowable on the field, and therefore
    // the maximum that we will acount for in the data strucutures
    static final int MAX_ROBOTS_ON_FIELD = 10;

    // Debug drawing stuff
    static final Color SAW_BLUE_BEACON_COLOR = Color.CYAN;
    static final Color SAW_YELLOW_BEACON_COLOR = Color.YELLOW;

    // Robot drawing
    static final Color REAL_ROBOT_POSITION_COLOR = Color.CYAN;
    static final Color ESTIMATED_ROBOT_POSITION_COLOR = Color.BLUE;
    static final int   BLUE_TEAM = 0;
    static final int   RED_TEAM = 1;
    static final int   DEFAULT_TEAM = BLUE_TEAM;
    static final Color ROBOT_COLOR_RED_TEAM = Color.RED;
    static final Color ROBOT_COLOR_BLUE_TEAM = Color.BLUE;
    static final Color ESTIMATED_ROBOT_XY_UNCERT_COLOR = Color.YELLOW;
    static final Color ESTIMATED_ROBOT_HEADING_UNCERT_COLOR = Color.PINK;
    static final Color VISIBLE_LANDMARK_COLOR = Color.PINK;


    // Drawing paramaters
    // Adjust to change how the robots position, uncertainty, etc are represented
    // on the drawing
    public static final double DRAW_NUM_SD_RADI = 2.;
    public static final double POSITION_DOT_RADIUS = 2.;
    public static final double LANDMARK_SEEN_RADIUS = 4.;
    public static final double POSITION_HEADING_RADIUS = 25.;
    public static final double PAN_HEADING_RADIUS = 10.;
    public static final double HEADING_UNCERT_RADIUS = 20.;

    // Ball drawing
    static final Color REAL_BALL_COLOR = Color.pink;
    static final Color ESTIMATED_BALL_COLOR = Color.ORANGE;
    static final Color ESTIMATED_BALL_UNCERT_COLOR = Color.ORANGE;
    static final boolean DRAW_BALL_VELOCITY = true;

    // Particle drawing
    static final int PARTICLE_RADIUS = 3;
    static final int PARTICLE_HEADING_DIST = 10;

    // Known pose drawing
    static final float NO_DATA_VALUE = -111.111f;

    // This instance draws a line from the current estimate of the ball's
    // location to the the estimte of its location in
    // BALL_VELOCITY_DRAWING_MULTIPLIER_SECONDS seconds,
    // as implied by its estimated velocity.

    // Unit: inverse seconds
    static final double  BALL_VELOCITY_DRAWING_MULTIPLIER = 1.0;
    static final int     BALL_VELOCITY_STROKE = 1;

    // static final double RAD_TO_DEG = 360. / (2. * Math.PI);
    // static final double DEG_TO_RAD = (2. * Math.PI) / 360.;

    // Used when we are only tracking one robot.  Chosen to be a weird number
    // so that it doesen't collide with a robot's udp key
    static final Integer SINGLE_ROBOT_KEY = new Integer(-1273);

    private int num_positions_to_draw;

    // The number of most recent positions/uncertainties to appear on the
    // field at one time
    private int NUMBER_PREVIOUS_POSITIONS_TO_DRAW = 1;

    // To hold the history for one or more robots
    private HashMap<Integer, RobotHistory> robot_histories =
        new HashMap<Integer, RobotHistory>(MAX_ROBOTS_ON_FIELD);
    // Add a RobotHistory instance to handle the case that we are visualizing
    // a single robot
    RobotHistory single_robot;

    // because UDP currently doesn't provide heading uncertainty estimates
    static final double ASSUMED_HEADING_UNCERT_FOR_UDP_COMMUNICATIONS = 0.0;
    static final double ASSUMED_HEAD_PAN_FOR_UDP_COMMUNICATIONS = 0.0;

    // A tautological geometric constant
    private boolean draw_real, draw_est;

    private double by_x,by_y;
    private double yb_x,yb_y;

    private float[][] seenLandmarks = new float[100][3];
    private int numSeenLandmarks;
    private static final int X_INDEX = 0;
    private static final int Y_INDEX = 1;
    private static final int DISTINCT_INDEX = 2;
    private static final int LANDMARK_LINE_WIDTH = 1;

    // Information to be set for drawing particles
    // Particle set for drawing...
    private Vector< MCLParticle > currentParticles;
    // Hold information about the player
    private int mclTeamColor;
    private int mclPlayerNum;

    private double[] positionEstimates;
    private double[] uncertaintyEstimates;
    private float[] estimatedPose;
    private float[] realPose;
    private float[] realBallPose;

    /**
     * Constructs the painter to draw all possible localization information
     * @param toView Field on which the painter will paint.
     * @param panel
     */
    public WorldControllerPainter(Field toView, ImagePanel panel)
    {
        field = toView;
        viewer = panel;
        draw_real = true;
        draw_est = true;
        num_positions_to_draw = NUMBER_PREVIOUS_POSITIONS_TO_DRAW;

        by_x = -1;
        by_y = -1;
        yb_x = -1;
        yb_y = -1;

        single_robot = new RobotHistory();
        single_robot.team = 1;
        single_robot.color = BLUE_TEAM;
        robot_histories.put(SINGLE_ROBOT_KEY, single_robot);
        numSeenLandmarks = 0;
        currentParticles = new Vector<MCLParticle>();
        mclTeamColor = 0;
        mclPlayerNum = 1;

        positionEstimates = new double[3];
        uncertaintyEstimates = new double[3];
        realPose = new float[3];
        realPose[0] = NO_DATA_VALUE;
        realPose[1] = NO_DATA_VALUE;
        realPose[2] = NO_DATA_VALUE;
        estimatedPose = new float[3];
        estimatedPose[0] = NO_DATA_VALUE;
        estimatedPose[1] = NO_DATA_VALUE;
        estimatedPose[2] = NO_DATA_VALUE;
        realBallPose = new float[4];
        realBallPose[0] = NO_DATA_VALUE;
        realBallPose[1] = NO_DATA_VALUE;
        realBallPose[2] = NO_DATA_VALUE;
        realBallPose[3] = NO_DATA_VALUE;

    }

    /**
     * The class containing the BufferedImage which passes the graphics context
     * into here should have ALREADY scaled the context to match that of the
     * window.  Otherwise the landmarks etc. will not line up with the field
     * image.
     */
    public void paintInfoOnField(Graphics2D g2)
    {
        if (g2 == null) { return; }
        try {
            paintLandmarks(g2);
            paintRobotInformation(g2);
            paintParticleSet(g2);
            paintEstRobotPose(g2);
            paintEstimateMeanAndVariance(g2);
            paintRealRobotPose(g2);
            paintRealBallPose(g2);
        } catch (ConcurrentModificationException e) {
            // Ignore.  The painting and simulation threads are trying
            // to concurrently access elements in the same list
        } catch (Exception f) {
            // Any other exceptions signal trouble
            f.printStackTrace();
        }
    }

    /**
     * Steps through the array of seen landmarks and paints them to the screen.
     * Additionally, if debugging mode for landmarks is on, paints filled ovals
     * where the BY and/or YB beacons were found.
     */
    protected void paintLandmarks(Graphics2D g2)
    {
        for (int i = 0; i < numSeenLandmarks; i++) {
            float x = seenLandmarks[i][X_INDEX];
            float y = seenLandmarks[i][Y_INDEX];

            // Determine if it is ambiguous or not
            switch((int)seenLandmarks[i][DISTINCT_INDEX]) {
            case 0:
                // Pass off the work to another helper method
                drawSeenLandmark(g2, x, y);
                break;
            case 1:
                drawAmbiguousLandmark(g2, x, y, Color.red);
                break;
            case 2:
                drawAmbiguousLandmark(g2, x, y, Color.orange);
                break;
            case 3:
                drawAmbiguousLandmark(g2, x, y, Color.cyan);
                break;
            case 4:
                drawAmbiguousLandmark(g2, x, y, Color.pink);
                break;
            case 5:
                drawAmbiguousLandmark(g2, x, y, Color.magenta);
                break;
            case 6:
                drawAmbiguousLandmark(g2, x, y, Color.lightGray);
                break;
            default:
                drawAmbiguousLandmark(g2, x, y, Color.white);
                break;
            }
        }
        // Reset for the next frame
        numSeenLandmarks = 0;
    }



    public void setField(Field f)
    {
        field = f;
    }


    protected void paintRobotInformation(Graphics2D g2)
    {
        if (draw_real) {
            paintRobotRealInformation(g2);
        }
        if (draw_est) {
            paintRobotEstInformation(g2);
        }
    }

    /**
     * For each of the robots we are tracking, draws its position on the
     * field as well as its idea of where the ball is and has been recently.
     *
     * NOTE:  Unlike in the paint landmarks method where we need to switch the
     * coordinate system to account for the red team, by the time we enter this
     * method all the coordinate systems have been fixed for the robot
     * information.
     * @see updateRobot(Robot) update
     */
    protected void paintRobotRealInformation(Graphics2D g2)
    {
        // For each of the robots that we are tracking
        for (RobotHistory robot_history : robot_histories.values()) {
            // Draw the robot's most recent actual locations
            for (LocalizationPacket recent_position :
                     robot_history.actual_position_history) {
                drawRobotsRealPosition(g2,
                                     recent_position.getXActual(),
                                     recent_position.getYActual(),
                                     recent_position.
                                     getHeadingActual(),
                                     recent_position.
                                     getHeadPanActual());
            }
            // Draw the ball's most recent actual locations
            for (LocalizationPacket recent_position :
                     robot_history.actual_ball_position_history) {
                drawBallRealPosition(g2,
                                     recent_position.getXActual(),
                                     recent_position.getYActual());
            }
        }
    }

    /**
     * Draws the robots' estimated positions and their idea of where the ball
     * is and has been recently.  Again, please note that prior to entering this
     * method the red team's coordinate system has been suitably modified to
     * work with this method.
     * @see updateRobot(Robot) update
     */
    protected void paintRobotEstInformation(Graphics2D g2)
    {
        // For each of the robots that we are tracking
        for (RobotHistory robot_history : robot_histories.values()) {
            // Draw the most recent robot estimated locations and uncertainties
            Color robotColor, numberColor;
            if (robot_history.color == RED_TEAM) {
                robotColor = ROBOT_COLOR_RED_TEAM;
                numberColor = ROBOT_COLOR_RED_TEAM;
            } else {
                robotColor = ROBOT_COLOR_BLUE_TEAM;
                numberColor = ROBOT_COLOR_BLUE_TEAM;
            }
            for (LocalizationPacket recent_estimate :
                     robot_history.estimated_position_history) {
                // draw the location and heading estimate dot and line
                drawRobotsEstimatedPosition(g2,
                                          robotColor,
                                          recent_estimate.getXEst(),
                                          recent_estimate.getYEst(),
                                          recent_estimate.
                                          getHeadingEst(),
                                          recent_estimate.
                                          getHeadPanEst(),
                                          robot_history.number);
                // draw the position uncertanity oval, if applicable
                drawRobotsUncertainty(g2,
                                    recent_estimate.getXEst(),
                                    recent_estimate.getYEst(),
                                    recent_estimate.getHeadingEst(),
                                    recent_estimate.getXUncert(),
                                    recent_estimate.getYUncert(),
                                    recent_estimate.getHUncert());
                robotColor = robotColor.darker();
            }
            // Draw the most recent ball estimated locations and uncertainties
            Color ballColor = Color.WHITE;
            for (LocalizationPacket recent_estimate :
                     robot_history.estimated_ball_position_history) {
                drawBallEstimatedPosition(g2,
                                          recent_estimate.getXEst(),
                                          recent_estimate.getYEst(),
                                          recent_estimate.
                                          getXVelocity(),
                                          recent_estimate.
                                          getYVelocity(),
                                          robot_history.number,
                                          numberColor);
                drawBallUncertainty(g2,
                                    recent_estimate.getXEst(),
                                    recent_estimate.getYEst(),
                                    recent_estimate.getXUncert(),
                                    recent_estimate.getYUncert());
            }
            ballColor = ballColor.darker();
        }
    }


    // draw robot's real positioni
    public void drawRobotsRealPosition(Graphics2D drawing_on,
                                     double at_x, double at_y,
                                     double at_heading, double at_pan)
    {
        field.fillOval(drawing_on, REAL_ROBOT_POSITION_COLOR,
                       field.DRAW_STROKE, at_x, at_y, POSITION_DOT_RADIUS,
                       POSITION_DOT_RADIUS);
        double x_line_body_disp = (POSITION_HEADING_RADIUS *
                                   Math.cos(at_heading));
        double y_line_body_disp = (POSITION_HEADING_RADIUS *
                                   Math.sin(at_heading));
        field.drawLine(drawing_on, REAL_ROBOT_POSITION_COLOR, field.DRAW_STROKE,
                       at_x - x_line_body_disp, at_y - y_line_body_disp,
                       at_x, at_y);
        double x_line_pan_disp = (PAN_HEADING_RADIUS *
                                  Math.cos((at_pan +
                                            at_heading
                                            )));
        double y_line_pan_disp = (PAN_HEADING_RADIUS *
                                  Math.sin((at_pan +
                                            at_heading
                                            )));
        field.drawLine(drawing_on, REAL_ROBOT_POSITION_COLOR, field.DRAW_STROKE,
                       at_x, at_y, at_x + x_line_pan_disp,
                       at_y + y_line_pan_disp);
    }

    // draw robot's estimated position
    public void drawRobotsEstimatedPosition(Graphics2D drawing_on,
                                          Color robotColor, double at_x,
                                          double at_y, double at_heading,
                                          double at_pan, int player_number)
    {
        field.fillOval(drawing_on, robotColor, field.DRAW_STROKE, at_x, at_y,
                       POSITION_DOT_RADIUS, POSITION_DOT_RADIUS);
        // TAIL PART
        double x_line_body_disp = (POSITION_HEADING_RADIUS *
                                   Math.cos(at_heading));
        double y_line_body_disp = (POSITION_HEADING_RADIUS *
                                   Math.sin(at_heading));
        field.drawLine(drawing_on, robotColor, field.DRAW_STROKE,
                       at_x - x_line_body_disp, at_y - y_line_body_disp,
                       at_x, at_y);
        double x_line_pan_disp = (PAN_HEADING_RADIUS *
                                  Math.cos((at_pan +
                                            at_heading
                                            )));
        double y_line_pan_disp = (PAN_HEADING_RADIUS *
                                  Math.sin((at_pan +
                                            at_heading
                                            )));
        field.drawLine(drawing_on, robotColor, field.DRAW_STROKE, at_x, at_y,
                       at_x + x_line_pan_disp, at_y + y_line_pan_disp);

        // Here we add a number ontop of the robot
        field.drawNumber(drawing_on, robotColor, at_x+5, at_y+5,
                         player_number);
    }

    // draw robot uncertainty (x,y,h)
    public void drawRobotsUncertainty(Graphics2D drawing_on,
                                    double at_x,
                                    double at_y,
                                    double at_heading,
                                    double x_uncert,
                                    double y_uncert,
                                    double h_uncert)
    {
        // draw robot's uncert
        field.drawOval(drawing_on,
                       ESTIMATED_ROBOT_XY_UNCERT_COLOR,
                       field.DRAW_STROKE,
                       at_x,
                       at_y,
                       x_uncert,
                       y_uncert);

        //double half_h_uncert = h_uncert/2.0;

        // HEAD PART
        double x_line_pan_disp_plus = HEADING_UNCERT_RADIUS *
            Math.cos(h_uncert + at_heading);
        double y_line_pan_disp_plus = HEADING_UNCERT_RADIUS *
            Math.sin(h_uncert + at_heading);
        double x_line_pan_disp_minus = HEADING_UNCERT_RADIUS *
            Math.cos(-h_uncert + at_heading);
        double y_line_pan_disp_minus = HEADING_UNCERT_RADIUS *
            Math.sin(-h_uncert + at_heading);

        field.drawLine(drawing_on,
                       ESTIMATED_ROBOT_HEADING_UNCERT_COLOR,
                       field.DRAW_STROKE,
                       at_x,
                       at_y,
                       at_x + x_line_pan_disp_plus,
                       at_y + y_line_pan_disp_plus);

        field.drawLine(drawing_on,
                       ESTIMATED_ROBOT_HEADING_UNCERT_COLOR,
                       field.DRAW_STROKE,
                       at_x,
                       at_y,
                       at_x + x_line_pan_disp_minus,
                       at_y + y_line_pan_disp_minus);

        // Draw dashed lines as axis of the oval
        field.drawLine(drawing_on,
                       ESTIMATED_ROBOT_XY_UNCERT_COLOR,
                       new BasicStroke(1.0F, BasicStroke.CAP_BUTT,
                                       BasicStroke.JOIN_MITER, 10,
                                       new float[] {2}, 0),
                       at_x,
                       at_y - y_uncert,
                       at_x,
                       at_y + y_uncert);
        field.drawLine(drawing_on,
                       ESTIMATED_ROBOT_XY_UNCERT_COLOR,
                       new BasicStroke(1.0F, BasicStroke.CAP_BUTT,
                                       BasicStroke.JOIN_MITER, 10,
                                       new float[] {2}, 0),
                       at_x - x_uncert,
                       at_y,
                       at_x + x_uncert,
                       at_y);

    }

    // draw the ball's position from robot ekf
    public void drawBallEstimatedPosition(Graphics2D drawing_on,
                                          double at_x,
                                          double at_y,
                                          double x_velocity,
                                          double y_velocity,
                                          int player_number,
                                          Color robotColor)
    {
        field.fillOval(drawing_on, ESTIMATED_BALL_COLOR, field.DRAW_STROKE,
                       at_x, at_y, field.BALL_RADIUS, field.BALL_RADIUS);
        field.drawLine(drawing_on,
                       ESTIMATED_BALL_COLOR, BALL_VELOCITY_STROKE,
                       at_x, at_y,
                       at_x + x_velocity * BALL_VELOCITY_DRAWING_MULTIPLIER,
                       at_y + y_velocity * BALL_VELOCITY_DRAWING_MULTIPLIER);
        field.drawNumber(drawing_on, robotColor, at_x, at_y, player_number);
    }

    // draws ball's 'actual' position in simulation/camera
    public void drawBallRealPosition(Graphics2D drawing_on,
                                     double at_x,
                                     double at_y)
    {
        field.fillOval(drawing_on, REAL_BALL_COLOR, field.DRAW_STROKE,
                       at_x, at_y, field.BALL_RADIUS, field.BALL_RADIUS);
    }

    // draws ball uncertainty (both x and y uncertainty)
    public void drawBallUncertainty(Graphics2D drawing_on,
                                    double at_x,
                                    double at_y,
                                    double x_uncert,
                                    double y_uncert)
    {
        field.drawOval(drawing_on, ESTIMATED_BALL_UNCERT_COLOR,
                       field.DRAW_STROKE,
                       at_x, at_y, x_uncert, y_uncert);
    }

    public void drawSeenLandmark(Graphics2D drawing_on, float x, float y)
    {
        field.drawOval(drawing_on, Color.black, 1, x, y, 10, 10);
    }

    public void drawAmbiguousLandmark(Graphics2D drawing_on, float x, float y)
    {
        drawAmbiguousLandmark(drawing_on, x, y, Color.red);
    }
    public void drawAmbiguousLandmark(Graphics2D drawing_on, float x, float y,
                                      Color col)
    {
        field.drawOval(drawing_on, col, 1, x, y, 10, 10);
    }

    public void reportUpdatedActualLocation(LocalizationPacket
                                            actual_location)
    {
        reportUpdatedActualLocation(singleRobotHistory(), actual_location);
    }

    public void reportUpdatedActualLocation(RobotHistory robot_history,
                                            LocalizationPacket
                                            actual_location)
    {
        if (robot_history.actual_position_history.size() >=
            num_positions_to_draw) {
            robot_history.actual_position_history.removeLast();
        }
        robot_history.actual_position_history.addFirst(actual_location);
    }

    public void reportUpdatedActualBallLocation(LocalizationPacket
                                                actual_ball_location)
    {
        reportUpdatedActualBallLocation(singleRobotHistory(),
                                        actual_ball_location);
    }

    public void
        reportUpdatedActualBallLocation(RobotHistory robot_history,
                                        LocalizationPacket
                                        actual_ball_location)
    {
        if (robot_history.actual_ball_position_history.size() >=
            num_positions_to_draw) {
            robot_history.actual_ball_position_history.removeLast();
        }
        robot_history.actual_ball_position_history.addFirst(actual_ball_location);
    }

    public void
        reportUpdatedLocalization(LocalizationPacket new_localization_info,
                                  int team_color, int player_number)
    {
        if (team_color == RED_TEAM) {
            transformFieldCoordinatesToRedTeamSide(new_localization_info);
        }
        reportUpdatedLocalization(singleRobotHistory(),
                                  new_localization_info,
                                  team_color,
                                  player_number);
    }

    public void
        reportUpdatedLocalization(RobotHistory robot_history,
                                  LocalizationPacket new_localization_info,
                                  int team_color, int player_number)
    {
        if (robot_history.estimated_position_history.size() >=
            num_positions_to_draw) {
            robot_history.estimated_position_history.removeLast();
        }
        robot_history.estimated_position_history.addFirst(new_localization_info);
        robot_history.color = team_color;
        robot_history.number = player_number;
    }

    public void
        reportUpdatedBallLocalization(LocalizationPacket
                                      new_ball_localization_info,
                                      int team_color, int player_number)
    {
        if (team_color == RED_TEAM) {
            transformFieldCoordinatesToRedTeamSide(new_ball_localization_info);
        }
        reportUpdatedBallLocalization(singleRobotHistory(),
                                      new_ball_localization_info,
                                      team_color,
                                      player_number);
    }

    public void reportUpdatedBallLocalization(RobotHistory robot_history,
                                              LocalizationPacket
                                              new_ball_localization_info,
                                              int team_color,
                                              int player_number)
    {
        if (robot_history.estimated_ball_position_history.size() >=
            num_positions_to_draw) {
            robot_history.estimated_ball_position_history.removeLast();
        }
        robot_history.estimated_ball_position_history.
            addFirst(new_ball_localization_info);
        robot_history.color = team_color;
        robot_history.number = player_number;
    }

    public void reportTrackedLandmark(LocalizationPacket landmark_location)
    {
        reportTrackedLandmark(singleRobotHistory(), landmark_location);
    }

    public void reportTrackedLandmark(RobotHistory robot_history, LocalizationPacket
                                      landmark_location)
    {
        robot_history.visible_landmarks.add(landmark_location);
    }

    // 0 true, 1 false
    public void sawLandmark(float x, float y, int distinct)
    {
		// This prevents us from gathering too much of a
		// backlog of unpainted landmarks
		if (numSeenLandmarks > seenLandmarks.length){
			return;
		}
        seenLandmarks[numSeenLandmarks][X_INDEX] = x;
        seenLandmarks[numSeenLandmarks][Y_INDEX] = y;
        seenLandmarks[numSeenLandmarks][DISTINCT_INDEX] = distinct;
        numSeenLandmarks++;
    }

    public void clearSeenLandmarks()
    {
        numSeenLandmarks = 0;
    }

    public void sawBluePost(Double dist, Double angle)
    {
        if (dist == 0) {
            by_x = -1; by_y = -1;
            return;
        }

        LocalizationPacket est = singleRobotHistory().estimated_position_history.
            getFirst();
        double new_angle = AngleUtilities.sub180Angle(-angle+est.
                                                      getHeadingEst());
        double adj = Math.cos(new_angle)*dist;
        double opp = Math.sin(new_angle)*dist;

        by_x = opp+est.getXEst();
        by_y = adj+est.getYEst();
    }

    public void sawYellowPost(Double dist, Double angle)
    {
        if (dist == 0) {
            yb_x = -1; yb_y = -1;
            return;
        }

        LocalizationPacket est = singleRobotHistory().estimated_position_history.
            getFirst();
        double new_angle = AngleUtilities.sub180Angle(-angle+est.
                                                      getHeadingEst());
        double adj = Math.cos(new_angle)*dist;
        double opp = Math.sin(new_angle)*dist;

        yb_x = opp+est.getXEst();
        yb_y = adj+est.getYEst();
    }

    public void reportEndFrame()
    {
        viewer.repaint();
        clearVisibleLandmarks();
    }

    public void updateRobot(Robot robot)
    {
        // Use the robot's unique number to see if we allready have it in our
        // hash of all robots, adding it to the hash if we don't
        // Update the robot's history with the uncoming UDP data
        RobotHistory this_robots_history;
        if (robot_histories.containsKey(robot.getHash())) {
            this_robots_history = robot_histories.get(robot.getHash());
        }
        else {
            this_robots_history = new RobotHistory();
            this_robots_history.team = robot.getTeam();
            this_robots_history.color = robot.getColor();
            this_robots_history.number = robot.getNumber().intValue();
            robot_histories.put(robot.getHash(), this_robots_history);
        }
        RobotData d = robot.getData();
        LocalizationPacket robot_estimate = LocalizationPacket.
            makeEstimateAndUncertPacket(d.getRobotX(),
                                        d.getRobotY(),
                                        d.getRobotHeadingRadians(),
                                        ASSUMED_HEAD_PAN_FOR_UDP_COMMUNICATIONS,
                                        d.getRobotUncertX(),
                                        d.getRobotUncertY(),
                                        d.getRobotUncertH());
        LocalizationPacket ball_estimate = LocalizationPacket.
            makeBallEstimateAndUncertPacket(d.getBallX().doubleValue(),
                                            d.getBallY().doubleValue(),
                                            d.getBallXUncert().doubleValue(),
                                            d.getBallYUncert().doubleValue(),
                                            d.getBallXVel(),
                                            d.getBallYVel());
        // If the robot is on the red team, then we need to transform its
        // co-ordinates because it has a different (0,0) that the robots on the
        // blue team
        if (robot.getColor() == RED_TEAM) {
            transformFieldCoordinatesToRedTeamSide(robot_estimate);
            transformFieldCoordinatesToRedTeamSide(ball_estimate);
        }
        reportUpdatedLocalization(this_robots_history,
                                  robot_estimate,
                                  robot.getColor(),
                                  robot.getNumber().intValue());
        reportUpdatedBallLocalization(this_robots_history,
                                      ball_estimate,
                                      robot.getColor(),
                                      robot.getNumber().intValue());
        reportEndFrame();
    }

    public void transformFieldCoordinatesToRedTeamSide(LocalizationPacket
                                                       loc_pack)
    {
        // Currently transforms x_ and y_est, heading_est, x_velocity,
        // and y_velocity
        // loc_pack.x_est = field.FIELD_GREEN_WIDTH - loc_pack.x_est;
        // loc_pack.y_est = field.FIELD_GREEN_HEIGHT - loc_pack.y_est;
        // loc_pack.heading_est = AngleUtilities.sub180Angle(loc_pack.
        //                                                   heading_est + 180);
        // loc_pack.x_velocity = loc_pack.x_velocity * -1.0;
        // loc_pack.y_velocity = loc_pack.y_velocity * -1.0;
    }

    public RobotHistory singleRobotHistory()
    {
        return robot_histories.get(SINGLE_ROBOT_KEY);
    }

    public void clearEstimatedPositionHistory()
    {
        for (RobotHistory robot_history : robot_histories.values()) {
            robot_history.estimated_position_history.clear();
        }
    }
    public void clearEstimatedBallPositionHistory()
    {
        for (RobotHistory robot_history : robot_histories.values()) {
            robot_history.estimated_ball_position_history.clear();
        }
    }
    public void clearActualPositionHistory()
    {
        for (RobotHistory robot_history : robot_histories.values()) {
            robot_history.actual_position_history.clear();
        }
    }
    public void clearActualBallPositionHistory()
    {
        for (RobotHistory robot_history : robot_histories.values()) {
            robot_history.actual_ball_position_history.clear();
        }
    }

    public void clearVisibleLandmarks()
    {
        for (RobotHistory robot_history : robot_histories.values()) {
            robot_history.visible_landmarks.clear();
        }
    }

    public void clearSimulationHistory()
    {
        clearEstimatedPositionHistory();
        clearEstimatedBallPositionHistory();
        clearActualPositionHistory();
        clearActualBallPositionHistory();
        clearVisibleLandmarks();
        viewer.repaint();
    }

    // Setters for drawing stuff
    public void setDrawReal(boolean _draw_real) { draw_real = _draw_real; }
    public void setDrawEst(boolean _draw_est) { draw_est = _draw_est; }
    public void setPositionsToDraw(int d) { num_positions_to_draw = d; }

    // Getters
    public boolean getDrawReal() { return draw_real; }
    public boolean getDrawEst() { return draw_est; }
    public int getPositionsToDraw() { return num_positions_to_draw; }
    public Dimension getPreferredSize() { return field.getPreferredSize(); }

    // MCL Stuff
    /**
     * Updates the current set of data points to be drawn
     *
     * @param particles The set of particles to be drawn
     * @param tColor The team color. Either RED_TEAM or BLUE_TEAM
     * @param pNUm The player number of the robot being drawn
     */
    public void updateParticleSet(Vector<MCLParticle> particles, int tColor,
                                  int pNum)
    {
        currentParticles = particles;
        mclTeamColor = tColor;
        mclPlayerNum = pNum;
    }

    /**
     * Updates the current position estimates and uncertainty measures
     *
     * @param _x
     * @param _y
     * @param _h
     * @param _uncertX
     * @param _uncertY
     * @param _uncertH
     */
    public void updateUncertainytInfo(double _x, double _y, double _h,
                                      double _uncertX, double _uncertY,
                                      double _uncertH)
    {
        positionEstimates[0] = _x;
        positionEstimates[1] = _y;
        positionEstimates[2] = _h;
        uncertaintyEstimates[0] = _uncertX;
        uncertaintyEstimates[1] = _uncertY;
        uncertaintyEstimates[2] = _uncertH;

    }

    /**
     * Update the current known pose of the robot
     *
     * @param _x The x position of the robot
     * @param _y The y position of the robot
     * @param _h The h position of the robot
     */
    public void updateRealPoseInfo(float _x, float _y, float _h)
    {
        realPose[0] = _x;
        realPose[1] = _y;
        realPose[2] = _h;
    }

    /**
     * Update the current known pose of the ball
     *
     * @param _x The x position of the ball
     * @param _y The y position of the ball
     * @param _vx The x velocity of the ball
     * @param _vy The y velocity of the ball
     */
    public void updateRealBallInfo(float _x, float _y,
                                   float _vx, float _vy)
    {
        realBallPose[0] = _x;
        realBallPose[1] = _y;
        realBallPose[2] = _vx;
        realBallPose[3] = _vy;
    }

    /**
     * Update the current known pose of the robot
     *
     * @param _x The x position of the robot
     * @param _y The y position of the robot
     * @param _h The h position of the robot
     */
    public void updateEstPoseInfo(float _x, float _y, float _h)
    {
        estimatedPose[0] = _x;
        estimatedPose[1] = _y;
        estimatedPose[2] = _h;
    }

    /**
     * Paints the current set of particles on the field.  Called by
     * updateInfomration when the WorldControllerViewer is updated.
     *
     * @param g2 The graphics context to be drawn on.
     */
    public void paintParticleSet(Graphics2D g2) {
        for(MCLParticle p : currentParticles) {
            Color partColor;
            if( mclTeamColor == RED_TEAM) {
                partColor = ROBOT_COLOR_RED_TEAM;
            } else {
                partColor = ROBOT_COLOR_BLUE_TEAM;
            }
            drawParticle(g2, partColor, p.getX(),
                         p.getY(), p.getH(),
                         p.getWeight());
        }
    }

    /**
     * Paints the uncertainty ellipses and estimate center for the
     * reported information
     *
     * @param g2 The graphics context to be drawn on.
     */
    public void paintEstimateMeanAndVariance(Graphics2D g2)
    {
        drawRobotsUncertainty(g2, positionEstimates[0], positionEstimates[1],
                              positionEstimates[2], uncertaintyEstimates[0],
                              uncertaintyEstimates[1], uncertaintyEstimates[2]);
    }

    /**
     * Function to draw the know position of the robot; mostly used for
     * debugging of artificially created data. Draws nothing if the current pose
     * is not known.
     *
     * @param g2 The graphics context to be drawn on.
     */
    public void paintEstRobotPose(Graphics2D g2)
    {
        if (estimatedPose[0] == NO_DATA_VALUE) {// Test if data exists
            return; // Draw nothing if we don't know the current real pose
        }
        Color partColor;
        if( mclTeamColor == RED_TEAM) {
            partColor = ROBOT_COLOR_RED_TEAM;
        } else {
            partColor = ROBOT_COLOR_BLUE_TEAM;
        }

        drawRobotsEstimatedPosition(g2, partColor, estimatedPose[0],
                                    estimatedPose[1], estimatedPose[2], 0.0,
                                    mclPlayerNum);
    }


    /**
     * Function to draw the know position of the robot; mostly used for
     * debugging of artificially created data. Draws nothing if the current pose
     * is not known.
     *
     * @param g2 The graphics context to be drawn on.
     */
    public void paintRealRobotPose(Graphics2D g2)
    {
        if (realPose[0] == NO_DATA_VALUE) {// Test if data exists
            return; // Draw nothing if we don't know the current real pose
        }
        drawRobotsRealPosition(g2, /*Color.CYAN,*/ realPose[0], realPose[1],
                             realPose[2], 0.0);
    }

    /**
     * Function to draw the known position of the ball
     *
     * @param g2 The graphics context on which the ball should be drawn
     */
    public void paintRealBallPose(Graphics2D g2)
    {
        if (realBallPose[0] == NO_DATA_VALUE) {// Test if data exists
            return; // Draw nothing if we don't know the current real pose
        }
        drawBallRealPosition(g2, realBallPose[0],
                             realBallPose[1]);
    }


    /**
     * Draws a particle on the field
     *
     * @param drawing_on the graphics context on which to draw the particle
     * @param in_color the color of the particle to be drawn
     * @param x the x estimate of the particle
     * @param y the y estimate of the particle
     * @param h the heading estimate of the particle
     */
    public void drawParticle(Graphics2D drawing_on, Color in_color,
                             float x, float y, float h, float weight)
    {
        // Draw a circle centered at the robots position
        field.fillOval(drawing_on, in_color, field.DRAW_STROKE, x, y,
                       (double)PARTICLE_RADIUS, (double)PARTICLE_RADIUS);

        // Draw a line pointing in the direction of the heading
        field.drawLine(drawing_on, in_color, field.DRAW_STROKE, x, y,
                       x + PARTICLE_HEADING_DIST*Math.cos(h),
                       y + PARTICLE_HEADING_DIST*Math.sin(h));
    }
}