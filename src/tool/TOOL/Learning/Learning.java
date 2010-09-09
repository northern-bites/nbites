package TOOL.Learning;

import java.lang.Math;

import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;
import java.awt.GridLayout;
import java.awt.Color;
import java.awt.image.*;
import java.awt.Cursor;
import java.awt.geom.*;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GradientPaint;
import java.awt.AlphaComposite;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelListener;
import java.awt.event.MouseWheelEvent;
import java.awt.Toolkit;
import java.awt.Point;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;


import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;


import TOOL.Data.DataListener;
import TOOL.Data.DataSet;
import TOOL.Data.Frame;
import TOOL.Data.ColorTableListener;
import TOOL.Data.File.FileSource;
//import TOOL.Misc.Pair;
import TOOL.Misc.Estimate;


import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;
import java.util.Vector;

import javax.swing.JPanel;
import javax.swing.JSplitPane;

// Image stuff
import TOOL.Image.ColorTable;
import TOOL.Image.ImageSwatch;
import TOOL.Image.ImagePanel;
import TOOL.Image.TOOLImage;
import TOOL.Image.ThresholdedImage;
import TOOL.Image.ProcessedImage;
import TOOL.Image.PixelSelectionPanel;
import TOOL.Image.ImageMarkerPanel;
import TOOL.Image.CalibrationDrawingPanel;
import TOOL.Image.DrawingPanel;


import TOOL.Image.ImageOverlay;
import TOOL.Image.ImageOverlayAction;

import TOOL.Vision.Vision;

import TOOL.Data.Classification.Keys.Builder;
import TOOL.Data.Classification.KeyFrame.GoalType;
import TOOL.Data.Classification.KeyFrame.CrossType;
import TOOL.Data.Classification.Keys;
import TOOL.Data.Classification.KeyFrame;

import java.awt.event.KeyListener;
import java.awt.event.KeyEvent;
import TOOL.GUI.IncrementalSliderParent;


import TOOL.TOOL;
import TOOL.TOOLException;



/**
 * This is the learning class. It is responsible for managing and running
 * our classification system so we can machine learn vision.
 * Right now it checks for a given KEY.KEY file in the current data directory.
 * If it finds one it reads it and displays the data along with the current
 * frame.  If it doesn't then it creates one and uses the vision system to
 * guess at what the contents of the file should be.  Then a human can
 * go through and modify them as necessary or simply approve of the contents.
 * Hitting the "human approved" button is a signal to save the new contents.
 * However, the file is only written when the "Write" button is hit.
 * This file is the main entry point and contains the big overall panel.
 * It contains three subpanels - 1) an image viewing panel, 2) a panel for
 * editting the contents of a single frame, and 3) a panel for moving
 * through the frames.
 * @author modified Eric Chown, plus whoever wrote all the code I borrow from
 * other parts of the tool
 */

public class Learning implements DataListener, MouseListener,
                                 MouseMotionListener,
                                 PropertyChangeListener
{
    protected PixelSelectionPanel selector;    // Display panel
    protected KeyPanel key;                    // editing panel
    protected LearningPanel learnPanel;        // Moving throug frames
    protected ImageOverlay overlay;            // To overlay object drawing
    protected TOOLImage rawImage;              // raw image
    protected TOOL tool;                       // parent class
    protected ColorTable colorTable;           // current color table
    protected VisionState visionState;         // processes vision

    protected JPanel main_panel;               // master panel
    protected int imageHeight, imageWidth;     // size variables

    private Frame currentFrame;                // normal frame data
    private KeyFrame current;                  // current KEY data

    private int ind;                           // index of current frame

    protected Builder keys;                    // holds whole KEY.KEY file
    protected KeyFrame.Builder newKey;         // temporary to build new item

    private JSplitPane split_pane;             // we split panel up
    private boolean split_changing;

    private boolean quietMode;                 // Used when running in batch

    private Point start, end;

    private String keyName;                    // filename of Key file
    private DataSet currentSet;                // which data set we're processing
    private int goodBall, badBall;             // ball stat variables
    private int goodCross, badCross;           // cross stat variables
    private int okCross, falseCross;
    private int goodBlue, badBlue, okBlue;     // blue goal stats
    private int goodYellow, badYellow, okYellow; // yellow goal stats
    private int goodRed, badRed;               // red robot stats
    private int goodBlueRobot, badBlueRobot;   // blue robot stats
    private int missedBall, missedCross;       // false negatives
    private int missedBlue;                    // ditto
    private int missedYellow, missedRed;       // ditto
    private int missedBlueRobot;               // ditto
    private int goodL, badL, goodT, badT;      // ditto
    private int goodCC, badCC;
    private int missedL, missedT, missedCC;    // ditto
    private int falseT, falseL, falseCC;

    private String curFrame;                   // current frame of batch job
    private int    curFrameIndex;              // index

    /** Constructor.  Makes the panels and sets up the listeners.
     * @param t     the parent TOOL class
     */

    public Learning(TOOL t){
        tool = t;
        colorTable = tool.getColorTable();

        //get all the image panels ready
        selector = new PixelSelectionPanel();
        selector.changeSettings(ImagePanel.SCALE_AUTO_BOTH);

        key = new KeyPanel(this);
        keys = Keys.newBuilder();
        setupWindowsAndListeners();

        ind = 0;
        quietMode = false;

    }

    /**  Returns the current color table.  An artifact of copying code from other
         places.  May not be needed in this class.
         @return the current color table
    */

    public ColorTable getTable() {
        return colorTable;
    }

    /**
     * Sets up all the windows and panels; installs listener onto them.
     */
    private void setupWindowsAndListeners(){
        //data listeners
        tool.getDataManager().addDataListener(this);

        // create the main panel
        main_panel = new JPanel();
        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        main_panel.setLayout(gridbag);

        // create the panel that will hold the images
        JPanel images_panel = new JPanel();
        images_panel.setLayout(new GridLayout(1, 1));
        c.weightx = 1;
        c.weighty = 5;
        c.fill = GridBagConstraints.BOTH;
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.gridheight = GridBagConstraints.RELATIVE;
        gridbag.setConstraints(images_panel, c);
        main_panel.add(images_panel);

        // within that panel, we have a split view of the two images
        split_pane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT);
        split_pane.setLeftComponent(selector);
        split_pane.setRightComponent(key);
        split_pane.setDividerLocation(.8); // initial divider location
        split_pane.setResizeWeight(.8); // how to distribute new space
        split_changing = false;
        split_pane.addPropertyChangeListener(this);
        images_panel.add(split_pane);

        // set up mouse listeners
        selector.addMouseListener(this);
        key.addMouseListener(this);
        selector.addMouseMotionListener(this);
        key.addMouseMotionListener(this);

        learnPanel = new LearningPanel(this);
        main_panel.add(learnPanel);


        main_panel.setFocusable(true);
        main_panel.requestFocusInWindow();

        learnPanel.fixButtons();
        quietMode = false;
    }

    /** @return JPanel holding all of Calibrate stuff */
    public JPanel getContentPane() {
        return main_panel;
    }

    // Methods dealing with moving through the frames

    /** Move backwards one frame.
     */
    public void getLastImage() {
        tool.getDataManager().last();
        // fix the backward skipping button
        learnPanel.fixButtons();
    }

    /** Move backwards one frame.
     */
    public void getNextImage() {
        tool.getDataManager().next();
        // fix the forward skipping button
        learnPanel.fixButtons();
    }

    /** Move forwards to a later frame.
     * @param i   the frame to move to
     */
    public void skipForward(int i) {
        tool.getDataManager().skip(i);
        // fix the forward skipping button
        learnPanel.fixButtons();
    }

    /** Move backwards to an earlier frame.
     * @param i    the frame to move to
     */
    public void skipBackward(int i) {
        tool.getDataManager().revert(i);
        // fix the backward skipping button
        learnPanel.fixButtons();
    }

    /** Tell the data manager which image to use.
     *  This method may not be necessary in this class.
     * @param i   the index of the image
     */
    public void setImage(int i) {
        tool.getDataManager().set(i);
    }

    /** Flips either from batch of off
     */
    public void toggleQuietMode() {
        quietMode = !quietMode;
    }

    /** Check if there is another image forward.  Used
     *  to set the button correctly.
     * @return true when there is an image
     */
    public boolean canGoForward() {
        return tool.getDataManager().hasElementAfter();
    }

    /** Check if there is another image backward.  Used
     *  to set the button correctly.
     * @return true when there is an image
     */
    public boolean canGoBackward() {
        return tool.getDataManager().hasElementBefore();
    }

    /** @return the parent class
     */
    public TOOL getTool() {
        return tool;
    }

    /** @return true if we have a thresholded image, else false. */
    public boolean hasImage() {
        return currentFrame != null && currentFrame.hasImage();
    }

    /** @return true if in quiet mode */
    public boolean getQuietMode() {
        return quietMode;
    }


    ////////////////////////////////////////////////////////////
    // LISTENER METHODS
    ////////////////////////////////////////////////////////////

    // MouseListener methods
    public void mouseClicked(MouseEvent e) {}

    // When mouse enters, make sure the cursor is the rectangular swatch
    public void mouseEntered(MouseEvent e) {
    }

    public void mouseExited(MouseEvent e) {    }

    public void mousePressed(MouseEvent e) {
        start = e.getPoint();
    }

    public void mouseReleased(MouseEvent e) {

    }

    // MouseMotionListener methods
    public void mouseDragged(MouseEvent e) {
        end = e.getPoint();
        mouseMoved(e);
    }


    // PropertyChangeListener method
    public void propertyChange(PropertyChangeEvent e) {
        if (e.getPropertyName().equals(JSplitPane.DIVIDER_LOCATION_PROPERTY)
            && !split_changing) {
            split_changing = true;
            split_pane.setDividerLocation(.5);
            split_changing = false;
        }

        else if (e.getPropertyName().equals(ImagePanel.X_SCALE_CHANGE)) {
        }
    }

    public void mouseMoved(MouseEvent e) {
        int x = e.getX();
        int y = e.getY();
    }

    //mouseWheelListener Methods
    public void mouseWheelMoved(MouseWheelEvent e){

    }

    //dataListener Methods

    /** A new data set has been selected.  The big thing here
     * is to check for our key file.  If it doesn't exist then
     * we need to create a temporary version of one in case we
     * decide to edit it.  So we either read the data into our
     * data structure or we fill our data structure with default
     * values for every frame.  The file stuff uses Google's
     * protocol buffers system.
     * @param s     the Dataset that was selected
     * @param f     the current frame within the dataset
     */
    public void notifyDataSet(DataSet s, Frame f) {
        boolean keyExists = true;
        currentSet = s;
        keys = Keys.newBuilder();
        keyName = s.path()+"KEY.KEY";
        // See if the key exists.
        try {
            FileInputStream input = new FileInputStream(keyName);
            keys.mergeFrom(input);
            input.close();
        } catch (FileNotFoundException e) {
            keyExists = false;
            System.out.println(keyName + ": not found.  Creating a new file.");
        } catch (java.io.IOException e) {
            System.out.println("Problems with key file");
        }
        if (!keyExists) {
            for (int i = 0; i < s.size(); i++) {
                // make a new key for the file and add it
                KeyFrame next =
                    KeyFrame.newBuilder()
                    .setHumanChecked(false)
                    .setBall(false)
                    .setBlueGoal(GoalType.NO_POST)
                    .setYellowGoal(GoalType.NO_POST)
                    .setCross(CrossType.NO_CROSS)
                    .setRedRobots(0)
                    .setBlueRobots(0)
                    .setTCorners(0)
                    .setLCorners(0)
                    .setCcCorners(0)
                    .build();
                keys.addFrame(next);
            }
        }

        notifyFrame(f);
    }


    //to do: clean up this code - Octavian
    /** A new frame has been selected.  We need to update all of our
     * information - vision, our key, etc.
     * @param f    the frame
     */
    public void notifyFrame(Frame f) {
        if (!quietMode) {
            currentFrame = f;
            if (!f.hasImage())
                return;

            // Load VisionState for new frame
            newFrameForVisionState(f);

            rawImage = visionState.getImage();

            colorTable = visionState.getColorTable();

            // Since we now handle different sized frames, it's possible to
            // switch between modes, changing the image's size without updating
            // the overlay.  This will catch that
            if(overlay == null || overlay.getWidth() != rawImage.getWidth()) {
                overlay = new ImageOverlay(rawImage.getWidth(),rawImage.getHeight());
            }
            imageHeight = rawImage.getHeight();
            imageWidth = rawImage.getWidth();

            overlay.generateNewEdgeImage(rawImage);
            selector.updateImage(rawImage);
            visionState.update(false, f);
            visionState.updateObjects();

            // retrieve the frame information
            ind = f.index();
            current = keys.getFrame(ind);
            // setup the buttons on the key panel to reflect the contents of the file
            key.setHumanStatus(current.getHumanChecked());
            if (current.getHumanChecked()) {
                key.setHumanStatus(true);
                key.setBallStatus(current.getBall());
                key.setBlueGoalStatus(current.getBlueGoal());
                key.setYellowGoalStatus(current.getYellowGoal());
                key.setCrossStatus(current.getCross());
                key.setRedRobotStatus(current.getRedRobots());
                key.setBlueRobotStatus(current.getBlueRobots());
                key.setTCornerStatus(current.getTCorners());
                key.setLCornerStatus(current.getLCorners());
                key.setCcCornerStatus(current.getCcCorners());
                newKey =
                    KeyFrame.newBuilder()
                    .setHumanChecked(current.getHumanChecked())
                    .setBall(current.getBall())
                    .setBlueGoal(current.getBlueGoal())
                    .setYellowGoal(current.getYellowGoal())
                    .setCross(current.getCross())
                    .setRedRobots(current.getRedRobots())
                    .setBlueRobots(current.getBlueRobots())
                    .setTCorners(current.getTCorners())
                    .setLCorners(current.getLCorners())
                    .setCcCorners(current.getCcCorners());
            } else {
                // set up based upon vision data
                key.setHumanStatus(false);
                key.setBallStatus(getBall());
                key.setBlueGoalStatus(getBlueGoal());
                key.setYellowGoalStatus(getYellowGoal());
                key.setCrossStatus(getCross());
                key.setRedRobotStatus(getRedRobots());
                key.setBlueRobotStatus(getBlueRobots());
                key.setTCornerStatus(getTCorners());
                key.setLCornerStatus(getLCorners());
                key.setCcCornerStatus(getCcCorners());
                newKey =
                    KeyFrame.newBuilder()
                    .setHumanChecked(current.getHumanChecked())
                    .setBall(getBall())
                    .setBlueGoal(getBlueGoal())
                    .setYellowGoal(getYellowGoal())
                    .setCross(getCross())
                    .setRedRobots(getRedRobots())
                    .setBlueRobots(getBlueRobots())
                    .setTCorners(getTCorners())
                    .setLCorners(getLCorners())
                    .setCcCorners(getCcCorners());
            }
            // write out the vision data in the GUI
            key.setBall(getBallString());
            key.setBlueGoal(getBlueGoalString());
            key.setYellowGoal(getYellowGoalString());
            key.setCross(getCrossString());
            key.setRedRobot(getRedRobotString());
            key.setBlueRobot(getBlueRobotString());
            key.setTCorner(getTCornerString());
            key.setLCorner(getLCornerString());
            key.setCcCorner(getCcCornerString());
            //learnPanel.setOverlays();
            // set up the builder in case we decide to edit

            selector.setOverlayImage(visionState.getThreshOverlay());
            selector.repaint();

            // They loaded something so make sure our buttons reflect the
            // active state; e.g. that our undo stack and redo stack are
            // empty.
            learnPanel.fixButtons();
            // 0 based indexing.
            learnPanel.setText("Image " + (f.index()) + " of " +
                               (f.dataSet().size() - 1) +
                               " -  processed in " + visionState.getProcessTime() +
                               " micro secs");
        }
    }


    /* We often get a series of frames with the same data.  In this case the
       personn editing is saying that this frame is basically the same as the
       last.  So get its info and substitute it for the vision data.
    */
    public void useLast() {
        int ind = currentFrame.index();
        current = keys.getFrame(ind - 1);
        // setup the buttons on the key panel to reflect the contents of the file
        key.setHumanStatus(false);
        key.setBallStatus(current.getBall());
        key.setBlueGoalStatus(current.getBlueGoal());
        key.setYellowGoalStatus(current.getYellowGoal());
        key.setCrossStatus(current.getCross());
        key.setRedRobotStatus(current.getRedRobots());
        key.setBlueRobotStatus(current.getBlueRobots());
        key.setTCornerStatus(current.getTCorners());
        key.setLCornerStatus(current.getLCorners());
        key.setCcCornerStatus(current.getCcCorners());
        newKey =
            KeyFrame.newBuilder()
            .setHumanChecked(current.getHumanChecked())
            .setBall(current.getBall())
            .setBlueGoal(current.getBlueGoal())
            .setYellowGoal(current.getYellowGoal())
            .setCross(current.getCross())
            .setRedRobots(current.getRedRobots())
            .setBlueRobots(current.getBlueRobots())
            .setTCorners(current.getTCorners())
            .setCcCorners(current.getCcCorners());
    }

    /** Run a "batch" learning job.  We're going to bootstrap this.
        Our first goal is simply to run all the frames in the current
        directory and collect statistics on the ones that are marked
        for human approval.
    */
    public void runBatch () {
        System.out.println("Running a batch job");
        initStats();
        quietMode = true;
        int framesProcessed = 0;
        long t = System.currentTimeMillis();
        curFrame = currentSet.path();
        for (Frame d : currentSet) {
            try {
                currentSet.load(d.index());
            } catch (TOOLException e) {
                System.out.println("Couldn't load frame");
            }
            current = keys.getFrame(d.index());
            curFrameIndex = d.index();
            if (current.getHumanChecked()) {
                // we have good data, so let's process the frame
                newFrameForVisionState(d);
                visionState.update(false, d);
                visionState.updateObjects();
                updateBallStats();
                updateGoalStats();
                updateCrossStats();
                updateRobotStats();
                updateCornerStats();
                framesProcessed++;
            }
        }
        t = System.currentTimeMillis() - t;
        quietMode = false;
        printStats(framesProcessed, t);
    }

    /**
       Gets the directory one step up from current directory and runs a recursive batch
       operation on the sets contained within it.
    */
    public void runRecursiveBatchOnCurrentDir()
    {
        String topPath = currentSet.path();
        // We need to get rid of the current directory
        int end = topPath.length() - 2;
        for ( ; end > -1 && !topPath.substring(end, end+1).equals(System.getProperty("file.separator"));
              end--) {}
        if (end > -1) {
            topPath = topPath.substring(0, end+1);
            runRecursiveBatch(topPath);
        } else {
            System.out.println("No possible path");
            return;
        }
    }

    /** Run a recursive batch job.
        Obviously this is not for the faint of heart as it could take a very
        long time depending on the amount of data contained.
    */
    public void runRecursiveBatch(String topPath) {
        System.out.println("Running recursive batch job");
        initStats();
        quietMode = true;
        int framesProcessed = 0;
        long t = System.currentTimeMillis();
        // topPath should now contain the parent directory pathname
        // now we need to start retrieving all of the data sets that contain it
        FileSource source = (FileSource)(tool.getSourceManager().addSource(topPath));
        if (source == null){
            System.out.println("Recursive batch top directory " + topPath + " was invalid.");
            return;
        }
        List<DataSet> dataList = source.getDataSets();
        for (DataSet d : dataList) {
            // we have a target data set
            curFrame = d.path();
            String keyName = d.path()+"KEY.KEY";
            // See if the key exists.
            try {
                FileInputStream input = new FileInputStream(keyName);
                keys.clear();
                keys.mergeFrom(input);
                input.close();
                for (Frame f : d) {
                    try {
                        f.load();
                    } catch (TOOLException e) {
                        System.out.println("Couldn't load frame");
                    }
                    current = keys.getFrame(f.index());
                    curFrameIndex = f.index();
                    if (shouldProcessFrame(current)) {
                        // we have good data, so let's process the frame
                        newFrameForVisionState(f);
                        visionState.update(true, f);
                        visionState.updateObjects();
                        updateBallStats();
                        updateGoalStats();
                        updateCrossStats();
                        updateRobotStats();
                        updateCornerStats();
                        framesProcessed++;
                    }
                    try {
                        f.unload();
                    } catch (TOOLException e) {
                        System.out.println("Problem unloading frame");
                    }
                }
            } catch (FileNotFoundException e) {
                // key file doesn't exist, so skip it
            } catch (java.io.IOException e) {
                // something went wrong, so keep going
            }
        }
        t = System.currentTimeMillis() - t;
        quietMode = false;
        System.out.println("Processed " + topPath);
        printStats(framesProcessed, t);
    }


    /** Learn a new color table. Start by gathering
        current path and try running batch on every data set it contains.
        While doing that collect a bunch of stats on each of the colors
        we care about.
        Obviously this is not for the faint of heart as it could take a very
        long time depending on the amount of data contained.
    */
    public void runRecursiveBatchLearning() {
        initStats();
        System.out.println("Starting color learning");
        quietMode = true;
        int framesProcessed = 0;
        long t = System.currentTimeMillis();
        String topPath = currentSet.path();
        boolean screen = false;
        int balls = 0, yposts = 0, bposts = 0, crosses = 0, brobots = 0, rrobots = 0, greens = 0;
        // We need to get rid of the current directory
        int end = topPath.length() - 2;
        List<DataSet> dataList;
        for ( ; end > -1 && !topPath.substring(end, end+1).equals(System.getProperty("file.separator"));
              end--) {}
        if (end > -1) {
            visionState.initStats();
            topPath = topPath.substring(0, end+1);
            // topPath should now contain the parent directory pathname
            // now we need to start retrieving all of the data sets that contain it
            FileSource source = (FileSource)(tool.getSourceManager().activeSource());
            dataList = source.getDataSets();
            for (DataSet d : dataList) {
                if (d.path().startsWith(topPath)) {
                    // we have a target data set
                    curFrame = d.path();
                    String keyName = d.path()+"KEY.KEY";
                    // See if the key exists.
                    try {
                        FileInputStream input = new FileInputStream(keyName);
                        keys.clear();
                        keys.mergeFrom(input);
                        input.close();
                        for (Frame f : d) {
                            try {
                                f.load();
                            } catch (TOOLException e) {
                                System.out.println("Couldn't load frame");
                            }
                            current = keys.getFrame(f.index());
                            curFrameIndex = f.index();
                            if (current.getHumanChecked()) {
                                // we have good data, so let's process the frame
                                newFrameForVisionState(f);
                                // we need to figure out what objects are in the frame
                                boolean or, yell, bl, wh, re, na;
                                or = current.getBall();
                                if (or) {
                                    balls++;
                                }
                                switch (current.getYellowGoal()) {
                                case NO_POST: yell = false;
                                    break;
                                default:
                                    yell = true;
                                    yposts++;
                                }
                                switch (current.getBlueGoal()) {
                                case NO_POST: bl = false;
                                    break;
                                default:
                                    bl = true;
                                    bposts++;
                                }
                                switch (current.getCross()) {
                                case NO_CROSS: wh = false;
                                    break;
                                default:
                                    wh = true;
                                    crosses++;
                                }
                                re = current.getRedRobots() > 0;
                                na = current.getBlueRobots() > 0;
                                if (re) rrobots++;
                                if (na) brobots++;
                                //if (!re && !na) {
                                greens += visionState.learnGreenWhite();
                                //}
                                visionState.updateStats(or, yell, bl, wh, re, na);
                                framesProcessed++;
                            }
                            try {
                                f.unload();
                            } catch (TOOLException e) {
                                System.out.println("Problem unloading frame");
                            }
                        }
                    } catch (FileNotFoundException e) {
                        // key file doesn't exist, so skip it
                    } catch (java.io.IOException e) {
                        // something went wrong, so keep going
                    }
                }
            }
            System.out.println("Processed "+framesProcessed+" with "+balls);
            visionState.updateGreenWhite(greens);
            // now get the general color ranges of green and white
            int yMin = 500, yMax = -1, uMin = 500, uMax = -1, vMin = 500, vMax = -1;
            // now do the same thing, but process green and white slightly differently
            for (int i = 0; i < 128; i++) {
                for (int j = 0; j < 128; j++) {
                    for (int k = 0; k < 128; k++) {
                        byte color = colorTable.getRawColor(i, j, k);
                        if (color == Vision.GREEN) {
                            yMin = Math.min(yMin, i);
                            yMax = Math.max(yMax, i);
                            uMin = Math.min(uMin, j);
                            uMax = Math.max(uMax, j);
                            vMin = Math.min(vMin, k);
                            vMax = Math.max(vMax, k);
                        }
                    }
                }
            }
            boolean secondPass = true;
            System.out.println("Values "+yMin+" "+yMax+" "+uMin+" "+uMax+" "+vMin+" "+vMax);
            int updated = 0;
            visionState.initStats();
            for (DataSet d : dataList) {
                if (d.path().startsWith(topPath) && secondPass) {
                    // we have a target data set
                    curFrame = d.path();
                    String keyName = d.path()+"KEY.KEY";
                    // See if the key exists.
                    try {
                        FileInputStream input = new FileInputStream(keyName);
                        keys.clear();
                        keys.mergeFrom(input);
                        input.close();
                        for (Frame f : d) {
                            try {
                                f.load();
                            } catch (TOOLException e) {
                                System.out.println("Couldn't load frame");
                            }
                            current = keys.getFrame(f.index());
                            curFrameIndex = f.index();
                            if (current.getHumanChecked()) {
                                // we have good data, so let's process the frame
                                newFrameForVisionState(f);
                                // we need to figure out what objects are in the frame
                                boolean or, yell, bl, wh, re, na;
                                or = current.getBall();
                                switch (current.getYellowGoal()) {
                                case NO_POST: yell = false;
                                    break;
                                default:
                                    yell = true;
                                }
                                switch (current.getBlueGoal()) {
                                case NO_POST: bl = false;
                                    break;
                                default:
                                    bl = true;
                                }
                                switch (current.getCross()) {
                                case NO_CROSS: wh = false;
                                    break;
                                default:
                                    wh = true;
                                }
                                re = current.getRedRobots() > 0;
                                na = current.getBlueRobots() > 0;
                                int temp = 0;
                                //if (!re && !na && !or) {
                                temp = visionState.moreLearnGreenWhite(yMin, yMax, uMin, uMax,
                                                                       vMin, vMax);
                                //}
                                visionState.updateStats(or, yell, bl, wh, re, na);
                                if (temp > 0) {
                                    System.out.println("Updated "+temp+" valuesin frame "+curFrame+" "+curFrameIndex);
                                    updated+= temp;
                                }
                            }
                            try {
                                f.unload();
                            } catch (TOOLException e) {
                                System.out.println("Problem unloading frame");
                            }
                        }
                    } catch (FileNotFoundException e) {
                        // key file doesn't exist, so skip it
                    } catch (java.io.IOException e) {
                        // something went wrong, so keep going
                    }
                }
            }
            visionState.updateGreenWhite(0);
            System.out.println("UPdated "+updated);
            t = System.currentTimeMillis() - t;
            quietMode = false;
            visionState.printStats(framesProcessed, balls, yposts, bposts, crosses, rrobots, brobots, false);
            // now let's see if we can improve on that
            /*for (DataSet d : dataList) {
              if (d.path().startsWith(topPath)) {
              // we have a target data set
              curFrame = d.path();
              String keyName = d.path()+"KEY.KEY";
              // See if the key exists.
              try {
              FileInputStream input = new FileInputStream(keyName);
              keys.clear();
              keys.mergeFrom(input);
              input.close();
              for (Frame f : d) {
              try {
              f.load();
              } catch (TOOLException e) {
              System.out.println("Couldn't load frame");
              }
              current = keys.getFrame(f.index());
              curFrameIndex = f.index();
              if (current.getHumanChecked()) {
              // we have good data, so let's process the frame
              visionState.newFrame(f, tool.getColorTable());
              // we need to figure out what objects are in the frame
              boolean or, yell, bl, wh, re, na;
              or = current.getBall();
              switch (current.getYellowGoal()) {
              case NO_POST: yell = false;
              break;
              default:
              yell = true;
              }
              switch (current.getBlueGoal()) {
              case NO_POST: bl = false;
              break;
              default:
              bl = true;
              }
              switch (current.getCross()) {
              case NO_CROSS: wh = false;
              break;
              default:
              wh = true;
              }
              re = current.getRedRobots() > 0;
              na = current.getBlueRobots() > 0;
              if (re) rrobots++;
              if (na) brobots++;
              visionState.reviseStats(or, yell, bl, wh, re, na);
              }
              try {
              f.unload();
              } catch (TOOLException e) {
              System.out.println("Problem unloading frame");
              }
              }
              } catch (FileNotFoundException e) {
              // key file doesn't exist, so skip it
              } catch (java.io.IOException e) {
              // something went wrong, so keep going
              }
              }*/
            System.out.println("Updating color table");
            //visionState.printStats(framesProcessed, balls, yposts, bposts, crosses, rrobots, brobots, true);
            System.out.println("Revision finished");
        }
    }


    /** Initialize all of our statistics variables
     */
    public void initStats() {
        goodBall = 0; badBall = 0; goodCross = 0; badCross = 0; falseCross = 0; okCross = 0;
        goodBlue = 0; badBlue = 0; goodYellow = 0; badYellow = 0; okBlue=0; okYellow=0;
        goodRed = 0; badRed = 0; goodBlueRobot = 0; badBlueRobot = 0;
        missedBall = 0; missedCross = 0; missedBlue = 0; missedYellow = 0;
        missedRed = 0; missedBlueRobot = 0; goodT = 0; goodL = 0; goodCC = 0;
        badT = 0; badL = 0; badCC = 0; missedT = 0; missedL = 0; missedCC = 0;
        falseT = 0; falseL = 0; falseCC = 0;
    }

    /** Print out statistics.
     */
    public void printStats(int processed, long t) {
        System.out.println("Processed "+processed+" frames in "+(t / 1000)+" seconds.");
        System.out.println("Ball Statistics:         Good : "+goodBall+"    False positives: "+
                           badBall+" Missed: "+missedBall);
        System.out.println("Blue Goal Statistics:    Good: "+goodBlue+"     Fair: "+
                           okBlue+" false positives: "+badBlue+" missed: "+missedBlue);
        System.out.println("Yellow Goal Statistics:  Good: "+goodYellow+"   Fair: "+
                           okYellow+" false positives: "+badYellow+" missed: "+missedYellow);
        System.out.println("Cross Statistics:        Good: "+goodCross+" OK: "+okCross+
                           "    False positives: "+falseCross+" badID: "+
                           badCross+" missed: "+missedCross);
        System.out.println("Corner Statistics:"+
                           "  GoodT: "+goodT+" GoodL: "+goodL+" GoodCC: "+goodCC+
                           "\n\tFalse Ts: "+ falseT+" False Ls: "+falseL+" False CCs: "+falseCC+
                           "\n\tMissed Ts: "+missedT+" Missed Ls: "+missedL+" Missed CCs: "+missedCC);
    }

    /** Compare our key file against vision and update stats accordingly
     */
    public void updateCornerStats() {
        int ells = current.getLCorners();
        int tees = current.getTCorners();
        int cees = current.getCcCorners();
        int ellsV = visionState.getLCornersVision();
        int teesV = visionState.getTCornersVision();
        int ceesV = visionState.getCcCornersVision();
        if (ells > ellsV) {
            missedL += ells - ellsV;
            goodL += ellsV;
            printMissedLCornerMessage();
        } else if (ellsV > ells) {
            falseL += ellsV - ells;
            printFalseLCornerMessage();
            goodL += ells;
        } else if (ells > 0) {
            goodL+= ells;
        }

        if (tees > teesV) {
            missedT += tees - teesV;
            goodT += teesV;
            printMissedTCornerMessage();
        } else if (teesV > tees) {
            falseT += teesV - tees;
            goodT += tees;
            printFalseTCornerMessage();
        } else if (tees > 0) {
            goodT += tees;
        }

        if (cees > ceesV) {
            missedCC += cees - ceesV;
            goodCC += ceesV;
            printMissedCcCornerMessage();
        } else if (ceesV > cees) {
            falseCC += ceesV - cees;
            goodCC += cees;
            printFalseCcCornerMessage();
        } else if (cees > 0) {
            goodCC += cees;
        }
    }

    /**
     * Print respective messages for missed/false corners in frames
     */
    public void printFalseLCornerMessage()
    {
        if (learnPanel.getFalseLCorners())
            System.out.println("False LCorner in "+curFrame+" frame "+curFrameIndex);
    };
    public void printFalseTCornerMessage()
    {
        if (learnPanel.getFalseTCorners())
            System.out.println("False TCorner in "+curFrame+" frame "+curFrameIndex);
    };
    public void printFalseCcCornerMessage()
    {
        if (learnPanel.getFalseCcCorners())
            System.out.println("False CcCorner in "+curFrame+" frame "+curFrameIndex);
    };
    public void printMissedLCornerMessage()
    {
        if (learnPanel.getMissedLCorners())
            System.out.println("Missed LCorner in "+curFrame+" frame "+curFrameIndex);
    };
    public void printMissedTCornerMessage()
    {
        if (learnPanel.getMissedTCorners())
            System.out.println("Missed TCorner in "+curFrame+" frame "+curFrameIndex);
    };
    public void printMissedCcCornerMessage()
    {
        if (learnPanel.getMissedCcCorners())
            System.out.println("Missed CcCorner in "+curFrame+" frame "+curFrameIndex);
    };

    /** Compare our key file against vision and update stats accordingly
     */
    public void updateBallStats() {
        if (current.getBall()) {
            if (visionState.getBallVision()) {
                goodBall++;
            }else {
                missedBall++;
                if (learnPanel.getMissedBalls())
                    System.out.println("Missed ball in "+curFrame+" frame "+curFrameIndex);
            }
        } else if (visionState.getBallVision()) {
            badBall++;
            if (learnPanel.getFalseBalls())
                System.out.println("False ball in "+curFrame+" frame "+curFrameIndex);
        }
    }

    /** Compare our key file against vision and update stats accordingly
     */
    public void updateGoalStats() {
        switch (current.getBlueGoal()) {
        case NO_POST:
            switch (visionState.getBlueGoalVision()) {
            case NO_POST: break;
            case LEFT:
            case RIGHT:
            case UNSURE:
                if (learnPanel.getFalseGoals())
                    System.out.println("False Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                badBlue++; break;
            case BOTH: badBlue += 2;
                if (learnPanel.getFalseGoals())
                    System.out.println("Two False Goal Posts in "+curFrame+
                                       " frame "+curFrameIndex);
            }
            break;
        case LEFT:
            switch (visionState.getBlueGoalVision()) {
            case NO_POST: missedBlue++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed a Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case LEFT: goodBlue++; break;
            case RIGHT: badBlue++;
                if (learnPanel.getFalseGoals() && learnPanel.getMissedGoals())
                    System.out.println("Bad Goal Post ID in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case UNSURE: okBlue++; break;
            case BOTH: badBlue++;
                if (learnPanel.getFalseGoals())
                    System.out.println("Extra Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            }
            break;
        case RIGHT:
            switch (visionState.getBlueGoalVision()) {
            case NO_POST: missedBlue++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed a Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case LEFT: badBlue++;
                if (learnPanel.getFalseGoals() && learnPanel.getMissedGoals())
                    System.out.println("Bad Goal Post ID in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case RIGHT: goodBlue++; break;
            case UNSURE: okBlue++; break;
            case BOTH: badBlue++;
                if (learnPanel.getFalseGoals())
                    System.out.println("Extra Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            }
            break;
        case BOTH:
            switch (visionState.getBlueGoalVision()) {
            case NO_POST: missedBlue += 2;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed two goals in"+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case LEFT: missedBlue++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed a Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case RIGHT: missedBlue++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed a Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case UNSURE: missedBlue++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed a Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                okBlue++; missedBlue++;
                break;
            case BOTH: goodBlue+= 2; break;
            }
            break;
        case UNSURE:
            switch (visionState.getBlueGoalVision()) {
            case NO_POST: missedBlue++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed a Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case LEFT: okBlue++; break;
            case RIGHT: okBlue++; break;
            case UNSURE: goodBlue++; break;
            case BOTH: badBlue++; okBlue++;
                if (learnPanel.getFalseGoals())
                    System.out.println("Extra Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            }
            break;
        }
        switch (current.getYellowGoal()) {
        case NO_POST:
            switch (visionState.getYellowGoalVision()) {
            case NO_POST: break;
            case LEFT:
            case RIGHT:
            case UNSURE:
                if (learnPanel.getFalseGoals())
                    System.out.println("False Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                badYellow++; break;
            case BOTH:
                if (learnPanel.getFalseGoals())
                    System.out.println("Two False Goal Posts in "+curFrame+
                                       " frame "+curFrameIndex);
                badYellow += 2;
            }
            break;
        case LEFT:
            switch (visionState.getYellowGoalVision()) {
            case NO_POST: missedYellow++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed a Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case LEFT: goodYellow++; break;
            case RIGHT: badYellow++;
                if (learnPanel.getFalseGoals() && learnPanel.getMissedGoals())
                    System.out.println("Bad Goal Post ID in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case UNSURE: okYellow++; break;
            case BOTH: badYellow++;
                if (learnPanel.getFalseGoals())
                    System.out.println("Extra Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            }
            break;
        case RIGHT:
            switch (visionState.getYellowGoalVision()) {
            case NO_POST: missedYellow++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed a Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case LEFT: badYellow++;
                if (learnPanel.getFalseGoals() && learnPanel.getMissedGoals())
                    System.out.println("Bad Goal Post ID in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case RIGHT: goodYellow++; break;
            case UNSURE: okYellow++; break;
            case BOTH: badYellow++;
                if (learnPanel.getFalseGoals())
                    System.out.println("Extra Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            }
            break;
        case BOTH:
            switch (visionState.getYellowGoalVision()) {
            case NO_POST: missedYellow += 2;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed two Goal Posts in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case LEFT: missedYellow++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed a Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case RIGHT: missedYellow++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed a Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case UNSURE: missedYellow++; okYellow++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed a Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case BOTH: goodYellow+= 2; break;
            }
            break;
        case UNSURE:
            switch (visionState.getYellowGoalVision()) {
            case NO_POST: missedYellow++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed a Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case LEFT: okYellow++; break;
            case RIGHT: okYellow++; break;
            case UNSURE: goodYellow++; break;
            case BOTH: badYellow++; okYellow++;
                if (learnPanel.getMissedGoals())
                    System.out.println("Missed Goal Post in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            }
            break;
        }
    }

    /** Check our key file against our vision system and collect stats.
     */
    public void updateCrossStats() {
        switch (current.getCross()) {
        case NO_CROSS:
            switch (visionState.getCrossVision()) {
            case NO_CROSS: break;
            case BLUE:
            case YELLOW:
            case UNKNOWN:
                falseCross++;
                if (learnPanel.getFalseCrosses())
                    System.out.println("False Cross in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case DOUBLE_CROSS: falseCross+=2;
                if (learnPanel.getFalseCrosses())
                    System.out.println("Two False Crosses in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            }
            break;
        case BLUE:
            switch (visionState.getCrossVision()) {
            case NO_CROSS: missedCross++;
                if (learnPanel.getMissedCrosses())
                    System.out.println("Missed Cross in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case BLUE: goodCross++;
                break;
            case YELLOW: badCross++;
                if (learnPanel.getFalseCrosses())
                    System.out.println("Bad Cross in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case UNKNOWN:
                okCross++; break;
            case DOUBLE_CROSS: badCross++; goodCross++;
                if (learnPanel.getFalseCrosses())
                    System.out.println("False Cross in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            }
            break;
        case YELLOW:
            switch (visionState.getCrossVision()) {
            case NO_CROSS: missedCross++;
                if (learnPanel.getMissedCrosses())
                    System.out.println("Missed Cross in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case BLUE: badCross++;
                if (learnPanel.getFalseCrosses())
                    System.out.println("Bad Cross in "+curFrame+
                                       " frame "+curFrameIndex);

                break;
            case YELLOW: goodCross++; break;
            case UNKNOWN:
                okCross++; break;
            case DOUBLE_CROSS: badCross++; goodCross++;
                if (learnPanel.getFalseCrosses())
                    System.out.println("False Cross in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            }
            break;
        case UNKNOWN:
            switch (visionState.getCrossVision()) {
            case NO_CROSS: missedCross++;
                if (learnPanel.getMissedCrosses())
                    System.out.println("Missed Cross in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case BLUE:
            case YELLOW:
                okCross++; break;
            case UNKNOWN:
                goodCross++; break;
            case DOUBLE_CROSS: badCross++; goodCross++;
                if (learnPanel.getFalseCrosses())
                    System.out.println("False Cross in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            }
            break;
        case DOUBLE_CROSS:
            switch (visionState.getCrossVision()) {
            case NO_CROSS: missedCross+=2;
                if (learnPanel.getMissedCrosses())
                    System.out.println("Two Missed Crosses in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case BLUE:
            case YELLOW:
            case UNKNOWN:
                missedCross++;
                goodCross++;
                if (learnPanel.getMissedCrosses())
                    System.out.println("Missed Cross in "+curFrame+
                                       " frame "+curFrameIndex);
                break;
            case DOUBLE_CROSS: goodCross+=2; break;
            }
        }
    }

    /** Should we process this frame? Does it fit the requirements
     * set by the panel buttons
     *
     * @return boolean If the current frame has the required objects in it
     */
    public boolean shouldProcessFrame(KeyFrame current){
        final boolean screen = ( learnPanel.getOnlyBalls()   ||
                                 learnPanel.getOnlyGoals()   ||
                                 learnPanel.getOnlyCrosses()   ||
                                 learnPanel.getOnlyBots()      ||
                                 learnPanel.getOnlyCcCorners() ||
                                 learnPanel.getOnlyTCorners()  ||
                                 learnPanel.getOnlyLCorners());

        return current.getHumanChecked() &&
            (!screen ||
             (learnPanel.getOnlyBalls() && current.getBall()) ||

             (learnPanel.getOnlyGoals() &&
              (current.getBlueGoal().getNumber() != 0 ||
               current.getYellowGoal().getNumber() != 0)) ||

             (learnPanel.getOnlyCrosses() && current.getCross().getNumber() != 0) ||

             (learnPanel.getOnlyBots() && (current.getRedRobots() != 0 ||
                                           current.getBlueRobots() != 0)) ||

             (learnPanel.getOnlyLCorners() && current.getLCorners() != 0) ||
             (learnPanel.getOnlyTCorners() && current.getTCorners() != 0) ||
             (learnPanel.getOnlyCcCorners() && current.getCcCorners() != 0)

             );
    }

    /** Someday we'll use this to collect robot stats.  But first we need to be
        able to recognize them!
    */
    public void updateRobotStats() {
    }


    /** Returns the overlay containing image edges.  Can probably be dumped.
     * @ return the overlay
     */
    public ImageOverlay getEdgeOverlay() {
        return overlay;
    }

    /** Returns the object the runs vision processing for us.
     * @return link to vision
     */
    public VisionState getVisionState() {
        return visionState;
    }

    /** Used to set the information in the Key file.
     * @param hasBall    whether or not the frame has a ball
     */
    public void setBall(boolean hasBall) {
        if (newKey != null) {
            newKey.setBall(hasBall);
        }
    }


    /** When the "Write" button is pressed this is executed.
     *  It writes the contents of our data structure to the KEY.KEY
     *  file.
     */
    public void writeData() {
        // Write the new key file back to disk.
        try {
            FileOutputStream output = new FileOutputStream(keyName);
            keys.build().writeTo(output);
            output.close();
            newKey = null;
        } catch (java.io.IOException e) {
            System.out.println("Problems with key file");
        }
        // now as odd as this may seem, reread the data!
        // this is because we have already used this builder
        keys = Keys.newBuilder();
        try {
            FileInputStream input = new FileInputStream(keyName);
            keys.mergeFrom(input);
            input.close();
        } catch (FileNotFoundException e) {
            System.out.println(keyName + ": not found.  Creating a new file.");
        } catch (java.io.IOException e) {
            System.out.println("Problems with key file");
        }

    }

    /** Used to set the information in the Key file.
     * @param hasHuman    whether or not the frame was human approved
     */
    public void setHuman(boolean hasHuman) {
        if (newKey != null) {
            newKey.setHumanChecked(true);
            keys.setFrame(ind , newKey);
            newKey =
                KeyFrame.newBuilder()
                .setHumanChecked(current.getHumanChecked())
                .setBall(current.getBall())
                .setBlueGoal(current.getBlueGoal())
                .setYellowGoal(current.getYellowGoal())
                .setCross(current.getCross())
                .setRedRobots(current.getRedRobots())
                .setBlueRobots(current.getBlueRobots())
                .setLCorners(current.getLCorners())
                .setTCorners(current.getTCorners());
        }
        key.setHumanStatus(hasHuman);
    }

    /** Used to set the information in the Key file.
     * @param which    whether or not the frame has a cross and what type
     */
    public void setCross(CrossType which) {
        if (newKey != null)
            newKey.setCross(which);
    }

    /** Used to set the information in the Key file.
     * @param which    whether or not the frame has a yellow goal and what type
     */
    public void setYellowGoal(GoalType which) {
        if (newKey != null)
            newKey.setYellowGoal(which);
    }

    /** Used to set the information in the Key file.
     * @param which    whether or not the frame has a blue goal and what type
     */
    public void setBlueGoal(GoalType which) {
        if (newKey != null)
            newKey.setBlueGoal(which);
    }

    /** Used to set the information in the Key file.
     * @param howMany    the number of red robots
     */
    public void setRedRobot(int howMany) {
        if (newKey != null)
            newKey.setRedRobots(howMany);
    }

    /** Used to set the information in the Key file.
     * @param howMany    the number of blue robots
     */
    public void setBlueRobot(int howMany) {
        if (newKey != null)
            newKey.setBlueRobots(howMany);
    }

    /** Used to set the information in the Key file.
     * @param ells       how many L corners there are in the frame
     */
    public void setLCorners(int ells) {
        if (newKey != null)
            newKey.setLCorners(ells);
    }

    /** Used to set the information in the Key file.
     * @param tees    how many T corners there are in the frame
     */
    public void setTCorners(int tees) {
        if (newKey != null)
            newKey.setTCorners(tees);
    }

    /** Used to set the information in the Key file.
     * @param tees    how many CC corners there are in the frame
     */
    public void setCcCorners(int cees) {
        if (newKey != null)
            newKey.setCcCorners(cees);
    }

    /** Used to get information from vision.
     * @return    whether there is a ball or not
     */
    public boolean getBall() {
        if (visionState == null) return false;
        return visionState.getBallVision();
    }

    /** Used to get information from vision.
     * @return    status of blue goal
     */
    public GoalType getBlueGoal() {
        if (visionState == null) return GoalType.NO_POST;
        return visionState.getBlueGoalVision();
    }

    /** Used to get information from vision.
     * @return    status of yellow goal
     */
    public GoalType getYellowGoal() {
        if (visionState == null) return GoalType.NO_POST;
        return visionState.getYellowGoalVision();
    }

    /** Used to get information from vision.
     * @return    status of field cross
     */
    public CrossType getCross() {
        if (visionState == null) return CrossType.NO_CROSS;
        return visionState.getCrossVision();
    }

    /** Used to get information from vision.
     * @return    how many red robots
     */
    public int getRedRobots() {
        if (visionState == null) return 0;
        return visionState.getRedRobotsVision();
    }

    /** Used to get information from vision.
     * @return    how many blue robots
     */
    public int getBlueRobots() {
        if (visionState == null) return 0;
        return visionState.getBlueRobotsVision();
    }

    /** Used to get information from vision.
     * @return    how many L corners
     */
    public int getLCorners() {
        if (visionState == null) return 0;
        return visionState.getLCornersVision();
    }

    /** Used to get information from vision.
     * @return    how many CC corners
     */
    public int getCcCorners() {
        if (visionState == null) return 0;
        return visionState.getCcCornersVision();
    }

    /** Used to get information from vision.
     * @return    how many T corners
     */
    public int getTCorners() {
        if (visionState == null) return 0;
        return visionState.getTCornersVision();
    }

    /** Used to get information from vision.
     * @return    whether human has approved or not
     */
    public String getHuman() {
        //return visionState.getHumanString();
        return "No";
    }

    /** Based on current state returns an appropriate description for
     * display.
     * @return   ball descriptor
     */
    public String getBallString() {
        if (visionState == null) return "No Frame Loaded";
        return visionState.getBallString();
    }

    /** Based on current state returns an appropriate description for
     * display.
     * @return   cross descriptor
     */
    public String getCrossString() {
        if (visionState == null) return "No Frame Loaded";
        return visionState.getCrossString();
    }

    /** Based on current state returns an appropriate description for
     * display.
     * @return   red robot descriptor
     */
    public String getRedRobotString() {
        if (visionState == null) return "No Frame Loaded";
        return visionState.getRedRobotString();
    }

    /** Based on current state returns an appropriate description for
     * display.
     * @return   blue robot descriptor
     */
    public String getBlueRobotString() {
        if (visionState == null) return "No Frame Loaded";
        return visionState.getBlueRobotString();
    }

    /** Based on current state returns an appropriate description for
     * display.
     * @return   blue robot descriptor
     */
    public String getLCornerString() {
        if (visionState == null) return "No Frame Loaded";
        return visionState.getLCornerString();
    }

    /** Based on current state returns an appropriate description for
     * display.
     * @return   blue robot descriptor
     */
    public String getTCornerString() {
        if (visionState == null) return "No Frame Loaded";
        return visionState.getTCornerString();
    }

    /** Based on current state returns an appropriate description for
     * display.
     * @return   cc corner descriptor
     */
    public String getCcCornerString() {
        if (visionState == null) return "No Frame Loaded";
        return visionState.getCcCornerString();
    }

    /** Based on current state returns an appropriate description for
     * display.
     * @return   blue goal descriptor
     */
    public String getBlueGoalString() {
        if (visionState == null) return "No Frame Loaded";
        return visionState.getBlueGoalString();
    }

    /** Based on current state returns an appropriate description for
     * display.
     * @return   yellow goal descriptor
     */
    public String getYellowGoalString() {
        if (visionState == null) return "No Frame Loaded";
        return visionState.getYellowGoalString();
    }


    /**
     * If visionState is null, initialize, else just load the frame.
     *
     * @param f Frame to load into the vision state
     */
    public void newFrameForVisionState(Frame f) {
        if (visionState == null)
            visionState = new VisionState(f, tool.getColorTable());
        else
            visionState.newFrame(f, tool.getColorTable());
    }


}
