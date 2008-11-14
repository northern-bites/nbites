package edu.bowdoin.robocup.TOOL.WorldController;

import javax.swing.*;
import javax.swing.event.*;
import java.awt.event.*;
import java.awt.*;
import java.awt.geom.Point2D;
import java.awt.GridLayout;
import java.util.HashMap;

public class DebugViewer extends JFrame {

    public static final int BOX_HEIGHT = 700;
    public static final int BOX_WIDTH = 300;
    public static final int LOC_PANEL_ENTRY_WIDTH = 400;
    public static final int LOC_PANEL_ENTRY_HEIGHT = 25;
    public static final int LOC_PANEL_WIDTH = BOX_WIDTH;
    public static final int LOC_PANEL_HEIGHT = BOX_HEIGHT/2;

    // string constants
    public static final String FRAME_STRING = "Frame:";
    public static final String OUT_OF_STRING = "of";
    public static final String
	MY_ESTIMATES_STRING = "-----MY LOC ESTIMATES-----";
    public static final String
	BALL_ESTIMATES_STRING = "-----BALL ESTIMATES-----";
    public static final String LANDMARKS_STRING = "-----LANDMARKS-----";
    public static final String MY_XYH_STRING = "X,Y,H:";
    public static final String UNCERT_XYH_STRING = "Uncert X,Y,H:";
    public static final String BALL_X_STRING = "Ball X:";
    public static final String BALL_Y_STRING = "Ball Y:";
    public static final String BALL_UNCERT_X_STRING = "Uncert X:";
    public static final String BALL_UNCERT_Y_STRING = "Y:";
    public static final String BALL_VEL_X_STRING = "Velocity X:";
    public static final String BALL_VEL_Y_STRING = "Y:";
    public static final String BALL_VEL_ABS_STRING = "Abs Velocity:";
    public static final String BALL_VEL_UNCERT_X_STRING = "Vel. Uncert X: ";
    public static final String BALL_VEL_UNCERT_Y_STRING = "Y: ";
    public static final String BALL_STRING = "Ball Dist/Bearing:";
    public static final String ODO_XYH_CHANGE_STRING = "Odo X,Y,H:";
    public static final String ODO_X_CHANGE_STRING = "Odo X:";
    public static final String ODO_Y_CHANGE_STRING = "Odo Y:";
    public static final String ODO_H_CHANGE_STRING = "Odo H:";
    public static final String
	LANDMARK_STUFF_STRING = "ID  :  Dist  /  Bearing";

    // landmark string array
    public final static String[] LANDMARKS = {"My Goal Left Post",
					      "My Goal Right Post",
					      "Opp Goal Left Post",
					      "Opp Goal Right Post",
					      "Left Beacon",
					      "Right Beacon",
					      "Ball",
					      "My Corner Left L",
					      "My Corner Right L",
					      "My Goal Left T",
					      "My Goal Right T",
					      "My Goal Left L",
					      "My Goal Right L",
					      "Center Left T",
					      "Center Right T",
					      "Opp Corner Left L",
					      "Opp Corner Right L",
					      "Opp Goal Left T",
					      "Opp Goal Right T",
					      "Opp Goal Left L",
					      "Opp Goal Right L"};
    // takes a Point and maps it to a string
    public HashMap <Point2D.Double,String> cornerMap;

    // frame labels
    private JLabel frameLabel1, frameLabel2;
    public JLabel frameNumber, frameTotal;
    private JPanel framePanel;

    // master label panel declarations
    private JLabel myEstimatesLabel;
    private JPanel myEstimatesPanel;
    private JLabel ballEstimatesLabel;
    private JPanel ballEstimatesPanel;
    private JLabel landmarksLabel;
    private JPanel landmarksPanel;
    private JLabel landmarkStuffLabel;
    private JPanel landmarkStuffPanel;

    // my values
    private JLabel myXYHLabel;
    public JLabel myX;
    public JLabel myY;
    public JLabel myH;
    private JPanel myXYHPanel;

    // my uncertainty
    private JLabel myUncertXYHLabel;
    public JLabel myUncertX;
    public JLabel myUncertY;
    public JLabel myUncertH;
    private JPanel myUncertXYHPanel;

    // odometry values
    private JLabel odoXYHLabel;
    public JLabel odoX;
    public JLabel odoY;
    public JLabel odoH;
    private JPanel odoXYHPanel;

    // ball x/y estimates
    private JLabel ballXLabel;
    public JLabel ballX;
    private JLabel ballYLabel;
    public JLabel ballY;
    private JPanel ballXYPanel;

    // ball uncertainty estimates
    private JLabel ballUncertXLabel;
    public JLabel ballUncertX;
    private JLabel ballUncertYLabel;
    public JLabel ballUncertY;
    private JPanel ballUncertPanel;

    // ball velocity estimates
    private JLabel ballVelXLabel;
    public JLabel ballVelX;
    private JLabel ballVelYLabel;
    public JLabel ballVelY;
    private JPanel ballVelPanel;

    private JLabel ballVelAbsLabel;
    public JLabel ballVelAbs;
    private JPanel ballVelAbsPanel;

    // ball velocity estimates
    private JLabel ballVelUncertXLabel;
    public JLabel ballVelUncertX;
    private JLabel ballVelUncertYLabel;
    public JLabel ballVelUncertY;
    private JPanel ballVelUncertPanel;

    private WorldController wc;
    private Container window;
    private Component bottom_space;
    private JPanel locPanel;
    private Component landmark_components[];
    private int num_landmarks;

    public DebugViewer(WorldController _wc, int x, int y) {
	super("Debug Viewer");

	// local copy of the WorldController instance
	wc = _wc;

	JFrame.setDefaultLookAndFeelDecorated(true);

	setLayout(new BoxLayout(this.getContentPane(), BoxLayout.PAGE_AXIS));

	window = getContentPane();
	num_landmarks = 0;
	landmark_components = new Component[10];

        cornerMap = new HashMap<Point2D.Double,String>();

	// frame number

        frameLabel1 = new JLabel(FRAME_STRING, JLabel.CENTER);
	frameLabel2 = new JLabel(OUT_OF_STRING, JLabel.CENTER);
	frameNumber = new JLabel("1", JLabel.CENTER);
	frameTotal = new JLabel("1", JLabel.CENTER);
       	framePanel = new JPanel();
	framePanel.add(frameLabel1);
	framePanel.add(frameNumber);
	framePanel.add(frameLabel2);
	framePanel.add(frameTotal);

	// estimates label
	myEstimatesLabel = new JLabel(MY_ESTIMATES_STRING, JLabel.CENTER);
	myEstimatesPanel = new JPanel();
	myEstimatesPanel.add(myEstimatesLabel);
	myEstimatesPanel.
	    setPreferredSize(new Dimension(LOC_PANEL_ENTRY_WIDTH,
					   LOC_PANEL_ENTRY_HEIGHT));

	// ball estimates label
	ballEstimatesLabel = new JLabel(BALL_ESTIMATES_STRING, JLabel.CENTER);
	ballEstimatesPanel = new JPanel();
	ballEstimatesPanel.add(ballEstimatesLabel);
	ballEstimatesPanel.
	    setPreferredSize(new Dimension(LOC_PANEL_ENTRY_WIDTH,
					   LOC_PANEL_ENTRY_HEIGHT));

	// landmarks label
	landmarksLabel = new JLabel(LANDMARKS_STRING, JLabel.CENTER);
	landmarksPanel = new JPanel();
	landmarksPanel.add(landmarksLabel);
	landmarksPanel.
	    setPreferredSize(new Dimension(LOC_PANEL_ENTRY_WIDTH,
					   LOC_PANEL_ENTRY_HEIGHT));

	landmarkStuffLabel = new JLabel(LANDMARK_STUFF_STRING, JLabel.CENTER);
	landmarkStuffPanel = new JPanel();
	landmarkStuffPanel.add(landmarkStuffLabel);
	landmarkStuffPanel.
	    setPreferredSize(new Dimension(LOC_PANEL_ENTRY_WIDTH,
					   LOC_PANEL_ENTRY_HEIGHT));

	// my estimates
	myXYHLabel = new JLabel(MY_XYH_STRING, JLabel.CENTER);
	myX = new JLabel("0", JLabel.CENTER);
	myY = new JLabel("0", JLabel.CENTER);
	myH = new JLabel("0", JLabel.CENTER);
	myXYHPanel = new JPanel();
	myXYHPanel.add(myXYHLabel);
	myXYHPanel.add(myX);
	myXYHPanel.add(myY);
	myXYHPanel.add(myH);
	myXYHPanel.
	    setPreferredSize(new Dimension(LOC_PANEL_ENTRY_WIDTH,
					   LOC_PANEL_ENTRY_HEIGHT));

	// my uncertainty estimates
	myUncertXYHLabel = new JLabel(UNCERT_XYH_STRING, JLabel.CENTER);
	myUncertX = new JLabel("0", JLabel.CENTER);
	myUncertY = new JLabel("0", JLabel.CENTER);
	myUncertH = new JLabel("0", JLabel.CENTER);
	myUncertXYHPanel = new JPanel();
	myUncertXYHPanel.add(myUncertXYHLabel);
	myUncertXYHPanel.add(myUncertX);
	myUncertXYHPanel.add(myUncertY);
	myUncertXYHPanel.add(myUncertH);
	myUncertXYHPanel.
	    setPreferredSize(new Dimension(LOC_PANEL_ENTRY_WIDTH,
					   LOC_PANEL_ENTRY_HEIGHT));

	// odometry estimate initializations
	odoXYHLabel = new JLabel(ODO_XYH_CHANGE_STRING, JLabel.CENTER);
	odoX = new JLabel("0", JLabel.CENTER);
	odoY = new JLabel("0", JLabel.CENTER);
	odoH = new JLabel("0", JLabel.CENTER);
	odoXYHPanel = new JPanel();
	odoXYHPanel.add(odoXYHLabel);
	odoXYHPanel.add(odoX);
	odoXYHPanel.add(odoY);
	odoXYHPanel.add(odoH);

	ballXLabel = new JLabel(BALL_X_STRING, JLabel.CENTER);
	ballX = new JLabel("0", JLabel.CENTER);
	ballYLabel = new JLabel(BALL_Y_STRING, JLabel.CENTER);
	ballY = new JLabel("0", JLabel.CENTER);
	ballXYPanel = new JPanel();
	ballXYPanel.add(ballXLabel);
	ballXYPanel.add(ballX);
	ballXYPanel.add(ballYLabel);
	ballXYPanel.add(ballY);
	ballXYPanel.
	    setPreferredSize(new Dimension(LOC_PANEL_ENTRY_WIDTH,
					   LOC_PANEL_ENTRY_HEIGHT));

	ballUncertXLabel = new JLabel(BALL_UNCERT_X_STRING, JLabel.CENTER);
	ballUncertX = new JLabel("0", JLabel.CENTER);
	ballUncertYLabel = new JLabel(BALL_UNCERT_Y_STRING, JLabel.CENTER);
	ballUncertY = new JLabel("0", JLabel.CENTER);
	ballUncertPanel = new JPanel();
	ballUncertPanel.add(ballUncertXLabel);
	ballUncertPanel.add(ballUncertX);
	ballUncertPanel.add(ballUncertYLabel);
	ballUncertPanel.add(ballUncertY);
	ballUncertPanel.
	    setPreferredSize(new Dimension(LOC_PANEL_ENTRY_WIDTH,
					   LOC_PANEL_ENTRY_HEIGHT));

	ballVelXLabel = new JLabel(BALL_VEL_X_STRING, JLabel.CENTER);
	ballVelX = new JLabel("0", JLabel.CENTER);
	ballVelYLabel = new JLabel(BALL_VEL_Y_STRING, JLabel.CENTER);
	ballVelY = new JLabel("0", JLabel.CENTER);
	ballVelPanel = new JPanel();
	ballVelPanel.add(ballVelXLabel);
	ballVelPanel.add(ballVelX);
	ballVelPanel.add(ballVelYLabel);
	ballVelPanel.add(ballVelY);
	ballVelPanel.
	    setPreferredSize(new Dimension(LOC_PANEL_ENTRY_WIDTH,
					   LOC_PANEL_ENTRY_HEIGHT));

	// Add something to show absolute velocity
	ballVelAbsLabel = new JLabel(BALL_VEL_ABS_STRING, JLabel.CENTER);
	ballVelAbs = new JLabel("0", JLabel.CENTER);
	ballVelAbsPanel = new JPanel();
	ballVelAbsPanel.add(ballVelAbsLabel);
	ballVelAbsPanel.add(ballVelAbs);
	ballVelPanel.
	    setPreferredSize(new Dimension(LOC_PANEL_ENTRY_WIDTH,
					   LOC_PANEL_ENTRY_HEIGHT));

	ballVelUncertXLabel = new JLabel(BALL_VEL_UNCERT_X_STRING,
					 JLabel.CENTER);
	ballVelUncertX = new JLabel("0", JLabel.CENTER);
	ballVelUncertYLabel = new JLabel(BALL_VEL_UNCERT_Y_STRING,
					 JLabel.CENTER);
	ballVelUncertY = new JLabel("0", JLabel.CENTER);
	ballVelUncertPanel = new JPanel();
	ballVelUncertPanel.add(ballVelUncertXLabel);
	ballVelUncertPanel.add(ballVelUncertX);
	ballVelUncertPanel.add(ballVelUncertYLabel);
	ballVelUncertPanel.add(ballVelUncertY);
	ballVelUncertPanel.
	    setPreferredSize(new Dimension(LOC_PANEL_ENTRY_WIDTH,
					   LOC_PANEL_ENTRY_HEIGHT));


	// CALIBRATE BUTTONS LAYOUT

	locPanel = new JPanel();

	locPanel.add(framePanel);
	locPanel.add(myEstimatesPanel);
	locPanel.add(myXYHPanel);
	locPanel.add(myUncertXYHPanel);
	locPanel.add(odoXYHPanel);
	locPanel.add(ballEstimatesPanel);
	locPanel.add(ballXYPanel);
	locPanel.add(ballUncertPanel);
	locPanel.add(ballVelPanel);
	locPanel.add(ballVelAbsPanel);
	locPanel.add(ballVelUncertPanel);
	locPanel.add(landmarksPanel);
	locPanel.add(landmarkStuffPanel);

	locPanel.setPreferredSize(new Dimension(LOC_PANEL_WIDTH,
						LOC_PANEL_HEIGHT));
	window.add(locPanel);

	setLocation(x,y);

	pack();
	setVisible(false);

	populateCornerMap();
    }

    private void populateCornerMap() {
	// blue corner left l
	Point2D.Double blueCornerLeftL =
	    new Point2D.Double(wc.the_field.FIELD_WHITE_LEFT_SIDELINE_X,
			       wc.the_field.FIELD_WHITE_BOTTOM_SIDELINE_Y);
	cornerMap.put(blueCornerLeftL, "Blue Corner Left L");
    }

    public Dimension getMinimumSize()
    { return new Dimension(BOX_WIDTH,BOX_HEIGHT); }

    public Dimension getPreferredSize()
    { return getMinimumSize(); }

    public int getNumLandmarks() {
        return num_landmarks;
    }

    public void addLandmark(int id, double dist, double bearing) {
	if (id < 0 || id >= wc.NUM_LANDMARKS) {
	    System.out.println("DebugViewer.java sawLandmark(): " +
			       "Saw Non-Existant Landmark: " + id +
                               " at line " + frameNumber.getText());
	    return;
	}
	//System.out.println("addLandmark id: " + id +
	//		   " dist: " + dist +
	//		   " bearing: " + bearing);

	// label/panel declarations
	JLabel id_label, dist_label, bearing_label;
	JLabel colon_label, slash_label;
	JPanel panel;

	// get id string from LANDMARKS array
	id_label = new JLabel(LANDMARKS[id], JLabel.CENTER);
	// convert dist/bearing to strings, add to labels
	dist_label = new JLabel("" + dist, JLabel.CENTER);
	bearing_label = new JLabel("" + bearing, JLabel.CENTER);
	// make a colon and slash as JLabels
	colon_label = new JLabel(":", JLabel.CENTER);
	slash_label = new JLabel("/", JLabel.CENTER);

	// create and add to panel
	panel = new JPanel();
	panel.add(id_label);
	panel.add(colon_label);
	panel.add(dist_label);
	panel.add(slash_label);
	panel.add(bearing_label);
	panel.setAlignmentY(Component.TOP_ALIGNMENT);

	// add panel to window
	landmark_components[num_landmarks] = locPanel.add(panel);
	//window.validate();

	// incremenet landmarks on window
	num_landmarks++;
    }

    //public void addSeenCorner(

    // removes all landmarks from debug viewer
    public void removeLandmarks() {

	//window.invalidate();
	// remove landmarks
	for (int i = 0; i < num_landmarks; i++) {
	    locPanel.remove(landmark_components[i]);
	}
	locPanel.updateUI();
	//window.validate();
        num_landmarks = 0;
    }

    /*
    // input is: LANDMARK id index , and value
    // this function maps the id to the proper label
    public void displayLocData(int id, Double value) {
	JLabel lbl = myX;
	if (id == wc.TCP_LOC_MY_X) {
	    lbl = myX;
	}
	else if (id == wc.TCP_LOC_MY_Y) {
	    lbl = myY;
	}
	else if (id == wc.TCP_LOC_MY_H) {
	    lbl = myH;
	}
	else if (id == wc.TCP_LOC_MY_UNCERT_X) {
	    lbl = myUncertX;
	}
	else if (id == wc.TCP_LOC_MY_UNCERT_Y) {
	    lbl = myUncertY;
	}
	else if (id == wc.TCP_LOC_MY_UNCERT_H) {
	    lbl = myUncertH;
	}
	else if (id == wc.TCP_LOC_BALL_X) {
	    lbl = ballX;
	}
	else if (id == wc.TCP_LOC_BALL_Y) {
	    lbl = ballY;
	}
	else if (id == wc.TCP_LOC_BALL_UNCERT_X) {
	    lbl = ballUncertX;
	}
	else if (id == wc.TCP_LOC_BALL_UNCERT_Y) {
	    lbl = ballUncertY;
	}
	else if (id == wc.TCP_LOC_BALL_VEL_X) {
	    lbl = ballVelX;
	}
	else if (id == wc.TCP_LOC_BALL_VEL_Y) {
	    lbl = ballVelY;
	}
	else if (id == wc.TCP_LOC_ODO_X) {
	    lbl = odoX;
	}
	else if (id == wc.TCP_LOC_ODO_Y) {
	    lbl = odoY;
	}
	else if (id == wc.TCP_LOC_ODO_Y) {
	    lbl = odoY;
	}


	lbl.setText("" + value);
	}*/
}
