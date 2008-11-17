
package edu.bowdoin.robocup.TOOL.ColorEdit;

import java.lang.Math;

import javax.swing.JFrame;
import javax.swing.JPanel;

import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelListener;

import javax.swing.*;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.GradientPaint;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Color;
import java.awt.image.BufferedImage;
import java.util.Iterator;

import edu.bowdoin.robocup.TOOL.TOOL;
import edu.bowdoin.robocup.TOOL.Data.ColorTableListener;
import edu.bowdoin.robocup.TOOL.GUI.IncrementalSliderParent;
import edu.bowdoin.robocup.TOOL.Calibrate.ColorSwatchPanel;
import edu.bowdoin.robocup.TOOL.Calibrate.ColorSwatchParent;
import edu.bowdoin.robocup.TOOL.Calibrate.ColorTableUpdate;
import edu.bowdoin.robocup.TOOL.Calibrate.VisionState;
import edu.bowdoin.robocup.TOOL.Data.DataListener;
import edu.bowdoin.robocup.TOOL.Data.DataSet;
import edu.bowdoin.robocup.TOOL.Data.Frame;
import edu.bowdoin.robocup.TOOL.Image.ColorTable;
import edu.bowdoin.robocup.TOOL.Image.ColorTablePanel;
import edu.bowdoin.robocup.TOOL.Image.YCbCrColorSpace;
import edu.bowdoin.robocup.TOOL.Vision.Vision;



/**
 * 
 * Color table editor.  Allows the user to have different views of a color
 * table, and to edit individual entries.
 * 
 * Two main modes: viewing slices of the color table along an axis (Y, U, or V)
 * and viewing all entries of a given color.
 *
 * The three modes YVIEW, UVIEW, and VVIEW correspond to fixing as constant the
 * Y, U, and V axes respectively.  The value of the slider corresponds with the 
 * fixed value through which you're slicing; e.g. if you are in YMODE and the
 * slider is set to 13, you will see all (13, U, V) values in the table.
 *
 * Within each of those three views, there is a different way of seeing the 
 * table.  By default, the ranges viewable depend on which color you choose.
 * For instance, if you pick blue in the YMODE, the program will look in the
 * color table to see the minimum Y value that's been thresholded as blue, as
 * well as the max, in order to fix the slider values.  The same thing happens
 * for U and V so that the table you see is "zoomed in" around the areas of 
 * blue.
 *
 * If you choose the "All View" checkbox, you effectively zoom out from the 
 * table and see the entire thing.  Instead of fixing the YUV values based on
 * a given color, the view instead ranges over the min and max of each variable
 * for the entire table - so the minimum Y value in the whole table that's been
 * thresholded something other than GREY, to the maximum Y value, and so on so
 * forth.
 *
 * For the COLORVIEW, we go through the table and find every instance where
 * something has been thresholded as the current color, and display all the
 * shades on the screen in the order in which we found it.  This looks very 
 * pretty and mesmerizing but I'm honestly not sure why it's useful. 
 *
 * In each of the YUV Modes, you can change the thresholding of entries directly
 * if you are in edit mode.  Once in edit mode, choose the color you wish to
 * threshold and click the entry you wish to change.  You can also
 * press and drag a rectangle to fill a whole area with color.
 *
 * To come: 
 * Fixed editing of color space
 * Zooming?
 * Simultaneous view with threshold view
 * 
 *
 * @author Eric Chown
 * @date   May 13, 2007
 * Conversion to TOOL use and overhaul by Nick Dunn.
 */
public class ColorEdit extends JPanel implements DataListener,
                                                 ColorSwatchParent,
                                                 MouseListener,
                                                 IncrementalSliderParent,
                                                 MouseWheelListener,
                                                 ColorTableListener {
                                                  
    public static enum View { YVIEW, UVIEW, VVIEW, COLORVIEW };
    private ColorTablePanel imagePanel;

    // Variables for colors
    public static final int NUM_COLORS = 14;
    
    public static final int FAKE_MIN = 300;
    public static final int FAKE_MAX = -300;
    public static final int Y_MIN = 0;
    public static final int Y_MAX = 1;
    public static final int U_MIN = 2;
    public static final int U_MAX = 3;
    public static final int V_MIN = 4;
    public static final int V_MAX = 5;
    public static final int NUM_ENTRIES = 6;
    
    public static final int DEFAULT_MAX = 256;
    public static final int DEFAULT_SIZE = DEFAULT_MAX;

    public static final byte GREY = Vision.GREY;
    public static final byte WHITE = Vision.WHITE;
    public static final byte GREEN = Vision.GREEN;
    public static final byte BLUE = Vision.BLUE;
    public static final byte YELLOW = Vision.YELLOW;
    public static final byte ORANGE = Vision.ORANGE;
    public static final byte YELLOWWHITE = Vision.YELLOWWHITE;
    public static final byte BLUEGREEN = Vision.BLUEGREEN;
    public static final byte ORANGERED = Vision.ORANGERED;
    public static final byte ORANGEYELLOW = Vision.ORANGEYELLOW;
    public static final byte RED = Vision.RED;
    public static final byte NAVY = Vision.NAVY;
    public static final byte BLACK = Vision.BLACK;
    public static final byte PINK = Vision.PINK;

    public static final byte DEFAULT_COLOR = GREEN;
   
     // Used to keep track of which rectangular region to edit.
    private Point startLoc, endLoc;

    // We keep stats about YUV min and max (6) and one for the count
    public static final int NUMSTATS = 7;
    // array goes from 0 to NUM_COLORS - 1 to represent each color's info;
    // the global information goes in NUM_COLORS index.
    public static final int GLOBAL_INDEX = NUM_COLORS;
    
    // Private variables:
    // leave one extra space for the global color table stats
    private int[][] colorStats = new int[NUM_COLORS + 1][NUMSTATS];
    private int yShift, uShift, vShift;
    // these correspond to the size of the YUV channels in the color table;
    // they're generally smaller than the DEFAULT_MAX due to compression
    private int yMax, uMax, vMax;

    // Used to keep track of the axes scale as well as to calculate
    // offsets.
    private int xAxisMin, xAxisMax, yAxisMin, yAxisMax;
  
    // Currently selected value on slider
    private int sliderValue;
    private byte curColor;
    // If we're editing, sometimes the color through which we're slicing is
    // different than which we have selected; keep track.
    private byte curColorForSlice;

    // if we're in All view vs. y view, etc.
    protected View view;
    // if true, we do not restrict YUV values based on the color we have 
    // selected
    private boolean allView;
    // if true we can edit the table, plus our drawing methods change
    private boolean editing;

    // ColorTable we're currently working on, getting information from
    private ColorTable colorTable;

    // Holds all the GUI stuff that we add ourself to.
    private TOOL tool;
    private ColorEditPanel buttonPanel;
    // buffImage is the background gradient of colors; overlay is the 
    // thresholded colors
    private BufferedImage buffImage, overlay;
    // Used to convert from YUV to RGB
    private YCbCrColorSpace converter;

    public ColorEdit(TOOL t) {
	super();
        
	tool = t;
	colorTable = tool.getColorTable();

        // Get the necessary shifting info from table
	yShift = colorTable.getYShift();
	uShift = colorTable.getUShift();
	vShift = colorTable.getVShift();
        // and the size 
        yMax = colorTable.getYDimension();
        uMax = colorTable.getUDimension();
        vMax = colorTable.getVDimension();

        view = View.YVIEW;

        converter = new YCbCrColorSpace();
	
        setupWindowsAndListeners();
        collectStats();
        
        setColor(DEFAULT_COLOR);
        buttonPanel.setColor(DEFAULT_COLOR);

    }
    
    private void setupWindowsAndListeners() {
        // Necessary to have the drawing panels size correctly
        
        setLayout(new GridLayout(2,0));

        buttonPanel = new ColorEditPanel(this);
        imagePanel = new ColorTablePanel(this);
        imagePanel.changeSettings(ColorTablePanel.SCALE_RATIO_SIZE_HYBRID);
        
        buffImage = new BufferedImage(DEFAULT_SIZE, DEFAULT_SIZE,
                                      BufferedImage.TYPE_INT_ARGB); 
        imagePanel.setImage(buffImage);
        
        add(imagePanel);
        add(buttonPanel);


        imagePanel.addMouseListener(this);
        addMouseWheelListener(this);
        addSwatchListener(buttonPanel.getSwatches());
        addKeyListener(buttonPanel);

        repaint();
    }


    public void setTable(ColorTable a) {
        colorTable = a;

        // Get the necessary shifting info from table
	yShift = colorTable.getYShift();
	uShift = colorTable.getUShift();
	vShift = colorTable.getVShift();
        // and the size 
        yMax = colorTable.getYDimension();
        uMax = colorTable.getUDimension();
        vMax = colorTable.getVDimension();

        refresh();
    }



    /** fulfills ColorSwatchParent interface; adds a listener to the 
     * colorSwatchPanel so that we can process keystrokes intended for it 
     * (e.g. the numbers 1-0 on the keyboard)
     */
    public void addSwatchListener(ColorSwatchPanel a) {
        addKeyListener(a);
    }


    /**
     * Fulfills DataListener interface.  When we get a new color table,
     * we react to it.
     */
    public void notifyDataSet(DataSet s, Frame f) {
        notifyFrame(f);
    }
    
    public void notifyFrame(Frame f) {
        colorTable = tool.getColorTable();
        collectStats();
    }

    /** 
     * Called whenever the user clicks on a color swatch.
     * Fulfills interface requirement.
     * In colorview, means we need to recolorize the image.
     * Otherwise we need to slice and fix the axes.
     */
    public void setColor(byte n) {
        curColor = n;
        buttonPanel.setColor(n);
        imagePanel.setColor(byteToColor(n));

        // If we're in edit mode, we want to keep our old
        // color for slice
        if (!editing) {
            curColorForSlice = n;
        }

        if (view == View.COLORVIEW) {
            colorizeImage();
            repaint();
        }
        // If we're editing, we don't want to change the scales;
        // just the selected color.  Otherwise what you're trying to
        // edit goes away from you.
        else if (!editing) {
            slice();
            fixSliderAxes();
        }
    }

    

    
    public void refresh() {
        collectStats();
        if (view == View.COLORVIEW){
            colorizeImage();
        }
        else {
            slice();
        }
        // Since the axes depend on the entries stored for min/max YUV values,
        // we must call this AFTER refreshing and collecting the stats
        fixSliderAxes();

        repaint();
    }

    public ColorTable getColorTable() {
        return colorTable;
    }

    // TODO: Fill in
    public void fillHoles() {
	if(editing){
	    ColorTableUpdate change = colorTable.fillHoles(curColor);
	    

	    // figure out what needs to be colored..
	    slice();
	    repaint();

	    // Alert all color table listeners that the color table has changed
	    tool.getDataManager().notifyColorTableDependants(colorTable,
							     change,
							     this);
	}else{
	    System.out.println("Must be in EDIT mode to fill holes");
	}
    }

    // Whereas refresh() will refresh entire screen and recalculate all stats,
    // by passing in a ColorTableUpdate we can selectively modify our stats
    // and determine what to recolor without repainting the entire screen.
    public void refresh(ColorTableUpdate d) {
        byte newColor = d.getColor();
        Iterator<int[]>YUV = d.getIterator();
        Iterator<Byte>oldColors = d.getOldColorIterator();

        // Increase the number of entries for our color by the number of entries
        // in our update
        colorStats[newColor][NUM_ENTRIES] += d.getSize();

        while (YUV.hasNext()) {
            YUV.next();
        }

    }

    
    public ColorTable getTable() {
        return colorTable; 
    }

    public ColorEditPanel getColorEditPanel() {
        return buttonPanel;
    }

    /**
     * Attempts to undo the most recent change to the color table.  If
     * the ImageChangeStack is empty, prints out an error message alerting
     * user to that fact.
     * Note that since that we are working directly with the color table,
     * all of the ImageOverlayActions we push and pop off the stack will be
     * null, as we are not changing the overlay at all (in fact, 
     * it doesn't exist for us in this module)
     */
    public void undo() {
	if (!colorTable.canUndo()) {
	    buttonPanel.setText("Undo stack is empty; cannot undo.");
	    return;
	}
	
	
        colorTable.undo();

        if (view == View.COLORVIEW) {
            colorizeImage();
        }
        else {
            slice();
        }
	repaint();

    }
    
    public void redo() {
	if (!colorTable.canRedo()) {
	    buttonPanel.setText("Redo stack is empty; cannot redo.");
	    return;
	}
                
        colorTable.redo();
        if (view == View.COLORVIEW) {
            colorizeImage();
        }
        else {
            slice();
        }

        repaint();
        
    }
    
    /**
     * Sets the allView off or on.  If allView is on, then in a given
     * slice of Y, U, or V value, we will let the other free axes
     * range through all values we have information for.  Otherwise,
     * we will restrict the values to those of the current color.
     */
    public void setAllView(boolean choice) {
        allView = choice;
        if (view != View.COLORVIEW) {
            slice();
            fixSliderAxes();
            repaint();
        }
    }

    
    
    /** Initializes the color table statistics.
     * Starts by assigning arbitrarily small values to the
     * max statistics and arbitrarily large values to the
     * minimum statistics, that way as soon as we get an actual
     * entry, it becomes the new min and max for that channel,
     * and we go on from there.
     */
    
    public void initStats() {
	// init table
        for (int i = 0; i < NUM_COLORS+1; i++) {
            for (int j = 0; j < NUMSTATS - 1; j++) {
		// 1, 3, 4, the maxes ones...
                if (j % 2 == 1) {
		    colorStats[i][j] = FAKE_MAX;
		}
		else {
                    // mins
		    colorStats[i][j] = FAKE_MIN;
		}
	    } //end  numstats loop
	    // Count is 0
	    colorStats[i][NUMSTATS - 1] = 0;
	}//end num colors loop
    }//end function
    
    /** Runs through the color table and counts up where the colors 
     *are and how many there are.
     */
    public void collectStats() {
	initStats();
        
        for(int y=0; y<yMax; y++) {
            for(int u=0; u<uMax; u++) {
                for(int v=0; v<vMax; v++){
		    updateColor(y, u, v,
				colorTable.getRawColor(y,u,v));
                }//v
	    }//u
	}//y

        if (allView) {
            setText("Total num entries: "+colorStats[GLOBAL_INDEX][6]);
        }
        else {
            setText("Total num entries for color: " + colorStats[curColor][6]);
        }
    }


    /**
     * Updates the stats about the color and its yuv values.
     * If one of the YUV values is either a min or max for that color,
     * the stats array is updated accordingly.  Otherwise we just increase
     * the count of that color.
     * @precondition 0 <= color < NUM_COLORS
     */
    public void updateColor(int y, int u, int v, byte color) {
	colorStats[color][Y_MIN] = Math.min(colorStats[color][Y_MIN], y); 
	colorStats[color][Y_MAX] = Math.max(colorStats[color][Y_MAX], y); 
	colorStats[color][U_MIN] = Math.min(colorStats[color][U_MIN], u); 
	colorStats[color][U_MAX] = Math.max(colorStats[color][U_MAX], u); 
	colorStats[color][V_MIN] = Math.min(colorStats[color][V_MIN], v); 
	colorStats[color][V_MAX] = Math.max(colorStats[color][V_MAX], v); 
	colorStats[color][NUM_ENTRIES]++;

        // We don't want unthresholded values to affect the global table.
        if (color == GREY) { return; }

        // Increase global color table stats
        colorStats[GLOBAL_INDEX][Y_MIN] = 
            Math.min(colorStats[GLOBAL_INDEX][Y_MIN], y); 
	colorStats[GLOBAL_INDEX][Y_MAX] = 
            Math.max(colorStats[GLOBAL_INDEX][Y_MAX], y); 
	colorStats[GLOBAL_INDEX][U_MIN] = 
            Math.min(colorStats[GLOBAL_INDEX][U_MIN], u); 
	colorStats[GLOBAL_INDEX][U_MAX] = 
            Math.max(colorStats[GLOBAL_INDEX][U_MAX], u); 
	colorStats[GLOBAL_INDEX][V_MIN] = 
            Math.min(colorStats[GLOBAL_INDEX][V_MIN], v); 
	colorStats[GLOBAL_INDEX][V_MAX] = 
            Math.max(colorStats[GLOBAL_INDEX][V_MAX], v); 
	// Increase the number of times color's been seen
	colorStats[GLOBAL_INDEX][NUM_ENTRIES] ++;

    }
    
    
    public boolean getEditing() {
        return editing;
    }

    public View getView() {
        return view;
    }

    /** Turns editing on and off.  When editing is on then the color
     *table can be directly changed.
     */
    public void setEditing(boolean choice) {
	editing = choice;
        imagePanel.setDrawing(choice);
    }

    /** Displays string s on screen in echo area */
    public void setText(String s) {
        buttonPanel.setText(s);
    }


     /** Picks up a value from the slider to determine which "slice" of the 
     * color table to display.
     * @parameter v - the value of the slider.
     */
    public void setValue(int v) {
	sliderValue = v;
        slice();
	repaint();
    }


    /**
     * Converts the x y coordinates of the picture into
     * an ordered pair of YUV type values, then edits the table entry
     * at that location to color.
     */
    public void changePixel(int x, int y, byte color) {

        ColorTableUpdate change = new ColorTableUpdate(color);
        int Y = 0, U = 0, V = 0;
        
        switch (view) {
        case YVIEW:
            // x coord is V, y coord is U
            Y = sliderValue;
            V = x;
            U = y;
            break;
        case UVIEW:
            // x coord is Y, y coord is V
            U = sliderValue;
            Y = x;
            V = y;
            break;
        case VVIEW:
            // x coord is Y, y coord is U
            V = sliderValue;
            Y = x;
            U = y;
            break;
        }
                
        int[] coord = {Y<<yShift,V<<vShift,U<<uShift};
        byte oldColor = colorTable.getColor(Y<<yShift,V<<vShift,U<<uShift);
        
        change.addPixel(coord, oldColor);
        colorTable.modifyTable(change);        
        
        // Make sure we can undo the change we're about to make
        colorTable.pushUndo(change);
        
        slice();
        repaint();

        // Alert all color table listeners that the color table has changed
        tool.getDataManager().notifyColorTableDependants(colorTable,
                                                         change,
                                                         this);
    }

    /** Convenience method that changes two points into (x,y) pairs and then
     * passes them off into the real rectEdit function) */
    public void rectEdit(Point first, Point second, byte color) {
        int x1, x2, y1, y2;
        x1 = (int) first.getX();
        y1 = (int) first.getY();

        x2 = (int) second.getX();
        y2 = (int) second.getY();

        rectEdit(x1,y1,x2,y2,color);
    }

    /**
     * This method converts (x,y) image coordinates into coordinates
     * in the YUV space and then fills in a rectangle with the selected color.
     * Note that these are not the (x,y) coordinates of the mouse that are
     * obtained with MouseEvent e.getX(); they must first be scaled to the
     * buffImage's coordinate system.  (e.g. even though on our screen the
     * image takes up 400 pixels in the x direction, it might actually only have
     * 70 pixels of information that have been scaled to fit.)
     *
     * The order of the x1,y1,x2,y2 are unimportant in the sense that
     * this method determines which is the upper left and which is the lower
     * right; it is unnecessary to do so prior to this method.
     *
     * @param x1 the x location of the upper left or lower right corner of rect
     * @param y1 the y location of the upper left or lower right corner of rect
     * @param x2  "  x     "    "    "
     * @param y2  "  y     "
     * @param color the color to enter into the color table 
     */
    public void rectEdit(int x1, int y1, int x2, int y2, byte color) {

        ColorTableUpdate change = new ColorTableUpdate(color);
        int yStart, yEnd, uStart, uEnd, vStart, vEnd; 
        yStart = yEnd = uStart = uEnd = vStart = vEnd = 0;

        // Determine which plane we are working in based on the view, and hence
        // whether the x locations correspond to the Y scale, for instance, or 
        // the U
        switch (view) {
        case YVIEW:
            yStart = yEnd = sliderValue;
            // x coord is V, y coord is U
            vStart = Math.min(x1, x2);
            vEnd = Math.max(x1, x2);
            
            uStart = Math.min(y1, y2);
            uEnd = Math.max(y1, y2);
            break;
        case UVIEW:
            uStart = uEnd = sliderValue;

            // x coord is Y, y coord is V
            yStart = Math.min(x1, x2);
            yEnd = Math.max(x1, x2);

            vStart = Math.min(y1, y2);
            vEnd = Math.max(y1, y2);
            break;
        case VVIEW:
            vStart = vEnd = sliderValue;

            // x coord is Y, y coord is U
            yStart = Math.min(x1, x2);
            yEnd = Math.max(x1, x2);

            uStart = Math.min(y1, y2);
            uEnd = Math.max(y1, y2);
            break;
        }

       
        for (int Y = yStart; Y<=yEnd; Y++) {
            for (int U = uStart; U <= uEnd; U++) {
                for (int V = vStart; V <= vEnd; V++) {
                    // Note that the order is different due to the way the Aibo
                    // architecture.  
                    int[] coord = {Y<<yShift,V<<vShift,U<<uShift};
                    byte oldColor = colorTable.getColor(Y<<yShift, V<<vShift,
                                                        U<<uShift);
                    change.addPixel(coord, oldColor);
                } // end V
            } // end U
        }// end Y

        colorTable.pushUndo(change);
        // makes all the cumulative changes to the color table
        colorTable.modifyTable(change);

        // figure out what needs to be colored..
        slice();
        repaint();

        // Alert all color table listeners that the color table has changed
        tool.getDataManager().notifyColorTableDependants(colorTable,
                                                         change,
                                                         this);
    }


    /** Mouse wheel listener methods */
    public void mouseWheelMoved(MouseWheelEvent e){
	
	
	if(e.isShiftDown()){
            // Change colors modulo the number of colors so that we wrap around
            // the end.  Note that we add NUM_COLORS so that in the event we
            // have a negative number, the modulo comes out correctly
            setColor((byte) ((curColor + e.getWheelRotation() + NUM_COLORS)
                             % NUM_COLORS));
        }
        // Move the slider
	else {
            if (e.getWheelRotation() < 0) {
                buttonPanel.pressPlus();
            }
            else {
                buttonPanel.pressMinus();
            }
	}
    }




   

    /** TODO: paint axes */
    public void paint(Graphics g) {
       
        super.paint(g);
        /*Graphics2D g2d = (Graphics2D)g;
        if (view != View.COLORVIEW) {
            g2d.drawImage(overlay, imagePanel.getImageOp(), 0, 0);
            drawAxes(g2d, xAxisMin<<yShift, xAxisMax<<yShift, yAxisMin<<yShift, yAxisMax<<yShift);
            }*/
        buttonPanel.repaint();
        
    }

    
    public void drawAxes(Graphics2D g, int xMin, int xMax, int yMin, int yMax) {
        String xAxisLabel = "";
        String yAxisLabel = "";
        
        switch (view) {
        case YVIEW:
            // x coord is V, y coord is U
            xAxisLabel = "V";
            yAxisLabel = "U";
            break;
        case UVIEW:
            // x coord is Y, y coord is V
            xAxisLabel = "Y";
            yAxisLabel = "V";
            break;
        case VVIEW:
            // x coord is Y, y coord is U
            xAxisLabel = "Y";
            yAxisLabel = "U";
            break;
        }
        
        g.drawString(xAxisLabel + "(" + xMin + ", " + xMax + ")", 0, 400);
        g.drawString(yAxisLabel + "(" + yMin + ", " + yMax + ")",400, 200);

    }

		
    /**
     * Sets the view to newView.  By using an 
     * enum type, we guarantee we won't get bad
     * input.  Basically changes what the ColorEdit
     * will show - either a slice of the table or the whole thing.
     */
    public void setView(View newView) {
	view = newView;
       
        if (newView == View.COLORVIEW) {
            overlay = null;
            imagePanel.setColorTableOverlay(overlay);
            // We may have made changes to color table since last time
            collectStats();
            colorizeImage();
        }
        else {
            fixSliderAxes();
            slice();
        }
    }
    
    
    /**
     * Whenever we change modes or colors, it's necessary to rescale the
     * slider so that it only ranges between valid entries in the table.
     */
    public void fixSliderAxes() {
        // If we're in global view, we scale based on global min maxes, not
        // local to the color we're on
        int colorAxis = (allView ? GLOBAL_INDEX : curColor);
        int sliderMin = 0; int sliderMax = 0;

        switch (view) {
        case YVIEW:
            sliderMin = colorStats[colorAxis][Y_MIN];
            sliderMax = colorStats[colorAxis][Y_MAX];
            break;
        case UVIEW:
            sliderMin = colorStats[colorAxis][U_MIN];
            sliderMax = colorStats[colorAxis][U_MAX];
            break;
        case VVIEW:
            sliderMin = colorStats[colorAxis][V_MIN];
            sliderMax = colorStats[colorAxis][V_MAX];
        }
        if (sliderMin == FAKE_MIN || sliderMax == FAKE_MAX) {
            sliderMin = sliderMax = 0;
        }

        buttonPanel.setRange(sliderMin, sliderMax);
    }


    /**
     * Displays a slice through the color table; e.g. if we are in Y view,
     * and our value is set to 53, we fix Y = 53 and let U and V range for
     * all values that they can take on in our current color table.
     *
     * Two different things are rendered.  The first is the background color,
     * or simply the YUV -> RGB representation of what color we're looking at
     * in the color table.  The x and y axes of our image depend on which view
     * we are in; if we are in YView then our x axis becomes the V axis, and our
     * picture's y axis becomes the color U axis.
     *
     * On top of the background colors we render the thresholded colors.  That
     * is to say, if we are in Y View at value = 58, and in our color table
     * something at Y = 58, U = 28, V = 125 is say, Yellow, then we draw a 
     * yellow pixel at that point.
     *
     * If allView is on, then will scale to show all entries.  Otherwise, 
     * this method changes what you look at based on what color you
     * have selected.  If you have green selected, for instance, the YUV
     * values will be scaled such that you almost always see green thresholded
     * entries on the screen.
     */
    public void slice() {

        // If we are in allView, we don't care about the current color
        // and instead scale based on the global color table.  Otherwise
        // we restrict our view to the color.
        int axesScalingColor;
        if (allView) {
            axesScalingColor = GLOBAL_INDEX;
        }
        else {
            axesScalingColor = curColorForSlice;
        }

        // Used to avoid iterating over needless, empty values in color table.
        int yMin = colorStats[axesScalingColor][Y_MIN]; 
	int yMax = colorStats[axesScalingColor][Y_MAX];
	int uMin = colorStats[axesScalingColor][U_MIN];
	int uMax = colorStats[axesScalingColor][U_MAX];
	int vMin = colorStats[axesScalingColor][V_MIN];
	int vMax = colorStats[axesScalingColor][V_MAX];
        
	int ySize = Math.abs(yMax - yMin + 1);
	int uSize = Math.abs(uMax - uMin + 1);
	int vSize = Math.abs(vMax - vMin + 1);

        // Size the swatch based on min and max.  in YVIEW, u and v vary, and
        // so on and so forth.  Also keep track of our min and max on the 
        // axes so that we can display a scale correctly.
        int dim1 = 0; int dim2 = 0;
        
        switch (view) {
        case YVIEW:
            // x coord is V, y coord is U
            dim1 = vSize;
            xAxisMin = vMin;
            xAxisMax = vMax;
            
            dim2 = uSize;
            yAxisMin = uMin;
            yAxisMax = uMax;
            
            break;
        case UVIEW:
            // x coord is Y, y coord is V
            dim1 = ySize;
            xAxisMin = yMin;
            xAxisMax = yMax;

            dim2 = vSize;
            yAxisMin = vMin;
            yAxisMax = vMax;
            break;
        case VVIEW:
            // x coord is Y, y coord is U
            dim1 = ySize;
            xAxisMin = yMin;
            xAxisMax = yMax;

            dim2 = uSize;
            yAxisMin = uMin;
            yAxisMax = uMax;
            break;
            
        }

        // Create the swatch of the correct size to hold all entries, but only
        // if we've changed size from last time or we don't already have
        // an image; otherwise we'll just repaint over the old one
        if (buffImage == null || (buffImage.getWidth() != dim1 ||
                                  buffImage.getHeight() != dim2)) {
            buffImage = new BufferedImage(dim1, dim2, 
                                          BufferedImage.TYPE_INT_ARGB);
        }
        if (overlay == null || (overlay.getWidth() != dim1 ||
                                overlay.getHeight() != dim2)) {
            overlay = new BufferedImage(dim1, dim2, 
                                        BufferedImage.TYPE_INT_ARGB);
        }
        
        imagePanel.setImage(buffImage);
        imagePanel.setColorTableOverlay(overlay);

        /** Check to see if we even need to draw anything */
        
        if (noThresholdedValuesExist(view, sliderValue)) {
            Graphics2D g2 = buffImage.createGraphics();
            Color startColor = Color.WHITE;
            Color endColor = byteToColor(curColorForSlice);
            GradientPaint colorGradient = 
                new GradientPaint(0, buffImage.getHeight() / 2,
                                  startColor,
                                  buffImage.getWidth(),
                                  buffImage.getHeight() / 2,
                                  endColor);
            g2.setPaint(colorGradient);
            g2.fillRect(0,0, buffImage.getWidth(), buffImage.getHeight());
            // Standard method of calculating a contrasting color.
            Color contrastingColor = new Color(endColor.getRGB() ^ 0xFFFFFF);
            g2.setColor(contrastingColor);
            g2.drawString("No entries exist yet for this value.",
                          buffImage.getWidth() / 2,
                          buffImage.getHeight() / 2);
            //System.out.println("No entries exist yet for this value.");

            return;
        }


	for (int u = uMin; u <= uMax; u++) {
	    for (int v = vMin; v <= vMax; v++) {
		for (int y = yMin; y <= yMax; y++) {
              
		    // value = the int value specified by the slider.
		    // We're slicing at a given YUV value, so we need to display
		    // things if we're at the corresponding Y, U, or V value..
		    if ((view == View.YVIEW && y == sliderValue) ||
			(view == View.UVIEW && u == sliderValue) || 
			(view == View.VVIEW && v == sliderValue)) {

                        boolean classified = true;

                        byte tableColor = colorTable.getRawColor(y,u,v);
                        if (tableColor == GREY) { classified = false; }
                                         
                        // Note order: Y,V,U not YUV
                        int color = converter.toRGB(y<<yShift, 
                                                    v<<vShift,
                                                    u<<uShift).getRGB();
			// foreground color will be drawn on top of the 
                        // background gradient if we've thresholded the 
                        // YUV value.
                        int foregroundColor;
                        if (classified) {
                            foregroundColor = byteToColor(tableColor).getRGB();
                        }
                        else { // transparent
                            foregroundColor = 0x0;
                        }

                        // x and y locations on the buffered image
                        int xLoc, yLoc;
                        xLoc = yLoc = 0;

                        // Fills in the background colors and the thresholded
                        // colors if necessary
                        switch (view) {
                            // Subtract the minimums to ensure that we stay
                            // within the bounds [0, dimMax) when we set pixels.
                        case YVIEW:
                            xLoc = v-vMin; yLoc = u-uMin;
                            break;
                        case UVIEW:
                            xLoc = y-yMin; yLoc = v-vMin;
                            break;
                        case VVIEW:
                            xLoc = y-yMin; yLoc = u-uMin;
                            break;
                        }
                        // Fill in background and foreground .
                        buffImage.setRGB(xLoc, yLoc, color);
                        overlay.setRGB(xLoc, yLoc, foregroundColor);
		    }// end view check
		}// end y loop
	    }// end v loop
	}// end u loop



    }


    /**
     * Instead of slicing the table in terms of YUV values, this method
     * paints every color that has been thresholded as curColor.  For instance,
     * if we have thresholded 500 different shades of blue pixels as BLUE,
     * then if curColor is blue and this method is called, all 500 different
     * shades of blue will be drawn on the screen.
     */
    public void colorizeImage() {

       	int yMin = colorStats[curColor][Y_MIN]; 
	int yMax = colorStats[curColor][Y_MAX];
	int uMin = colorStats[curColor][U_MIN];
	int uMax = colorStats[curColor][U_MAX];
	int vMin = colorStats[curColor][V_MIN];
	int vMax = colorStats[curColor][V_MAX]; 

	int numEntries = colorStats[curColor][NUM_ENTRIES];
	if (numEntries == 0) {
	    setText("No values have been thresholded that color");
	    return;
	}
	
	setText("Stats:\n y: [" + yMin + ", " + yMax + "]\n" +
			   "u: [" + uMin + ", " + uMax + "]\n" +
			   "v: [" + vMin + ", " + vMax + "]\n" +
			   "Total entries: " + numEntries);

        
	int numPixelsPerSide = (int) Math.sqrt(numEntries);
	
        buffImage = new BufferedImage(numPixelsPerSide, numPixelsPerSide,
                                      BufferedImage.TYPE_INT_ARGB); 
        imagePanel.setImage(buffImage);

	int counter = 0;
        int NUM_PIXELS_PER_LINE = Math.min(numPixelsPerSide, DEFAULT_MAX);

	for (int u = uMin; u <= uMax; u++) {
	    for (int v = vMin; v <= vMax; v++) {
		for (int y = yMin; y <= yMax; y++) {
                    
                    
		    if (colorTable.getRawColor(y,u,v) == curColor) {

                        // don't want array out of bounds
                        if (counter == NUM_PIXELS_PER_LINE * 
                            NUM_PIXELS_PER_LINE) { 
                            return; }
                        
                        // Note the order: it's different than YUV due to 
                        // aibo's architecture for some reason.
                        int color = converter.toRGB(y<<yShift,
                                                    v<<vShift,
                                                    u<<uShift).getRGB();
                        
                        // draw the pixel in image array here
                        buffImage.setRGB(counter % NUM_PIXELS_PER_LINE,
                                         counter / NUM_PIXELS_PER_LINE,
                                         color);
                        counter++;
		    } // end curColor
		} // end y
            }// end v
	} // end u
    }// end function


    
    /**
     * @return the color at index n if it's a valid index into our colors
     * array, else null
     */
    public Color byteToColor(byte n) {
        if (n > Vision.COLORS.length || n < 0) {
            System.out.println("Error, color out of range");
            return null;
        }
        return Vision.COLORS[n];

    }

    ////////////////////////////////////////////////////////////
    // LISTENER METHODS
    ////////////////////////////////////////////////////////////

    


    public void colorTableChanged(ColorTable source, ColorTableUpdate update,
                                  ColorTableListener originator) {
        // We don't want to deal with actions that we ourselves propagated;
        // should already be handled by the time this method is evoked
        if (originator == this) { return; }
        
        // Only refresh the screen if it will be visible, otherwise don't waste
        // the cpu cycles
        if (!isShowing()) {
            return;
        }
        // TODO: Make it use the colortable update information so we don't have
        // to repaint / recalculate everything.

        // Change our slider based on the average y, u, or v value if we're in
        // the same color as they just thresholded.  This allows our view
        // to change along with the clicks
        /*        
        if (update.getColor() == curColorForSlice) {
            switch (view) {
            case YVIEW:
                sliderValue = update.getAverageYChannel(curColorForSlice);
                break;
            case UVIEW:
                sliderValue = update.getAverageUChannel(curColorForSlice);
                break;
            case VVIEW:
                sliderValue = update.getAverageVChannel(curColorForSlice);
                break;
            default:
                break;
            }
        }
        buttonPanel.setSliderValue(sliderValue);
        System.out.println("slider value is : " + sliderValue);
        */
        refresh();
        
    }
    
     /** Mouse listener methods */
    public void mouseClicked(MouseEvent e) {}

    public void mouseEntered(MouseEvent e) {}

    public void mouseExited(MouseEvent e) {}


    

    /**
     * If we're in edit mode, this method figures out the coordinates of
     * the mouse click based on picture coordinates rather than screen 
     * coordinates.
     */
    public void mousePressed(MouseEvent e) {
        // Only make changes if we're in edit mode
        if (!editing || view == View.COLORVIEW) { return; }

        // furthermore, only make changes if within bounds of picture
        int x = imagePanel.getImageX((int) e.getX()); 
        int y = imagePanel.getImageY((int) e.getY());
        
        // out of the picture bounds
        if (x == -1 || y == -1) { return; } 

        // We need to scale it such that x = 0 (first pixel) corresponds to
        // x = xAxisMin (since we often range from some nonzero number)
        x += xAxisMin;
        y += yAxisMin;

        startLoc = new Point(x, y);
       }

    
    public void mouseReleased(MouseEvent e) {
        if (!editing || view == View.COLORVIEW) { return; }

        int x = imagePanel.getImageX((int) e.getX());
        int y = imagePanel.getImageY((int) e.getY());
        
        if (x == -1 || y == -1 || startLoc == null) { return; }
        
        // We need to scale it such that x = 0 (first pixel) corresponds to
        // x = xAxisMin (since we often range from some nonzero number)
        x += xAxisMin;
        y += yAxisMin;

        endLoc = new Point(x, y);
        
        // Single pixel change!
        if (startLoc.equals(endLoc)) {
            changePixel(x, y, curColor);
        }
        // Rect edit!
        else {
            rectEdit(startLoc, endLoc, curColor);
        }
    }

    /** return true if and only if we have no entries in the table for the given
     * slice of the table and value.  e.g. if we're in Y view and the curvalue
     * is 128, return true if our table only has undefined values for a Y
     * value of 128.
     */
    public boolean noThresholdedValuesExist(View view, int curValue) {
        switch (view) {
        case YVIEW:/*
            System.out.println("slider value: " + curValue);
            System.out.println("min color stat: " +colorStats[GLOBAL_INDEX][Y_MIN]);
            System.out.println("max color stat: " + colorStats[GLOBAL_INDEX][Y_MAX] );*/
            return colorStats[GLOBAL_INDEX][Y_MIN] > curValue ||
                colorStats[GLOBAL_INDEX][Y_MAX] < curValue;
        case UVIEW:
            return colorStats[GLOBAL_INDEX][U_MIN] > curValue ||
                colorStats[GLOBAL_INDEX][U_MAX] < curValue;
        case VVIEW:
            return colorStats[GLOBAL_INDEX][V_MIN] > curValue ||
                colorStats[GLOBAL_INDEX][V_MAX] < curValue;
        default:
            return false;
        }
    }


}
