package TOOL.Classifier;

import javax.swing.*;
import java.awt.*;
import java.awt.image.*;

import java.awt.event.*;
import java.io.*;
import java.util.Arrays;
import java.util.ArrayList;

import TOOL.TOOL;
import TOOL.Misc.Pair;
import TOOL.Calibrate.VisionState;
import TOOL.ColorEdit.*;
import TOOL.Data.DataListener;
import TOOL.Data.DataSet;
import TOOL.Data.Frame;
import TOOL.Image.*;

import TOOL.Image.ClassifierDrawingPanel;

import java.util.LinkedList;


/**
 * @author Nicholas Dunn
 * A system to allow quantitative analysis of Vision systems.
 *
 * This class provides the backbone for our classification system.  The actual
 * drawing code is done within ClassifierDrawingPanel, while ClassifierPanel
 * handles the buttons and most buttons on the screen.
 *
 * The idea behind this class is that a user will manually go through a series
 * of frames and classify what he finds in each; for instance he will drag
 * a rectangle around the goal post and then say that the box he just drew 
 * was the Blue Left GoalPost or the Blue Backstop, for instance.
 *
 * The system records the relevant data (for instance, the id of a corner
 * and its (x,y) location in the image.  It provides ways of saving to file
 * so that the data is persistent and in a formate easily accessible to our
 * Vision processing system.  The C code can then compare the information with
 * what it generates in order to see which areas are working correctly or not.
 * This is important with the newfound emphasis on lines.
 *
 *
 * The format for the logged files will be as follows:
 * Header information[int numFrames, String robotName]
 * [
 * [ Ball object info (int id, int upperLeftCornerX, int upperLeftCornerY, 
        int width, int height)], 
 * [ Goal info (int id, int x1, int y1, int x2, int y2, int x3, int y3, 
 *      int x4, int y4),... ],
 * [ Beacon info (int id, int x1, int y1, int x2, int y2, int x3, int y3, 
 *      int x4, int y4),...]
 * [ line info (int id, int x1, int y1, int x1, int x2),... ]
 * [ center circle info (int id, int upperLeftCornerX, int upperLeftCornerY, 
 *        int width, int height)],
 * [ corner info (int type (L or T), int id, int x1, int y1, int x1, int x2),
 *   ...]
 * [ colored arcs info (int id, int endpoint1.x, int endpoint1.y,
 *       int endpoint2.x, int endpoint2.y, int radius.x, int radius.y)],
 * FILENUM.FRM ]
 *\newline
 * [...
 * and so on and so forth.
 * Whenever the system is expanded (for instance, to allow classifying of
 * robots maybe?) you should ALWAYS add the logging information to the END.
 * That way we can easily make the conversion later on, both in C and in Java.
 */


public class Classifier extends JPanel implements DataListener,
                                                  MouseListener,
                                                  MouseWheelListener,
                                                  ActionListener{
    // Holds all the modules
    protected TOOL tool;
    protected TOOLImage rawImage;
    // Pane in which all the drawing gets done
    protected ClassifierDrawingPanel displayer;
    // Holds all the GUI components (buttons, JTextAreas) 
    protected ClassifierPanel panel; 
    
    protected int imageHeight, imageWidth;
    protected int[] choice;
    
    // Used for a hackish sort of way to pop up a JDialog at a specific
    // point on the screen; the static methods don't let you specify an (x,y)
    // coordinate, which we need.
    protected JComboBox text;
    protected JCheckBox occluded;
    protected JButton okButton;
    protected JButton cancelButton;
    protected JOptionPane optionPane;
    protected JDialog dialog;
    
    // Number of images in the current directory; determines the size of the
    // array we allocate
    protected int numImages = -1;
    // The index into our array
    protected int curImageIndex;
    protected int index = 0;

    // Holds the objects themselves
    protected ClassifierObject[][] log;//ArrayList<ClassifierObject[]> log;
    // Holds the number of each type of object
    protected int[][] objectCount;
    protected DataSet images = null;


    // For storage space purposes, put a hard cap on the number of corners
    // etc., that the user can classify in a single frame.  Given that
    // in an actual situation there will never be this many corners visible,
    // should never be an issue.
    public static final int MAX_N_BALLS = 1;    // Our vision system is built
                                                // for just one ball
    public static final int MAX_N_GOAL_PARTS = 6; // 3 per goal, two goals
    public static final int MAX_N_BEACONS = 2;  // by, yb
    public static final int MAX_N_LINES = 10;   
    public static final int MAX_N_CCS = 1;      // only one cc
    public static final int MAX_N_CORNERS = 10;
    public static final int MAX_N_ARCS = 4;    // one on each corner of field
    public static final int MAX_OBJECTS = MAX_N_CORNERS + MAX_N_LINES +
        MAX_N_CCS + MAX_N_GOAL_PARTS + MAX_N_BEACONS + MAX_N_ARCS;

    public static final int[] LIMITS = new int[] {
        MAX_N_BALLS,
        MAX_N_GOAL_PARTS,
        MAX_N_BEACONS,
        MAX_N_LINES,
        MAX_N_CCS,
        MAX_N_CORNERS,
        MAX_N_ARCS,
        MAX_OBJECTS
    };
        
    public ClassifierDrawingPanel.DrawingMode amode;

    // Ball, Goal, Beacon, Line, Center circle, Corner, CornerArc
    public static final int NUM_OBJECTS = 7;
    public static final int BALL_INDEX = 0;
    public static final int GOAL_INDEX = 1;
    public static final int BEACON_INDEX = 2;
    public static final int LINE_INDEX = 3;
    public static final int CENTER_CIRCLE_INDEX = 4;
    public static final int CORNER_INDEX = 5;
    public static final int CORNER_ARC_INDEX = 6;


    /** The mode you are in corresponds to the entry chosen from the
     * ClassifierPanel.  Each mode has a corresponding list of Strings which
     * are used to classify the specific object you are classifying, 
     * as well as a shape which is used by the ClassifierDrawingPanel to 
     * determine what shape to draw when the mouse is dragged.  Also
     * contains a color that dictates how the shape is drawn
     */
    public enum ClassMode {
        BALL (Ball.IDS, ClassifierDrawingPanel.DrawingMode.ELLIPSE,
              Color.PINK, BALL_INDEX),
            LINE (Line.IDS, ClassifierDrawingPanel.DrawingMode.LINE,
                  Color.CYAN, LINE_INDEX), 
            CENTER_CIRCLE (CenterCircle.IDS, 
                           ClassifierDrawingPanel.DrawingMode.ELLIPSE,
                           Color.CYAN, CENTER_CIRCLE_INDEX), 
            L_CORNER (Corner.L_CORNER_IDS, 
                      ClassifierDrawingPanel.DrawingMode.POINT,
                      Color.ORANGE, CORNER_INDEX),
            T_CORNER (Corner.T_CORNER_IDS,
                      ClassifierDrawingPanel.DrawingMode.POINT,
                      Color.ORANGE, CORNER_INDEX),
            BLUE_CORNER_ARC (CornerArc.BLUE_IDS,
                             ClassifierDrawingPanel.DrawingMode.ARC,
                             Color.BLUE, CORNER_ARC_INDEX),
            YELLOW_CORNER_ARC (CornerArc.YELLOW_IDS,
                               ClassifierDrawingPanel.DrawingMode.ARC,
                               Color.YELLOW, CORNER_ARC_INDEX), 
            BEACON (Beacon.IDS, ClassifierDrawingPanel.DrawingMode.RECT,
                    Color.BLACK, BEACON_INDEX),
            BLUE_GOAL (Goal.BLUE_IDS,
                       ClassifierDrawingPanel.DrawingMode.RECT,
                       Color.BLUE, GOAL_INDEX),
            YELLOW_GOAL (Goal.YELLOW_IDS,
                         ClassifierDrawingPanel.DrawingMode.RECT,
                         Color.YELLOW, GOAL_INDEX);
        
        private int arrayIndex;
        private String[] ids;
        private ClassifierDrawingPanel.DrawingMode shape;
        private Color color;
        ClassMode(String[] ids, ClassifierDrawingPanel.DrawingMode shape,
                  Color color, int arrayIndex) {
            this.ids = ids;
            this.shape = shape;
            this.color = color;
            this.arrayIndex = arrayIndex;
        }
        public String[] getStrings() { return ids; }
        public ClassifierDrawingPanel.DrawingMode getShape() { return shape; }
        public Color getColor() { return color; }
        public int getArrayIndex() { return arrayIndex; }
    } 
    // Which mode we're currently in.
    protected ClassMode mode;
   
    
    /** Constructor requires a TOOL module be passed in.  Defaults to line 
     * classification */
    public Classifier(TOOL t) {
        super();
        tool = t;
        // Default to classifying lines
        mode = ClassMode.LINE;
        setupWindowsAndListeners();
    }


    /**
     * Deletes all currently classified objects on the screen.
     */
    public void clearCanvas() {
        for (int i = 0; i < log[curImageIndex].length; i++) {
            log[curImageIndex][i] = null;
        }
        for (int i = 0; i < objectCount[curImageIndex].length; i++) {
            objectCount[curImageIndex][i] = 0;
        }
    }


    public void setText(String s) {
        panel.setText(s);
    }

    public void mouseWheelMoved(MouseWheelEvent e){
	
	// Change pictures
	if (e.isMetaDown() || e.isControlDown()) {
	    if (e.getWheelRotation() > 0) {
		tool.getDataManager().next();
            }
	    else{
		tool.getDataManager().last();
	    }
	    return;
	}
        else {
            if (e.getWheelRotation() > 0) {
	    
            }
	    else{
	
	    }
        }
    }

    /** TODO - Move this into DATA!!!!   This is not Classifier-specific code,
     * it is general data loading/saving code for object data in a Frame.
     * Please move to, for example, FrameLoader in TOOL.Data.File.  This code
     * is not valid for any DataSet other than FileSet.  Add the generic
     * interface to Datum and DataSet interface, then implement the FileSet
     * version.  Then later, I can implement the SqlSet version for the
     * database.
     *
     * Thanks.   -- Jeremy
     */
    public void writeLogFile() {
        String dataSetPath = tool.getDataManager().activeSet().path();
        FileOutputStream fos = null;
        PrintStream p = null;
        try {
            fos = new FileOutputStream(dataSetPath + "/log.txt");
            p = new PrintStream(fos);
        } catch (Exception e) {
            e.printStackTrace(System.err);
        }
		

    	for (int i = 0; i < log.length; i++) {
            try {
                p.print("Frame: " + i + "\n");
            } catch (Exception e) {
                        
            }
			
            for (int j = 0; j < log[i].length; j++) {
                if( log[i][j] != null ) {
                    try {
                        p.print("\t" + log[i][j].toLog() + "\n");
                    } catch (Exception e) {
                        e.printStackTrace(System.err);
                    }
    
//					System.out.println(path + " " + log[i][j].toLog());
                }
            }
        }
        p.close();
        try {
            fos.close();
        } catch (Exception e) {
                
        }
    }

    /**
     * Initialize the windows, add key and mouse listeners
     */ 
    private void setupWindowsAndListeners() {

        displayer = new ClassifierDrawingPanel(this);
        panel = new ClassifierPanel(this);
                        
        // We want the drawing panel to be able to interpret mouse drags
        displayer.addMouseListener(this);
        addMouseWheelListener(this);
        
        setLayout(new GridLayout(2,0));

        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        
        c.weightx = 1;
        c.weighty = 5;
        c.fill = GridBagConstraints.BOTH;
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.gridheight = GridBagConstraints.RELATIVE;
        gridbag.setConstraints(this, c);

        add(displayer);
        add(panel);
        
        // Initialize the optionPane so that we can classify things
        optionPane = new JOptionPane(
                                     "Classify component as:",
                                     JOptionPane.PLAIN_MESSAGE,
                                     JOptionPane.DEFAULT_OPTION,
                                     null,null);
        okButton = new JButton("OK");
        okButton.addActionListener(this);
    }

    
    public ClassifierDrawingPanel getDrawingPanel() {
        return displayer;
    }

    /**
     * @param x the raw screen x coordinate of corner
     * @param y the raw screen y coordinate of corner
     */
    public void addCorner(int x, int y) {
        int imageX, imageY;
    }

    public void addRectangularObject(int sx, int sy, Pair<Point,Point> line1,
                          Pair<Point,Point> line2) {
        // Move our x y loc to correspond to global screen coordinates
        Point loc = tool.getLocation();
        int[] classification = popupDialogBox(sx + (int)loc.getX(),
                                            sy + (int)loc.getY());
        int globalID;
        if (classification[0] == -1) {
            return;
        }
         
        // We have a valid field object that user classified; figure
        // out its global ID (e.g. what type of corner it is as 
        // opposed to what type of L corner it is).
        globalID = getGlobalID(classification[0]);
        
        ClassifierObject obj = ClassifierObject.createObject(mode,
                                                             globalID,
                                                             line1, line2);
        if (obj != null) {
            obj.setOccluded(classification[1] > 0);
            if (objectCount[curImageIndex][mode.getArrayIndex()] ==
                LIMITS[mode.getArrayIndex()]) {
                panel.setText("Too many " + mode.toString() + " objects on " +
                              "field; max of " + LIMITS[mode.getArrayIndex()]
                              + " already reached.");
                return;
            }
            else {

                System.out.println("object count: " + objectCount[curImageIndex][mode.getArrayIndex()] + " limit: " + LIMITS[mode.getArrayIndex()]);


                objectCount[curImageIndex][mode.getArrayIndex()]++;
                log[curImageIndex][index++] = obj;
                displayer.drawClassifiedShapes(log[curImageIndex]);
                
            }
        }
        
        
    }

    
    /** 
     * @param toChange the ClassifierObject being selected to change
     * @param sx screen x coordinate where point is
     * @param sy screen y coordinate where point is
     */
    public void changeID(ClassifierObject toChange, int sx, int sy) {
        Point loc = tool.getLocation();
        int[] classification = popupDialogBox(sx + (int)loc.getX(),
                                            sy + (int)loc.getY());
        int globalID;
        // they canceled changing the point
        if (classification[0] == -1) {
            return;
        }
        globalID = getGlobalID(classification[0]);
        toChange.setID(globalID);
        toChange.setOccluded(classification[1] > 0);
    }


    /** 
     * @param sx the screen coordinate of last endpoint's x value
     * @param sy the screen coordinate of last endpoint's y value
     * @param x1 *image* coordinate of endpoint1.x
     * @param y1 *image* coordinate of endpoint1.y
     * @param x2 *image* coordinate of endpoint2.x
     * @param y2 *image* coordinate of endpoint2.y
     * @param x3 *image* coordinate of arcRadius.x - used only for arcs
     * @param y3 *image* coordinate of arcRadius.y - used only for arcs
     */
    public void addObject(int sx, int sy, int x1, int y1,
                          int x2, int y2, int x3, int y3) {
        // Move our x y loc to correspond to global screen coordinates
        Point loc = tool.getLocation();
        int[] classification = popupDialogBox(sx + (int)loc.getX(),
                                            sy + (int)loc.getY());
        int globalID;
        if (classification[0] == -1) {
            return;
        }
         
        // We have a valid field object that user classified; figure
        // out its global ID (e.g. what type of corner it is as 
        // opposed to what type of L corner it is).
        globalID = getGlobalID(classification[0]);
        
        
        ClassifierObject obj = ClassifierObject.createObject(mode,
                                                             globalID,
                                                             x1, y1, 
                                                             x2, y2,
                                                             x3, y3);
        int type;
        
        if (obj != null) {
            obj.setOccluded(classification[1] > 0);
            if (objectCount[curImageIndex][mode.getArrayIndex()] ==
                LIMITS[mode.getArrayIndex()]) {
                panel.setText("Too many " + mode.toString() + " objects on " +
                              "field; max of " + LIMITS[mode.getArrayIndex()]
                              + " already reached.");
                return;
            }
            else {

                System.out.println("object count: " + objectCount[curImageIndex][mode.getArrayIndex()] + " limit: " + LIMITS[mode.getArrayIndex()]);


                objectCount[curImageIndex][mode.getArrayIndex()]++;
                log[curImageIndex][index++] = obj;
                displayer.drawClassifiedShapes(log[curImageIndex]);
                
            }/*
            log[curImageIndex][index++] = obj;
        }
        displayer.drawClassifiedShapes(log[curImageIndex]);
             */
        }

    }

    
    /**
     * Convert the string based index into the object's global indexing
     * system.  For instance, the indices into the L_CORNER array
     * do not match the global constants for a CORNER_ID, hence
     * the necessity of a conversion.  Certain field objects do not
     * have such a distinction, however, and they can return the
     * raw value.
     */
    public int getGlobalID(int x) {
        switch (mode) {
        case BALL:
        case LINE:
        case CENTER_CIRCLE:
        case BEACON:
            return x;
        case L_CORNER:
            return Corner.lIDToGlobal(x);
        case T_CORNER:
            return Corner.tIDToGlobal(x);
        case BLUE_CORNER_ARC:
            return CornerArc.blueIDToGlobal(x);
        case YELLOW_CORNER_ARC:
            return CornerArc.yellowIDToGlobal(x);
        case BLUE_GOAL:
            return Goal.blueIDToGlobal(x);
        case YELLOW_GOAL:
            return Goal.yellowIDToGlobal(x);
        default:
            System.err.println("Should not get here.. in getGlobalID()");
        }
        return -1;
    }

    
    /**
     * Pops up a dialog box at (x, y) on the screen in order to find out
     * what object the user just drew.  Note that this is not in terms of
     * image coordinates, but is instead in terms of screen coordinates.
     * 
     * Returns an array of two ints: [ObjectID, Occluded]
     * The value of choice[0] (objectID) is -1 by default.
     * The value of choice[1] is 0 (not occluded) by default.
     */

    protected int[] popupDialogBox(int x, int y) {
        // We can detect if they canceled selection
        choice = new int[]{-1, 0};
        text = new JComboBox(mode.getStrings());
        //text.addActionListener(this);
        occluded = new JCheckBox("Occluded?");
        // Add the JComboBox to the popup
        optionPane.setOptions(new Object[]{text, occluded, okButton});
        
        // Interrupts other threads until user clicks on the close button or
        // chooses a selection
        dialog = optionPane.createDialog(null, "Classify");
        dialog.setLocation(x,y);
        
        dialog.setVisible(true);
        return choice;
    }



    protected int[] popupReclassificationDialogBox(int x, int y) {
        // We can detect if they canceled selection
        choice = new int[]{-1, 0};
        text = new JComboBox(mode.getStrings());
        //text.addActionListener(this);
        occluded = new JCheckBox("Occluded?");
        
        // Add the JComboBox to the popup
        optionPane.setOptions(new Object[]{text, occluded, okButton});
        
        // Interrupts other threads until user clicks on the close button or
        // chooses a selection
        dialog = optionPane.createDialog(null, "Classify");
        dialog.setLocation(x,y);
        
        dialog.setVisible(true);
        return choice;
    }


    
    
    /**
     * When a choice has been made, hide the popup window
     */
    public void actionPerformed(ActionEvent e) {
        choice[0] = text.getSelectedIndex();
        choice[1] = occluded.isSelected() ? 1 : 0;
        dialog.setVisible(false);
        dialog.dispose();
    }
    
    
    public void setMode(ClassMode f) {
        mode = f;
        displayer.setMode(mode);
    }

	public void setImage(int i) {
		tool.getDataManager().set(i);
	}

	public TOOL getTool() {
		return tool;
	}

	public void getLastImage() {
		tool.getDataManager().last();
	}
	
	public void getNextImage() {
		tool.getDataManager().next();
	}
    

    public void notifyVisionState(VisionState v) {
        if(v == null)
	    return;
	rawImage = v.getImage();
        
	imageHeight = rawImage.getHeight();
	imageWidth = rawImage.getWidth();

        displayer.updateImage(rawImage);
    }

    public void notifyDataSet(DataSet s, Frame f) {
        notifyFrame(f);
    }
    
    public void notifyFrame(Frame f){
        if (!f.hasImage())
            return;

	rawImage = f.image();
        imageHeight = rawImage.getHeight();
        imageWidth = rawImage.getWidth();

        displayer.updateImage(rawImage);
        curImageIndex = f.index();
        
        // The very first time, we want to intercept the size of the directory
        if (images == null) {
            numImages = f.dataSet().size();
            images = f.dataSet();

            log = new ClassifierObject[numImages][ClassifierObject.NUM_OBJECTS];// ArrayList<ClassifierObject[]>();
            objectCount = new int[numImages][ClassifierObject.NUM_OBJECTS];
        }
        
        // We changed directory
        if (images != f.dataSet()) {
            // Ideally we will intercept the "load image directory" command
            // and make sure they have option to save their work first
            panel.setText("Did you just change directories?");

            
            numImages = f.dataSet().size();
            images = f.dataSet();

            log = new ClassifierObject[numImages][MAX_OBJECTS];
            objectCount = new int[numImages][ClassifierObject.NUM_OBJECTS];

        }
        else {
            displayer.drawClassifiedShapes(log[curImageIndex]);
            setText("Image " + (curImageIndex + 1) + " of " +
                f.dataSet().size());
            panel.setFrame((new Integer(curImageIndex)).toString());
        }
    }

    

    
    public void mousePressed(MouseEvent e) {}
    public void mouseReleased(MouseEvent e) {}
    public void mouseClicked(MouseEvent e) {}
    public void mouseEntered(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}
    
}
