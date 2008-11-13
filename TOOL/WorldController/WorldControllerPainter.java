package edu.bowdoin.robocup.TOOL.WorldController;
import edu.bowdoin.robocup.TOOL.Data.Field;
import edu.bowdoin.robocup.TOOL.Image.ImagePanel;

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

public class WorldControllerPainter implements DogListener
{
    // Holds a field instance, from which we obtain constants necessary
    // to render the world controller information
    protected Field field;
    // The panel holding the buffered image, on whose graphical context
    // we willd raw
    protected ImagePanel viewer;

    // DEBUG SWITCHES
    static final boolean DEBUG_SEEN_LANDMARKS = false;

    // The maximum number of dogs allowable on the field, and therefore
    // the maximum that we will acount for in the data strucutures
    static final int MAX_DOGS_ON_FIELD = 10;

    // Debug drawing stuff
    static final Color SAW_BLUE_BEACON_COLOR = Color.CYAN;
    static final Color SAW_YELLOW_BEACON_COLOR = Color.YELLOW;

    // Dog drawing
    static final Color REAL_DOG_POSITION_COLOR = Color.RED;
    static final Color ESTIMATED_DOG_POSITION_COLOR = Color.BLUE;
    static final int   BLUE_TEAM = 0;
    static final int   RED_TEAM = 1;
    static final int   DEFAULT_TEAM = BLUE_TEAM;
    static final Color DOG_COLOR_RED_TEAM = Color.RED;
    static final Color DOG_COLOR_BLUE_TEAM = Color.BLUE;
    static final Color ESTIMATED_DOG_XY_UNCERT_COLOR = Color.YELLOW;
    static final Color ESTIMATED_DOG_HEADING_UNCERT_COLOR = Color.PINK;
    static final Color VISIBLE_LANDMARK_COLOR = Color.PINK;


    // Drawing paramaters
    // Adjust to change how the dogs position, uncertainty, etc are represented
    // on the drawing
    public static final double DRAW_NUM_SD_RADI = 2.;
    public static final double POSITION_DOT_RADIUS = 2.;
    public static final double LANDMARK_SEEN_RADIUS = 4.;
    public static final double POSITION_HEADING_RADIUS = 20.;
    public static final double PAN_HEADING_RADIUS = 15.;
    public static final double HEADING_UNCERT_RADIUS = 20.;

    // Ball drawing
    static final Color REAL_BALL_COLOR = Color.RED;
    static final Color ESTIMATED_BALL_COLOR = Color.ORANGE;
    static final Color ESTIMATED_BALL_UNCERT_COLOR = Color.ORANGE;
    static final boolean DRAW_BALL_VELOCITY = true;

    // Particle drawing
    static final int PARTICLE_RADIUS = 3;
    static final int PARTICLE_HEADING_DIST = 10;

    // This instance draws a line from the current estimate of the ball's
    // location to the the estimte of its location in
    // BALL_VELOCITY_DRAWING_MULTIPLIER_SECONDS seconds,
    // as implied by its estimated velocity.

    // Unit: inverse seconds
    static final double  BALL_VELOCITY_DRAWING_MULTIPLIER = 1.0;
    static final int     BALL_VELOCITY_STROKE = 1;

    static final double RAD_TO_DEG = 360. / (2. * Math.PI);
    static final double DEG_TO_RAD = (2. * Math.PI) / 360.;

    // Used when we are only tracking one dog.  Chosen to be a weird number
    // so that it doesen't collide with a dog's udp key
    static final Integer SINGLE_DOG_KEY = new Integer(-1273);

    private int num_positions_to_draw;

    // The number of most recent positions/uncertainties to appear on the
    // field at one time
    private int NUMBER_PREVIOUS_POSITIONS_TO_DRAW = 5;

    // To hold the history for one or more dogs
    private HashMap<Integer, DogHistory> dog_histories =
        new HashMap<Integer, DogHistory>(MAX_DOGS_ON_FIELD);
    // Add a DogHistory instance to handle the case that we are visualizing
    // a single dog
    DogHistory single_dog;

    // because UDP currently doesn't provide heading uncertainty estimates
    static final double ASSUMED_HEADING_UNCERT_FOR_UDP_COMMUNICATIONS = 0.0;
    static final double ASSUMED_HEAD_PAN_FOR_UDP_COMMUNICATIONS = 0.0;

    // A tautological geometric constant
    static final double QUART_CIRC_DEGS = 360. / 4.;

    private boolean draw_real, draw_est;

    private double by_x,by_y;
    private double yb_x,yb_y;

    private int[][] seenLandmarks = new int[30][3];
    private int numSeenLandmarks;
    private static final int X_INDEX = 0;
    private static final int Y_INDEX = 1;
    private static final int COLOR_INDEX = 2;
    private static final int LANDMARK_LINE_WIDTH = 1;

    // Information to be set for drawing particles
    // Particle set for drawing...
    private Vector< MCLParticle > currentParticles;
    // Hold information about the player
    private int mclTeamColor;
    private int mclPlayerNum;

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

        single_dog = new DogHistory();
        single_dog.team = 1;
        single_dog.color = BLUE_TEAM;
        dog_histories.put(SINGLE_DOG_KEY, single_dog);
        numSeenLandmarks = 0;
        currentParticles = new Vector<MCLParticle>();
        mclTeamColor = 0;
        mclPlayerNum = 1;
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
            paintDogInformation(g2);
            paintParticleSet(g2);
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
            int x = seenLandmarks[i][X_INDEX];
            int y = seenLandmarks[i][Y_INDEX];
            int team_color = seenLandmarks[i][COLOR_INDEX];

            if (team_color == RED_TEAM) {
                // Red team is on the opposite side of the field, so fix the
                // x and y coords
                x = (int)field.FIELD_GREEN_WIDTH - x;
                y = (int)field.FIELD_GREEN_HEIGHT - y;
            }
            // Pass off the work to another helper method
            drawSeenLandmark(g2, x, y);
        }
        // Reset for the next frame
        numSeenLandmarks = 0;

        if (DEBUG_SEEN_LANDMARKS) {
            // draw seen by post
            if (by_x != -1)
                field.fillOval(g2,
                               SAW_BLUE_BEACON_COLOR,
                               LANDMARK_LINE_WIDTH,
                               by_x,
                               by_y,
                               field.BEACON_RADIUS,
                               field.BEACON_RADIUS);

            // draw seen yb post
            if (yb_x != -1)
                field.fillOval(g2,
                               SAW_YELLOW_BEACON_COLOR,
                               LANDMARK_LINE_WIDTH,
                               yb_x,
                               yb_y,
                               field.BEACON_RADIUS,
                               field.BEACON_RADIUS);
        }
    }



    public void setField(Field f)
    {
        field = f;
    }


    protected void paintDogInformation(Graphics2D g2)
    {
        if (draw_real) {
            paintDogRealInformation(g2);
        }
        if (draw_est) {
            paintDogEstInformation(g2);
        }
    }

    /**
     * For each of the robots we are tracking, draws its position on the
     * field as well as its idea of where the ball is and has been recently.
     *
     * NOTE:  Unlike in the paint landmarks method where we need to switch the
     * coordinate system to account for the red team, by the time we enter this
     * method all the coordinate systems have been fixed for the dog
     * information.
     * @see updateDog(Dog) update
     */
    protected void paintDogRealInformation(Graphics2D g2)
    {
        // For each of the dogs that we are tracking
        for (DogHistory dog_history : dog_histories.values()) {
            // Draw the dog's most recent actual locations
            for (LocalizationPacket recent_position :
                     dog_history.actual_position_history) {
                drawDogsRealPosition(g2,
                                     recent_position.getXActual(),
                                     recent_position.getYActual(),
                                     recent_position.
                                     getHeadingActual(),
                                     recent_position.
                                     getHeadPanActual());
            }
            // Draw the ball's most recent actual locations
            for (LocalizationPacket recent_position :
                     dog_history.actual_ball_position_history) {
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
     * @see updateDog(Dog) update
     */
    protected void paintDogEstInformation(Graphics2D g2)
    {
        // For each of the dogs that we are tracking
        for (DogHistory dog_history : dog_histories.values()) {
            // Draw the most recent dog estimated locations and uncertainties
            Color dogColor, numberColor;
            if (dog_history.color == RED_TEAM) {
                dogColor = DOG_COLOR_RED_TEAM;
                numberColor = DOG_COLOR_RED_TEAM;
            } else {
                dogColor = DOG_COLOR_BLUE_TEAM;
                numberColor = DOG_COLOR_BLUE_TEAM;
            }
            for (LocalizationPacket recent_estimate :
                     dog_history.estimated_position_history) {
                // draw the location and heading estimate dot and line
                drawDogsEstimatedPosition(g2,
                                          dogColor,
                                          recent_estimate.getXEst(),
                                          recent_estimate.getYEst(),
                                          recent_estimate.
                                          getHeadingEst(),
                                          recent_estimate.
                                          getHeadPanEst(),
                                          dog_history.number);
                // draw the position uncertanity oval, if applicable
                drawDogsUncertainty(g2,
                                    recent_estimate.getXEst(),
                                    recent_estimate.getYEst(),
                                    recent_estimate.getHeadingEst(),
                                    recent_estimate.getXUncert(),
                                    recent_estimate.getYUncert(),
                                    recent_estimate.getHUncert());
                dogColor = dogColor.darker();
            }
            // Draw the most recent ball estimated locations and uncertainties
            Color ballColor = Color.WHITE;
            for (LocalizationPacket recent_estimate :
                     dog_history.estimated_ball_position_history) {
                drawBallEstimatedPosition(g2,
                                          recent_estimate.getXEst(),
                                          recent_estimate.getYEst(),
                                          recent_estimate.
                                          getXVelocity(),
                                          recent_estimate.
                                          getYVelocity(),
                                          dog_history.number,
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


    // draw dog's real positioni
    public void drawDogsRealPosition(Graphics2D drawing_on,
                                     double at_x, double at_y,
                                     double at_heading, double at_pan)
    {
        field.fillOval(drawing_on, REAL_DOG_POSITION_COLOR,
                       field.DRAW_STROKE, at_x, at_y, POSITION_DOT_RADIUS,
                       POSITION_DOT_RADIUS);
        double heading_angle_from_right_horizon = at_heading + QUART_CIRC_DEGS;
        double x_line_body_disp = (POSITION_HEADING_RADIUS *
                                   Math.cos(DEG_TO_RAD *
                                            heading_angle_from_right_horizon));
        double y_line_body_disp = (POSITION_HEADING_RADIUS *
                                   Math.sin(DEG_TO_RAD *
                                            heading_angle_from_right_horizon));
        field.drawLine(drawing_on, REAL_DOG_POSITION_COLOR, field.DRAW_STROKE,
                       at_x - x_line_body_disp, at_y - y_line_body_disp,
                       at_x, at_y);
        double x_line_pan_disp = (PAN_HEADING_RADIUS *
                                  Math.cos(DEG_TO_RAD *
                                           (at_pan +
                                            heading_angle_from_right_horizon
                                            )));
        double y_line_pan_disp = (PAN_HEADING_RADIUS *
                                  Math.sin(DEG_TO_RAD *
                                           (at_pan +
                                            heading_angle_from_right_horizon
                                            )));
        field.drawLine(drawing_on, REAL_DOG_POSITION_COLOR, field.DRAW_STROKE,
                       at_x, at_y, at_x + x_line_pan_disp,
                       at_y + y_line_pan_disp);
    }

    // draw dog's estimated position
    public void drawDogsEstimatedPosition(Graphics2D drawing_on,
                                          Color dogColor, double at_x,
                                          double at_y, double at_heading,
                                          double at_pan, int player_number)
    {
        field.fillOval(drawing_on, dogColor, field.DRAW_STROKE, at_x, at_y,
                       POSITION_DOT_RADIUS, POSITION_DOT_RADIUS);
        // TAIL PART
        double heading_angle_from_right_horizon = at_heading + QUART_CIRC_DEGS;
        double x_line_body_disp = (POSITION_HEADING_RADIUS *
                                   Math.cos(DEG_TO_RAD *
                                            heading_angle_from_right_horizon));
        double y_line_body_disp = (POSITION_HEADING_RADIUS *
                                   Math.sin(DEG_TO_RAD *
                                            heading_angle_from_right_horizon));
        field.drawLine(drawing_on, dogColor, field.DRAW_STROKE,
                       at_x - x_line_body_disp, at_y - y_line_body_disp,
                       at_x, at_y);
        double x_line_pan_disp = (PAN_HEADING_RADIUS *
                                  Math.cos(DEG_TO_RAD *
                                           (at_pan +
                                            heading_angle_from_right_horizon
                                            )));
        double y_line_pan_disp = (PAN_HEADING_RADIUS *
                                  Math.sin(DEG_TO_RAD *
                                           (at_pan +
                                            heading_angle_from_right_horizon
                                            )));
        field.drawLine(drawing_on, dogColor, field.DRAW_STROKE, at_x, at_y,
                       at_x + x_line_pan_disp, at_y + y_line_pan_disp);

        // Here we add a number ontop of the dog
        field.drawNumber(drawing_on, dogColor, at_x+5, at_y+5,
                         player_number);
    }

    // draw dog uncertainty (x,y,h)
    public void drawDogsUncertainty(Graphics2D drawing_on,
                                    double at_x,
                                    double at_y,
                                    double at_heading,
                                    double x_uncert,
                                    double y_uncert,
                                    double h_uncert)
    {
        // draw dog's uncert
        field.drawOval(drawing_on,
                       ESTIMATED_DOG_XY_UNCERT_COLOR,
                       field.DRAW_STROKE,
                       at_x,
                       at_y,
                       x_uncert,
                       y_uncert);

        double heading_angle_from_right_horizon = at_heading + QUART_CIRC_DEGS;

        double half_h_uncert = h_uncert/2.0;

        // HEAD PART
        double x_line_pan_disp_plus = HEADING_UNCERT_RADIUS *
            Math.cos(DEG_TO_RAD * (half_h_uncert +
                                   heading_angle_from_right_horizon));
        double y_line_pan_disp_plus = HEADING_UNCERT_RADIUS *
            Math.sin(DEG_TO_RAD * (half_h_uncert +
                                   heading_angle_from_right_horizon));
        double x_line_pan_disp_minus = HEADING_UNCERT_RADIUS *
            Math.cos(DEG_TO_RAD * (-half_h_uncert +
                                   heading_angle_from_right_horizon));
        double y_line_pan_disp_minus = HEADING_UNCERT_RADIUS *
            Math.sin(DEG_TO_RAD * (-half_h_uncert +
                                   heading_angle_from_right_horizon));

        field.drawLine(drawing_on,
                       ESTIMATED_DOG_HEADING_UNCERT_COLOR,
                       field.DRAW_STROKE,
                       at_x,
                       at_y,
                       at_x + x_line_pan_disp_plus,
                       at_y + y_line_pan_disp_plus);

        field.drawLine(drawing_on,
                       ESTIMATED_DOG_HEADING_UNCERT_COLOR,
                       field.DRAW_STROKE,
                       at_x,
                       at_y,
                       at_x + x_line_pan_disp_minus,
                       at_y + y_line_pan_disp_minus);

    }

    // draw the ball's position from dog ekf
    public void drawBallEstimatedPosition(Graphics2D drawing_on,
                                          double at_x,
                                          double at_y,
                                          double x_velocity,
                                          double y_velocity,
                                          int player_number,
                                          Color dogColor)
    {
        field.fillOval(drawing_on, ESTIMATED_BALL_COLOR, field.DRAW_STROKE,
                       at_x, at_y, field.BALL_RADIUS, field.BALL_RADIUS);
        field.drawLine(drawing_on,
                       ESTIMATED_BALL_COLOR, BALL_VELOCITY_STROKE,
                       at_x, at_y,
                       at_x + x_velocity * BALL_VELOCITY_DRAWING_MULTIPLIER,
                       at_y + y_velocity * BALL_VELOCITY_DRAWING_MULTIPLIER);
        field.drawNumber(drawing_on, dogColor, at_x, at_y, player_number);
    }

    // draws ball's 'actual' position in simulation/camera
    public void drawBallRealPosition(Graphics2D drawing_on,
                                     double at_x,
                                     double at_y)
    {
        System.out.println("drawing real position at (" + at_x + " , " + at_y
                           + ")");

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

    public void drawSeenLandmark(Graphics2D drawing_on, int x, int y)
    {
        field.drawOval(drawing_on, Color.black, 1, x, y, 10, 10);
    }




    public void reportUpdatedActualLocation(LocalizationPacket
                                            actual_location)
    {
        reportUpdatedActualLocation(singleDogHistory(), actual_location);
    }

    public void reportUpdatedActualLocation(DogHistory dog_history,
                                            LocalizationPacket
                                            actual_location)
    {
        if (dog_history.actual_position_history.size() >=
            num_positions_to_draw) {
            dog_history.actual_position_history.removeLast();
        }
        dog_history.actual_position_history.addFirst(actual_location);
    }

    public void reportUpdatedActualBallLocation(LocalizationPacket
                                                actual_ball_location)
    {
        reportUpdatedActualBallLocation(singleDogHistory(),
                                        actual_ball_location);
    }

    public void
        reportUpdatedActualBallLocation(DogHistory dog_history,
                                        LocalizationPacket
                                        actual_ball_location)
    {
        if (dog_history.actual_ball_position_history.size() >=
            num_positions_to_draw) {
            dog_history.actual_ball_position_history.removeLast();
        }
        dog_history.actual_ball_position_history.addFirst(actual_ball_location);
    }

    public void
        reportUpdatedLocalization(LocalizationPacket new_localization_info,
                                  int team_color, int player_number)
    {
        if (team_color == RED_TEAM) {
            transformFieldCoordinatesToRedTeamSide(new_localization_info);
        }
        reportUpdatedLocalization(singleDogHistory(),
                                  new_localization_info,
                                  team_color,
                                  player_number);
    }

    public void
        reportUpdatedLocalization(DogHistory dog_history,
                                  LocalizationPacket new_localization_info,
                                  int team_color, int player_number)
    {
        if (dog_history.estimated_position_history.size() >=
            num_positions_to_draw) {
            dog_history.estimated_position_history.removeLast();
        }
        dog_history.estimated_position_history.addFirst(new_localization_info);
        dog_history.color = team_color;
        dog_history.number = player_number;
    }

    public void
        reportUpdatedBallLocalization(LocalizationPacket
                                      new_ball_localization_info,
                                      int team_color, int player_number)
    {
        if (team_color == RED_TEAM) {
            transformFieldCoordinatesToRedTeamSide(new_ball_localization_info);
        }
        reportUpdatedBallLocalization(singleDogHistory(),
                                      new_ball_localization_info,
                                      team_color,
                                      player_number);
    }

    public void reportUpdatedBallLocalization(DogHistory dog_history,
                                              LocalizationPacket
                                              new_ball_localization_info,
                                              int team_color,
                                              int player_number)
    {
        if (dog_history.estimated_ball_position_history.size() >=
            num_positions_to_draw) {
            dog_history.estimated_ball_position_history.removeLast();
        }
        dog_history.estimated_ball_position_history.
            addFirst(new_ball_localization_info);
        dog_history.color = team_color;
        dog_history.number = player_number;
    }

    public void reportTrackedLandmark(LocalizationPacket landmark_location)
    {
        reportTrackedLandmark(singleDogHistory(), landmark_location);
    }

    public void reportTrackedLandmark(DogHistory dog_history, LocalizationPacket
                                      landmark_location)
    {
        dog_history.visible_landmarks.add(landmark_location);
    }

    public void sawLandmark(int x, int y, int team_color)
    {
        seenLandmarks[numSeenLandmarks][0] = x;
        seenLandmarks[numSeenLandmarks][1] = y;
        seenLandmarks[numSeenLandmarks][2] = team_color;
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

        LocalizationPacket est = singleDogHistory().estimated_position_history.
            getFirst();
        double new_angle = AngleUtilities.sub180Angle(-angle+est.
                                                      getHeadingEst());
        double adj = Math.cos(new_angle * DEG_TO_RAD)*dist;
        double opp = Math.sin(new_angle * DEG_TO_RAD)*dist;

        by_x = opp+est.getXEst();
        by_y = adj+est.getYEst();
    }

    public void sawYellowPost(Double dist, Double angle)
    {
        if (dist == 0) {
            yb_x = -1; yb_y = -1;
            return;
        }

        LocalizationPacket est = singleDogHistory().estimated_position_history.
            getFirst();
        double new_angle = AngleUtilities.sub180Angle(-angle+est.
                                                      getHeadingEst());
        double adj = Math.cos(new_angle * DEG_TO_RAD)*dist;
        double opp = Math.sin(new_angle * DEG_TO_RAD)*dist;

        yb_x = opp+est.getXEst();
        yb_y = adj+est.getYEst();
    }

    public void reportEndFrame()
    {
        viewer.repaint();
        clearVisibleLandmarks();
    }

    public void updateDog(Dog dog)
    {
        // Use the dog's unique number to see if we allready have it in our
        // hash of all dogs, adding it to the hash if we don't
        // Update the dog's history with the uncoming UDP data
        DogHistory this_dogs_history;
        if (dog_histories.containsKey(dog.getHash())) {
            this_dogs_history = dog_histories.get(dog.getHash());
        }
        else {
            this_dogs_history = new DogHistory();
            this_dogs_history.team = dog.getTeam();
            this_dogs_history.color = dog.getColor();
            this_dogs_history.number = dog.getNumber().intValue();
            dog_histories.put(dog.getHash(), this_dogs_history);
        }
        DogData d = dog.getData();
        LocalizationPacket dog_estimate = LocalizationPacket.
            makeEstimateAndUncertPacket(d.getDogX(),
                                        d.getDogY(),
                                        d.getDogHeading(),
                                        ASSUMED_HEAD_PAN_FOR_UDP_COMMUNICATIONS,
                                        d.getDogUncertX(),
                                        d.getDogUncertY(),
                                        d.getDogUncertH());
        LocalizationPacket ball_estimate = LocalizationPacket.
            makeBallEstimateAndUncertPacket(d.getBallX().doubleValue(),
                                            d.getBallY().doubleValue(),
                                            d.getBallXUncert().doubleValue(),
                                            d.getBallYUncert().doubleValue(),
                                            d.getBallXVel(),
                                            d.getBallYVel());
        // If the dog is on the red team, then we need to transform its
        // co-ordinates because it has a different (0,0) that the dogs on the
        // blue team
        if (dog.getColor() == RED_TEAM) {
            transformFieldCoordinatesToRedTeamSide(dog_estimate);
            transformFieldCoordinatesToRedTeamSide(ball_estimate);
        }
        reportUpdatedLocalization(this_dogs_history,
                                  dog_estimate,
                                  dog.getColor(),
                                  dog.getNumber().intValue());
        reportUpdatedBallLocalization(this_dogs_history,
                                      ball_estimate,
                                      dog.getColor(),
                                      dog.getNumber().intValue());
        reportEndFrame();
    }

    public void transformFieldCoordinatesToRedTeamSide(LocalizationPacket
                                                       loc_pack)
    {
        // Currently transforms x_ and y_est, heading_est, x_velocity,
        // and y_velocity
        loc_pack.x_est = field.FIELD_GREEN_WIDTH - loc_pack.x_est;
        loc_pack.y_est = field.FIELD_GREEN_HEIGHT - loc_pack.y_est;
        loc_pack.heading_est = AngleUtilities.sub180Angle(loc_pack.
                                                          heading_est + 180);
        loc_pack.x_velocity = loc_pack.x_velocity * -1.0;
        loc_pack.y_velocity = loc_pack.y_velocity * -1.0;
    }

    public DogHistory singleDogHistory()
    {
        return dog_histories.get(SINGLE_DOG_KEY);
    }

    public void clearEstimatedPositionHistory()
    {
        for (DogHistory dog_history : dog_histories.values()) {
            dog_history.estimated_position_history.clear();
        }
    }
    public void clearEstimatedBallPositionHistory()
    {
        for (DogHistory dog_history : dog_histories.values()) {
            dog_history.estimated_ball_position_history.clear();
        }
    }
    public void clearActualPositionHistory()
    {
        for (DogHistory dog_history : dog_histories.values()) {
            dog_history.actual_position_history.clear();
        }
    }
    public void clearActualBallPositionHistory()
    {
        for (DogHistory dog_history : dog_histories.values()) {
            dog_history.actual_ball_position_history.clear();
        }
    }

    public void clearVisibleLandmarks()
    {
        for (DogHistory dog_history : dog_histories.values()) {
            dog_history.visible_landmarks.clear();
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
     * Paints the current set of particles on the field.  Called by
     * updateInfomration when the WorldControllerViewer is updated.
     *
     * @param g2 The graphics context to be drawn on.
     */
    public void paintParticleSet(Graphics2D g2) {
        for(MCLParticle p : currentParticles) {
            Color partColor;
            if( mclTeamColor == RED_TEAM) {
                partColor = DOG_COLOR_RED_TEAM;
            } else {
                partColor = DOG_COLOR_BLUE_TEAM;
            }
            drawParticle(g2, partColor, field.fieldToScreenX(p.getX()),
                         field.fieldToScreenY(p.getY()), p.getH() - 90.0f,
                         p.getWeight());
        }
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

    /**
     * Method draws an elipse of the associated position error centered at the
     * weighted mean of the particle set.
     *
     * @param meanX The weighted mean of the x estimate of the particles
     * @param meanY The weighted mean of the y estimate of the particles
     * @param meanH The weighted mean of the heading estimate of the particles
     */
    public void drawMCLMeanAndVariance(float meanX, float meanY, float meanH)
    {
    }
}