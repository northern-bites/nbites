package TOOL.Calibrate;

import TOOL.Data.ColorTableListener;
import TOOL.Data.DataListener;
import TOOL.Data.DataSet;
import TOOL.Data.Frame;

import TOOL.GUI.IncrementalSliderParent;

import TOOL.Image.CalibrationDrawingPanel;
import TOOL.Image.ColorTable;
import TOOL.Image.DrawingPanel;
import TOOL.Image.ImageMarkerPanel;
import TOOL.Image.ImageOverlay;
import TOOL.Image.ImageOverlayAction;
import TOOL.Image.ImagePanel;
import TOOL.Image.ImageSwatch;
import TOOL.Image.PixelSelectionPanel;
import TOOL.Image.ProcessedImage;
import TOOL.Image.TOOLImage;
import TOOL.Image.ThresholdedImage;

import TOOL.Misc.Estimate;
import TOOL.TOOL;
import TOOL.Vision.Vision;

import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.GradientPaint;
import java.awt.Graphics2D;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.Toolkit;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;

import java.awt.geom.*;
import java.awt.image.*;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import java.lang.Math;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map;
import java.util.Set;
import java.util.Stack;
import java.util.Vector;

import javax.swing.JPanel;
import javax.swing.JSplitPane;



/**
 * This is the over arching calibration class. It is responsible for many things
 * including: Seting up the content panes, listening to clicks in them, and
 * using the click information to make changes to the color table. Calibrate is
 * also responsible for keeping track of the brush sizes, and the current color
 * to threshold to.
 * @author modified Nicholas Dunn
 */

public class Calibrate implements DataListener, MouseListener,
                                  MouseMotionListener,
                                  MouseWheelListener, PropertyChangeListener,
                                  ColorSwatchParent,
                                  IncrementalSliderParent,
                                  ColorTableListener
{

    // Brush variables
    public static final int MAX_BRUSH_SIZE = 45;
    public static final int MIN_BRUSH_SIZE = 2;
    public static final int DFLT_BRUSH_SIZE = 10;
    private int brushSize;

    // Color variables
    private Color[] cursorColors = Vision.COLORS;
    public static final int NUM_THRESH_COLORS = Vision.COLORS.length;
    // Start as green..
    public static final byte DFLT_COLOR = 2;
    private byte currentColor;

    public static final int Y = 0, U = 0, V = 0;
    protected PixelSelectionPanel selector;
    protected CalibrationDrawingPanel displayer;
    protected ImageOverlay overlay;
    protected TOOLImage rawImage;
    protected int imageID;
    protected ProcessedImage thresholdedImage;
    protected TOOL tool;
    protected ColorTable colorTable;
    protected CalibratePanel calibratePanel;
    protected VisionState visionState;

    // Holds the changes made to overlay; the rest are taken care of
    // within ColorTable
    private LinkedList <ImageOverlayAction> undoStack;
    private LinkedList <ImageOverlayAction> redoStack;

    protected JPanel main_panel;

    protected static enum Mode { THRESHOLD, DEFINING_EDGE, UNDEFINE };
    protected Mode mode;

    private Frame currentFrame;
    // Used to keep track of which pixels have already been thresholded in
    // a single click
    private Set<Point> currentSelections;


    private JSplitPane split_pane;
    private boolean split_changing;

    // If true, the currently selected color will be thresholded as undefined
    private boolean undefineColor;
    private boolean small_table_mode;

    private Point start, end;

    // boolean to decide if the thresholded image colors are drawn or not.
    private boolean drawThreshColors;

    public Calibrate(TOOL t){
        tool = t;
        drawThreshColors = true;
        colorTable = tool.getColorTable();

        mode = Mode.THRESHOLD;

        // Initialize stacks for fixing errors
        undoStack = new LinkedList<ImageOverlayAction>();
        redoStack = new LinkedList<ImageOverlayAction>();

        currentSelections = new HashSet<Point>();


        //get all the image panels ready
        selector = new PixelSelectionPanel();
        displayer = new CalibrationDrawingPanel();
        // We listen to the scale changes and change the cursor accordingly
        displayer.addPropertyChangeListener(this);

        displayer.changeSettings(ImagePanel.SCALE_AUTO_BOTH);
        selector.changeSettings(ImagePanel.SCALE_AUTO_BOTH);

        currentColor = DFLT_COLOR;
        brushSize = DFLT_BRUSH_SIZE;

        setupWindowsAndListeners();


        // Will take care of the cursor
        updateSelectedColor(0);
        updateBrushSize(0);

        undefineColor = false;
        small_table_mode = false;

    }


    public ColorTable getTable() {
        return colorTable;
    }

    /** Fulfills IncrementalSliderParent contract */
    public void setValue(int x) {
        setEdgeThresh(x);
    }


    public void setDrawThreshColors(boolean x) {
        drawThreshColors = x;
        notifyFrame(currentFrame);
    }

    public boolean getDrawThreshColors() {
        return drawThreshColors;
    }

    /**
     * Changes the threshold value of the edge.  A higher value means that
     * fewer edges will be drawn on the image.
     * @param thresh the difference in YUV value necessary to constitute an edge
     */
    protected void setEdgeThresh(int thresh) {
        if (overlay != null) {
            visionState.setEdgeThreshold(thresh);
            notifyFrame(currentFrame);
            selector.repaint();
            displayer.repaint();
        }
    }

    // Acceptance level for Hough Transform
    public void setHoughAcceptThresh(int thresh){
        if (overlay != null){
            visionState.setHoughAcceptThresh(thresh);
            notifyFrame(currentFrame);
            selector.repaint();
            displayer.repaint();
        }
    }

    /**
     * @param choice whether or not to be in undefine mode, where
     * a click will undefine the color currently selected under user's cursor
     */
    protected void setUndefine(boolean choice) {
        undefineColor = choice;
        updateCursor();
    }


    /**
     * Necessary to fulfill the ColorSwatchParent interface; registers
     * ColorSwatchPanel a as the main_panel's keylistener.
     */
    public void addSwatchListener(ColorSwatchPanel a) {
        main_panel.addKeyListener(a);
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
        split_pane.setRightComponent(displayer);
        split_pane.setDividerLocation(.5); // initial divider location
        split_pane.setResizeWeight(.5); // how to distribute new space
        split_changing = false;
        split_pane.addPropertyChangeListener(this);
        images_panel.add(split_pane);

        // set up mouse listeners
        selector.addMouseListener(this);
        displayer.addMouseListener(this);
        selector.addMouseMotionListener(this);
        displayer.addMouseMotionListener(this);
        selector.addMouseWheelListener(this);
        displayer.addMouseWheelListener(this);

        calibratePanel = new CalibratePanel(this);
        main_panel.add(calibratePanel);

        // Make main_panel have the swatches listen to it
        addSwatchListener(calibratePanel.getSwatches());

        // Register a key listener for undo/redo functionality
        main_panel.addKeyListener(calibratePanel);
        main_panel.setFocusable(true);
        main_panel.requestFocusInWindow();

        calibratePanel.fixButtons();
        updateCursor();

    }

    /** @return JPanel holding all of Calibrate stuff */
    public JPanel getContentPane() {
        return main_panel;
    }

    public CalibratePanel getCalibratePanel() {
        return calibratePanel;
    }



    /** @param the color to which to set the current Color; redraws cursor */
    public void setColor(byte n) {
        currentColor = n;
        updateCursor();
    }

    /** Creates a new cursor of the current size and color, sets it as
     * window's cursor */
    protected void updateCursor() {
        Cursor newCursor =
            CalibrationCursor.getCursor(cursorColors[currentColor],
                                        (int)(brushSize * selector.getXScale()),
                                        undefineColor);
        try {
            main_panel.setCursor(newCursor);
        }
        // Main window hasn't yet been initialized..
        catch (NullPointerException e) {}
    }



    private void smallTableUpdate(int[] pixel,
                                  ColorTableUpdate update){

        //TODO: This method needs  some serious cleanup! -Joho (i wrote the code)
        //The SHIFTS for the old, smaller tables, relative to the shift for this
        //table

        int Y_SHIFT =colorTable.getYShift();
        int CB_SHIFT =colorTable.getCBShift();
        int CR_SHIFT =colorTable.getCRShift();

        boolean DEBUG_SMALL_MODE = false;

        int SMALL_Y_SHIFT = 3;
        int SMALL_CB_SHIFT = 2;
        int SMALL_CR_SHIFT = 2;

        int REL_Y_SHIFT = SMALL_Y_SHIFT -Y_SHIFT;
        int REL_CB_SHIFT = SMALL_CR_SHIFT -CR_SHIFT;
        int REL_CR_SHIFT = SMALL_CB_SHIFT -CB_SHIFT;


        //Values in the frame of a small table
        byte small_y = (byte)  (pixel[0] >> SMALL_Y_SHIFT );
        byte small_Cb = (byte) (pixel[1] >> SMALL_CB_SHIFT);
        byte small_Cr = (byte) (pixel[2] >> SMALL_CR_SHIFT);

        //Frames 'rounded' to the frame of this table

        byte reg_y = (byte)  (small_y << REL_Y_SHIFT );
        byte reg_Cb = (byte) (small_Cb << REL_CB_SHIFT);
        byte reg_Cr = (byte) (small_Cr << REL_CR_SHIFT);
        /*
          byte reg_y = (byte)  (pixel[0] >> Y_SHIFT );
          byte reg_Cb = (byte) (pixel[1] >> CB_SHIFT);
          byte reg_Cr = (byte) (pixel[2] >> CR_SHIFT);
        */
        //How big of a box do we want to paint into?
        //each is the dimension of one side of the box
        //effectively 2^SHIFT
        byte y_brush_size = (byte)  (1 << REL_Y_SHIFT);
        byte cb_brush_size = (byte) (1 << REL_CB_SHIFT);
        byte cr_brush_size = (byte) (1 << REL_CR_SHIFT);

        //now we iterate through the box, adding each pixel value to the update
        //but the iteration is at the frame of the regular color table, so we must scale it later
        for(int y = reg_y;
            y < reg_y + y_brush_size; y++ ){
            for(int cb = reg_Cb;
                cb < reg_Cb + cb_brush_size; cb++ ){
                for(int cr = reg_Cr;
                    cr < reg_Cr + cr_brush_size; cr++ ){
                    if(DEBUG_SMALL_MODE)
                        System.out.println(y + " , " + cb + " , " +cr);
                    int[] newPixel = {y<<Y_SHIFT,
                                      cb<<CB_SHIFT,
                                      cr<<CR_SHIFT};
                    byte oldColor =  colorTable.getColor(newPixel);

                    update.addPixel(newPixel,oldColor);
                }
            }

        }

    }


    /**
     * Method determines what to do based on the current state, and where
     * the image was clicked. Right now it recursively thresholds all the
     * pixels underneath the users color swatch cursor, unless their
     * YUV values stray too far from the center most point's.
     * @param x xCoord on image where to start thresholding
     * @param y yCoord.
     */
    public void pixelSelected(int x, int y){
        // clear the HashSet to keep track of which points have been
        // thresholded, in order to know when to stop recursing
        currentSelections.clear();

        // Note the changes to both the color table and the overlay.
        ColorTableUpdate updates = new ColorTableUpdate(currentColor);
        // We need to know which image we're working on so when we undo it,
        // we don't see the action unless that picture is on the screen
        ImageOverlayAction currentMove = new ImageOverlayAction(imageID);

        int w = x;
        int h = y;

        //check to make sure our bounding box doesnt leave the image
        if(inRawImage(w,h)){
            recurseCalibrate(w,h,w,h,rawImage.getYCbCr(w,h),updates, currentMove);
        }

        //send the updates to the colorTable
        colorTable.modifyTable(updates);
        // Make changes to the image overlay
        overlay.execute(currentMove);

        // Make sure the color table can undo this change later
        colorTable.pushUndo(updates);
        colorTable.clearRedo();

        // Make sure we can undo the overlay changes
        pushUndo(currentMove);

        redoStack.clear();

        //update the visionState
        visionState.update();
        //lastly, need to repaint
        // simply repaint the selector, as underlying image hasn't changed
        selector.repaint();

        // displayer needs to be updated to reflect the new thresholded changes
        displayer.updateImage(thresholdedImage);
        displayer.repaint();

        // Alert all color table listeners that the color table has changed
        tool.getDataManager().notifyColorTableDependants(colorTable,
                                                         updates,
                                                         this);
    }

    /**
     * Iterates through entire image, and searches for places where
     * there are "holes"; that is to say there are 8 pixels of the same
     * color around an undefined pixel. It then fills in these missing
     * pixels as the same color as those surrounding it.
     * The ThresholdedImage itself does most of the heavy lifting
     * in this method; it then adds changes made to the image
     * to the undoStack.
     * Based heavily on the old code from Calibrate.java
     */
    public void fillHoles() {

        if (thresholdedImage == null) {
            calibratePanel.setText("Cannot fill holes before loading a data set");
            return;
        }

        Pair<LinkedList<ColorTableUpdate>, Integer> result =
            thresholdedImage.fillHoles();


        LinkedList<ColorTableUpdate> currentUndo = result.getFirst();
        int numHoles = result.getSecond();

        // No changes to the overlay, but push a null so we stay in sync
        pushUndo(null);
        redoStack.clear();

        // We did make a change to the color table however, so add that
        colorTable.pushUndo(currentUndo);
        colorTable.clearRedo();


        calibratePanel.setText("Filled " + numHoles + " holes");
        calibratePanel.fixButtons();
        // displayer needs to be updated to reflect the new thresholded changes
        displayer.updateImage(thresholdedImage);

    }


    /**
     * Iteratively undefines a specific color from the area underneath
     * the user's cursor.
     * @param x the x coord of center of swatch to remove
     * @param y the y coord of center of swatch to remove
     * @param color the thresholded color value to undefine
     */
    public void undefineColor(int x, int y, byte color) {
        swapColor(x, y, color, (byte) Vision.GREY);
    }

    /**
     * Iteratively swaps a specific color from the area underneath
     * the user's cursor with a new color.
     * @param x the x coord of center of swatch to remove
     * @param y the y coord of center of swatch to remove
     * @param startColor the thresholded color value to replace
     * @param replacementColor the thresholded color value with which to replace
     *        startColor
     */
    public void swapColor(int rawX, int rawY, byte startColor,
                          byte replacementColor) {


        // We'll be defining all the pixels as "replacementColor"
        // within this area which are currently classified as "startColor"
        ColorTableUpdate updates = new ColorTableUpdate(replacementColor);
        ImageOverlayAction overlayChanges = new ImageOverlayAction(imageID);


        // Make sure that we don't iterate out of bounds
        int xStart, xStop, yStart, yStop;

        // We clicked near left size of image, can't fit whole rect on image
        if (rawX - brushSize/2 < 0) {
            xStart = 0;
            xStop = rawX + brushSize/2;
        }
        // We clicked near right edge of image
        else if (rawX + brushSize/2 > rawImage.getWidth() - 1) {
            xStart = rawX - brushSize/2;
            xStop = rawImage.getWidth();
        }
        // Somewhere in middle
        else {
            xStart = rawX - brushSize/2;
            xStop = rawX + brushSize/2;
        }

        // Clicked near top, can't fit whole rect
        if (rawY - brushSize/2 < 0) {
            yStart = 0;
            yStop = rawY + brushSize/2;
        }
        // Near bottom of screen
        else if (rawY + brushSize/2 > rawImage.getHeight() - 1) {
            yStart = rawY - brushSize/2;
            yStop = rawImage.getHeight();
        }
        else {
            yStart = rawY - brushSize/2;
            yStop = rawY + brushSize/2;
        }

        // Iterate over the rectangular region.
        for (int i = xStart; i < xStop; i++) {
            int threshI = i * thresholdedImage.getHeight() /
                rawImage.getHeight();
            for (int j = yStart; j < yStop; j++) {

                int threshJ = j * thresholdedImage.getWidth() /
                    rawImage.getWidth();

                // The color table has the start color at this pixel, swap
                // it with the new color
                if (thresholdedImage.
                    getThresholded(threshI, threshJ) == startColor) {

                    int[] currPixel = rawImage.getYCbCr(i,j);
                    updates.addPixel(currPixel, startColor);
                }

                // Even if the thresholded value in color table is not the
                // right color, if the overlay is still that color, remove
                // the color from the overlay.  This is a much more pleasing
                // aesthetic than the alternative - you don't get speckles.
                if (overlay.getThreshColor(i, j) == startColor) {
                    overlayChanges.add(i, j, startColor, replacementColor);
                }
            }
        }

        //send the updates to the colorTable
        colorTable.modifyTable(updates);
        overlay.execute(overlayChanges);

        // Color table gets the change added to its stack, clears redos
        colorTable.pushUndo(updates);
        colorTable.clearRedo();

        // Add to undostack and get rid of the redostack if it exists
        pushUndo(overlayChanges);
        redoStack.clear();


        // update the vision state (which thresholds the whole image
        // again, and updates the objects)
        visionState.update();
        //lastly, need to repaint
        // displayer needs to be updated to reflect the new thresholded changes
        displayer.updateImage(thresholdedImage);
        displayer.repaint();
        selector.repaint();

        // Alert all color table listeners that the color table has changed
        tool.getDataManager().notifyColorTableDependants(colorTable,
                                                         updates,
                                                         this);

    }





    /**
     * Recursively calibrates the image. Based loosly on code from JavaConnect.
     * Makes sure none of YUV vals are further than EDGE-THRESH from the orig.
     * Also makes sure we stay within brush size of the original pixel.
     *
     * @param x x-coord of pixel to calibrate
     * @param y y-coord of pixel to calibrate
     * @param xorig original pixel's x coord
     * @param yorig original pixel's y coord
     * @param origPixel the original YUV array
     * @param updates DS to add new pixels too
     * @param overlay keeps track of changes made to the overlay, for undoing
     */
    public void recurseCalibrate(int rawX, int rawY,
                                 int xorig, int yorig,
                                 int[] origPixel,
                                 ColorTableUpdate updates,
                                 ImageOverlayAction overlayAction){

        // We've already thresholded it
        if (currentSelections.contains(new Point(rawX, rawY))) {
            return;
        }

        //check to make sure we are still within orig. brush size
        if(Math.abs(rawX-xorig) > brushSize/2 ||
           Math.abs(rawY-yorig) > brushSize/2 )
            return;

        //make sure this pixel is in the image
        if(!inRawImage(rawX, rawY))
            return;

        //since it's in the image, and in the brush size, get new pix
        int[] currPixel = rawImage.getYCbCr(rawX,rawY);

        //check to see if we are making a jump from the orig pixel
        if(isTransition(origPixel,currPixel))
            return;


        /*if(overlay.isEdge(x,y))
          return;*/

        //so its an ok pixel, lets threshold it
        // keep track that we've already seen it
        currentSelections.add(new Point(rawX, rawY));

        int threshX = rawX * thresholdedImage.getWidth() /
            rawImage.getWidth();
        int threshY = rawY * thresholdedImage.getHeight() /
            rawImage.getHeight();

        // Find out the old threshold value (if any)
        byte oldColor = thresholdedImage.getThresholded(threshX, threshY);
        byte oldOverlayColor = overlay.getThreshColor(threshX, threshY);

        if(small_table_mode){

            smallTableUpdate(currPixel,updates);

        }else{
            updates.addPixel(currPixel, oldColor);
        }
        overlayAction.add(rawX, rawY, oldOverlayColor, currentColor);

        //overlay.setOverlay(x,y,currentColor);

        //figure out where to look next
        int left = rawX - 1;
        int right = rawX + 1;
        int up = rawY -1;
        int down = rawY +1;

        //check to the left
        recurseCalibrate(left,rawY,xorig,yorig,
                         origPixel,updates, overlayAction);

        //check to the right
        recurseCalibrate(right,rawY,xorig,yorig,
                         origPixel,updates, overlayAction);

        //check up
        recurseCalibrate(rawX,up,xorig,yorig,origPixel,updates, overlayAction);

        //check down
        recurseCalibrate(rawX,down,xorig,yorig,
                         origPixel,updates, overlayAction);


    }


    /**
     * Reverts most recent change to both the ColorTable and the ImageOverlay,
     * if any change exists.
     */
    public void undo() {

        if (undoStack.isEmpty()) {
            calibratePanel.setText("Undo stack is empty; cannot undo.");
            return;
        }


        // Takes care of adding it to the redo stack in the color table
        colorTable.undo();

        ImageOverlayAction overlayChanges = undoStack.removeLast();
        pushRedo(overlayChanges);

        calibratePanel.setText("Undoing. " + undoStack.size() +
                               " undos left");


        // We only change the imageOverlay if we're looking at the same
        // picture in which the changes were made
        if (overlayChanges != null &&
            imageID == overlayChanges.getID()) {
            overlay.revert(overlayChanges);
        }

        //update the thresholded image by calling visionstate.update
        visionState.update();
        displayer.updateImage(thresholdedImage);
        displayer.repaint();
        selector.repaint();

        // Update the undo button
        calibratePanel.fixButtons();
    }


    public void pushUndo(ImageOverlayAction e) {
        if (undoStack.size() == ColorTable.MAX_NUM_UNDOS) {
            undoStack.removeFirst();
        }
        undoStack.addLast(e);
    }

    public void pushRedo(ImageOverlayAction e) {
        if (redoStack.size() == ColorTable.MAX_NUM_REDOS) {
            redoStack.removeFirst();
        }
        redoStack.addLast(e);
    }


    /**
     * Repeats the most recent action pushed onto the UndoStack if it exists.
     * Every time an action is performed other than undoing, the redoStack
     * is emptied.
     */
    public void redo() {
        if (redoStack.isEmpty()) {
            calibratePanel.setText("Redo stack is empty; cannot redo.");
            return;
        }

        // Takes care of adding move to undo stack
        colorTable.redo();

        ImageOverlayAction overlayChanges = redoStack.removeLast();
        pushUndo(overlayChanges);

        calibratePanel.setText("Redoing. " + redoStack.size() +
                               " redos left");

        // We only change the imageOverlay if we're looking at the same
        // picture in which the changes were made.  Also, if we're redoing
        // a fill holes action, we need to not do anything to the overlay
        // because we push a null in the fill holes method.
        if (overlayChanges != null && imageID == overlayChanges.getID()) {
            overlay.execute(overlayChanges);
        }

        //update the thresholded image by calling update on visionstate
        visionState.update();
        // displayer needs to be updated to reflect the new thresholded changes
        displayer.updateImage(thresholdedImage);
        displayer.repaint();
        selector.repaint();

        // Update the redo button
        calibratePanel.fixButtons();
    }


    /** Determines whether or not the given YUV values of the old pixel are
     * too far from the YUV values of new pixel (in which case recursion on
     * thresholding stops*/
    public boolean isTransition(int[] oldPixel, int[] newPixel){
        return overlay.isTransition(oldPixel,newPixel);
    }


    public void setThresholded(boolean choice) {
        if (overlay != null) {
            overlay.setThresholding(choice);
            selector.repaint();
            displayer.repaint();
        }
    }


    public void getLastImage() {
        tool.getDataManager().last();
        // fix the backward skipping button
        calibratePanel.fixButtons();
    }

    public void getNextImage() {
        tool.getDataManager().next();
        // fix the forward skipping button
        calibratePanel.fixButtons();
    }

    public void skipForward(int i) {
        tool.getDataManager().skip(i);
        // fix the forward skipping button
        calibratePanel.fixButtons();
    }

    public void skipBackward(int i) {
        tool.getDataManager().revert(i);
        // fix the backward skipping button
        calibratePanel.fixButtons();
    }

    public void setImage(int i) {
        tool.getDataManager().set(i);
    }


    /**
     * Updates the brushSize variable and ensures that it neither
     * becomes too big nor too small.  Note:  Mousing all the way up
     * will not cause the size to wrap back around; rather it will
     * stay at the max size.
     */
    private void updateBrushSize(int amtToShift){
        brushSize +=amtToShift;
        if(brushSize < 1){
            brushSize = 1;
        }else if(brushSize > MAX_BRUSH_SIZE){
            brushSize = MAX_BRUSH_SIZE;
        }
        selector.setBrushSize(brushSize);
        selector.repaint();
        displayer.repaint();
    }
    /**
     * Updates the currentColor variable which changes the
     * look of the cursor as well as what color will be thresholded
     * on a user's click.  Note:  Cursor does not update automatically after
     * this call; you must call updateCursor() afterward.  Just like
     * updateBrushSize, there is no wrapping of the colors; you must scroll
     * back through to get to the beginning if you're at the end
     */
    private void updateSelectedColor(int amtToShift){
        currentColor +=amtToShift;
        if(currentColor < 0){
            currentColor = 0;
        }else if(currentColor >= NUM_THRESH_COLORS){
            currentColor = (byte) (NUM_THRESH_COLORS - 1);
        }
        selector.setCalibrateColor(currentColor);
        selector.repaint();
        displayer.repaint();

        calibratePanel.setColorSelected(currentColor);
    }

    /**
     * Checks if a point is within the thresholded image.
     */
    public boolean inThreshImage(int x, int y){
        return (x < thresholdedImage.getWidth() &&
                y < thresholdedImage.getHeight() &&
                y >= 0 && x >= 0);
    }

    /**
     * Checks if a point is within the raw image.
     */
    public boolean inRawImage(int x, int y){
        return (x < rawImage.getWidth() &&
                y < rawImage.getHeight() &&
                y >= 0 && x >= 0);
    }

    /**
     * @return whether we are in small_table_mode
     */
    public boolean isSmallTableMode() {
        return small_table_mode;
    }

    public void setSmallTableMode(boolean choice) {
        small_table_mode = choice;
        if (choice) {
            calibratePanel.setText("Small Table Mode enabled");
        }
        else {
            calibratePanel.setText("Small Table Mode disabled");
        }
    }

    public boolean canUndo() {
        return !undoStack.isEmpty();
    }

    public boolean canRedo() {
        return !redoStack.isEmpty();
    }

    public boolean canGoForward() {
        return tool.getDataManager().hasElementAfter();
    }

    public boolean canGoBackward() {
        return tool.getDataManager().hasElementBefore();
    }

    public void clearHistory() {
        undoStack.clear();
        redoStack.clear();
    }

    public TOOL getTool() {
        return tool;
    }

    /** @return true if we have a thresholded image, else false. */
    public Boolean hasImage() {
        return thresholdedImage != null;
    }


    ////////////////////////////////////////////////////////////
    // LISTENER METHODS
    ////////////////////////////////////////////////////////////

    // MouseListener methods
    public void mouseClicked(MouseEvent e) {}

    // When mouse enters, make sure the cursor is the rectangular swatch
    public void mouseEntered(MouseEvent e) {
        updateCursor();
    }

    public void mouseExited(MouseEvent e) {    }

    public void mousePressed(MouseEvent e) {
        start = e.getPoint();
    }

    public void mouseReleased(MouseEvent e) {

        int displayerX=0, displayerY=0, selectorX=0, selectorY=0;

        int x = e.getX();
        int y = e.getY();

        // Set the appropriate cursor values in each It matters,
        // because the displayer has the thresholded image and the
        // selector has the raw image and they can be different sizes
        if (e.getSource() == selector && rawImage != null) {
            displayerX = (selector.getImageX(x) *
                          displayer.getImage().getWidth()/
                          selector.getImage().getWidth());
            displayerY = (selector.getImageY(y) *
                          displayer.getImage().getHeight()/
                          selector.getImage().getHeight());
            selectorX = selector.getImageX(x);
            selectorY = selector.getImageY(y);
        } else if (e.getSource() == displayer && rawImage != null) {
            selectorX = (displayer.getImageX(x) *
                         selector.getImage().getWidth() /
                         displayer.getImage().getWidth());
            selectorY = (displayer.getImageY(y) *
                         selector.getImage().getHeight() /
                         displayer.getImage().getHeight());

            displayerX = displayer.getImageX(x);
            displayerY = displayer.getImageY(y);
        }

        //when the mouse is clicked, look up the
        //place in the image where it was clicked,
        if(e.getButton() == MouseEvent.BUTTON1 && !e.isControlDown()){

            // make sure we clicked inside the image
            if (inThreshImage(displayerX, displayerY)) {
                if (e.isShiftDown()) {

                    // get the pix est on this pixel assuming object height=0
                    Estimate est = thresholdedImage.pixEstimate(displayerX,
                                                                displayerY,0);

                    // we want to print a pixEstimate on this pixel if
                    // shift is down
                    System.out.printf(
                                      "pixel (%d,%d) dist: %.2f" +
                                      " bearing: %.2f\n",
                                      x, y, est.dist, est.bearing);
                } else{
                    // Undefine the color underneath the cursor if in that mode.
                    if (undefineColor) {
                        undefineColor(selectorX, selectorY, currentColor);
                    }
                    // Otherwise, do the normal process of thresholding
                    // the area under the cursor to be the currentColor.
                    else {
                        pixelSelected(selectorX, selectorY);
                    }
                }
            }
        }
        // Update the buttons; undo function might be available now
        calibratePanel.fixButtons();

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
            updateCursor();
        }
    }

    public void mouseMoved(MouseEvent e) {
        int x = e.getX();
        int y = e.getY();

        //move the marker around in the respective windows, even
        //when the mouse is not there
        if (e.getSource() == selector) {

                int selectorX = selector.getImageX(x);
                int selectorY = selector.getImageY(y);

            if (rawImage != null) {

                int displayerX = (selector.getImageX(x) *
                                 displayer.getImage().getWidth()/
                                 selector.getImage().getWidth());
                int displayerY = (selector.getImageY(y) *
                                 displayer.getImage().getHeight()/
                                 selector.getImage().getHeight());

                displayer.setMarkerImagePosition(displayerX,
                                                 displayerY);

                int tempx = selectorX;
                int tempy = selectorY;

                if (tempy < rawImage.getHeight() && tempy > 0 && tempx > 0 &&
                    tempx < rawImage.getWidth()) {
                    int pixie[] = rawImage.getPixel(selectorX,
                                                    selectorY);
                    int Y_SHIFT =colorTable.getYShift();
                    int CB_SHIFT =colorTable.getCBShift();
                    int CR_SHIFT =colorTable.getCRShift();

                    pixie[0] = pixie[0] >> Y_SHIFT;
                    pixie[1] = pixie[1] >> CB_SHIFT;
                    pixie[2] = pixie[2] >> CR_SHIFT;

                    calibratePanel.setXYText(displayerX,
                                             displayerY,
                                             pixie);
                }
            } else {
                calibratePanel.setXYText(selectorX,
                                         selectorY);
            }
        }else if (e.getSource() == displayer) {

            if (rawImage != null){
                int selectorX = (displayer.getImageX(x) *
                                 selector.getImage().getWidth() /
                                 displayer.getImage().getWidth());
                int selectorY = (displayer.getImageY(y) *
                                 selector.getImage().getHeight() /
                                 displayer.getImage().getHeight());

                selector.setMarkerImagePosition(selectorX,
                                                selectorY);
            }

            calibratePanel.setXYText(displayer.getImageX(x),
                                     displayer.getImageY(y));
        } else {
            calibratePanel.setXYText(-1,-1);
        }
    }

    //mouseWheelListener Methods
    public void mouseWheelMoved(MouseWheelEvent e){

        // Change pictures
        if (e.isMetaDown() || e.isControlDown()) {
            if (e.getWheelRotation() > 0) {
                getNextImage();
            }
            else{
                getLastImage();
            }
            return;
        }
        else if(e.isShiftDown()){
            //update selected color
            updateSelectedColor(e.getWheelRotation());
        }
        else {
            //update brush size
            updateBrushSize(e.getWheelRotation());
        }

        // Either color or size changed, fix the cursor
        updateCursor();
    }

    //dataListener Methods
    public void notifyDataSet(DataSet s, Frame f) {
        notifyFrame(f);
    }
    //to do: clean up this code - Octavian
    public void notifyFrame(Frame f) {
        currentFrame = f;
        if (!f.hasImage())
            return;
        //if visionState is null, initialize, else just load the frame
        if (visionState == null)
            visionState = new VisionState(f, tool.getColorTable());
        else
            visionState.newFrame(f, tool.getColorTable());

        thresholdedImage = visionState.getThreshImage();//sync the thresholded images
        rawImage = visionState.getImage();
        imageID = rawImage.hashCode();

        colorTable = visionState.getColorTable();

        // Since we now handle different sized frames, it's possible
        // to switch between modes, changing the image's size without
        // updating the overlay.  This will catch that
        if(overlay == null || overlay.getWidth() != rawImage.getWidth()) {
            overlay =
                new ImageOverlay(rawImage.getWidth(),rawImage.getHeight());
        } else {
            overlay.resetPixels();
        }
        selector.updateImage(rawImage);
        calibratePanel.setSelectorOverlay();

        visionState.update();
        calibratePanel.setDisplayerOverlay();
        displayer.updateImage(thresholdedImage);

        selector.repaint();
        displayer.repaint();

        // They loaded something so make sure our buttons reflect the
        // active state; e.g. that our undo stack and redo stack are
        // empty.
        calibratePanel.fixButtons();
        // 0 based indexing.
        calibratePanel.setText("Image " + (f.index()) + " of " +
                               (f.dataSet().size() - 1) +
                               " -  processed in " + visionState.getProcessTime() +
                               " micro secs");
    }


    public void colorTableChanged(ColorTable source, ColorTableUpdate update,
                                  ColorTableListener originator) {
        // We don't want to deal with actions that we ourselves propagated;
        // should already be handled by the time this method is evoked
        if (originator == this) { return; }

        //threshold the full image again
        if(thresholdedImage != null)//if no frame is loaded, don't want to update
            visionState.update();
        //lastly, need to repaint
        selector.repaint();
        displayer.repaint();
    }

    public CalibrationDrawingPanel getDisplayer() {
        return displayer;
    }

    public PixelSelectionPanel getSelector() {
        return selector;
    }

    public ImageOverlay getEdgeOverlay() {
        return overlay;
    }

    public VisionState getVisionState() {
        return visionState;
    }

    public void resetSet(){
        tool.getDataManager().useFrame(0);
        tool.newColorTable();
    }
}
