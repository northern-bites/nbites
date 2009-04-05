package TOOL.Image;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.image.BufferedImage;

import java.awt.AlphaComposite;
import java.util.Iterator;

import TOOL.TOOL;
import TOOL.Calibrate.Pair;
import TOOL.Vision.Vision;


/**
 * This class draws edge lines on the raw image, as well as the swatches of
 * color that the user paints in order to define (threshold) the YUV values
 * as corresponding to a given color. We maintain a separate int array 
 * for the values at each pixel, so that, for instance, we can differentiate
 * between the black of an edge and the black of a thresholded part of the 
 * image.
 */
public class ImageOverlay extends BufferedImage{
    private static final Color[] COLORS = Vision.COLORS;
    private static final byte TRANSPARENT = -1;
    // Will be drawn black.
    private static final byte EDGE = (byte) COLORS.length; 
    public static final int DEFAULT_THRESH = 20;
    public static final int MIN_THRESH = 5;
    public static final int MAX_THRESH = 50;

    protected boolean thresholding;

    int width, height, edgeThresh;
    byte[][] pixels;


    /**
     * Constructs an ImageOverlay of width and height as specified by
     * creating a buffered image of type ARGB (alpha channel specified)
     * and initializes the array of pixels to be TRANSPARENT to start.
     * @param width number of pixels wide the image will be
     * @param height number of pixels high
     */ 
    public ImageOverlay(int width, int height){
	
	super(width, height, BufferedImage.TYPE_INT_ARGB);

	this.width = width;
	this.height = height;
	edgeThresh = DEFAULT_THRESH;
	thresholding = true;
	resetPixels();	
    }

    
    public void setThresholding(boolean choice) {
        thresholding = choice;
    }


    /**
     * Sets all of the entries in int array to be TRANSPARENT
     * and sets the individual pixels of the buffered image to be 
     * literally transparent
     */
    public void resetPixels(){
	pixels  = new byte[width][height];
	for(int w = 0; w < width; w++){
	    for(int h = 0; h < height; h++){
		pixels[w][h] = TRANSPARENT;
		setRGB(w,h,0x0);
	    }
	}
    }


    /**
     * Searches all the pixels in the image for those thresholded as
     * EDGE and sets them to TRANSPARENT.  Also literally changes the
     * BufferedImage to make those pixels clear rather than black.
     * This method exists so that when the threshold value is modified
     * and we wish to recalculate the edge image, we can maintain all those
     * parts of the overlay that are NOT edge, i.e. all the things the user
     * manually thresholded
     */
    public void resetEdgePixels() {	
	for(int w = 0; w < width; w++){
	    for(int h = 0; h < height; h++){
		if (pixels[w][h] == EDGE) {
		    pixels[w][h] = TRANSPARENT;
		    setRGB(w,h,0x0);
		}
	    }
	}
    }

   
    /**
     * Changes the threshold value for defining what constitutes an edge.
     * Since this also affects whether a pixel is too far from another's
     * value to be considered the same shade (and thus stops the recursion
     * during a recursiveCalibrate call, having a high threshold value
     * will result in having less of the swatch under your square actually
     * filled in on each click.  
     */
    public void setEdgeThresh(int edgeThresh) {
	this.edgeThresh = edgeThresh;	
    }
    
    /**
     * Resets all the pixels in the image to be transparent, then 
     * generates the edges and defines them to be black.  Should be called
     * when a new image is being drawn and a new ImageOverlay needs to be drawn.
     */
    public void generateNewEdgeImage(TOOLImage rawImage) {
	resetPixels();
	generateEdgeImage(rawImage);
    }

    /**
     * Resets all the edge pixels to be transparent, then
     * generates the edges and defines them to be black.
     * Should be used when the edge threshold has been changed and 
     * you want to redraw just the edges
     */
    public void regenerateEdgeImage(TOOLImage rawImage) {
	resetEdgePixels();
	generateEdgeImage(rawImage);
    }

    /**
     * Heavily based on the edge image generater from JavaConnect
     * Does all the heavy lifting on drawing the edges on the image
     * and setting the entries in the int array to be EDGE
     */
    public void generateEdgeImage(TOOLImage rawImage){

        if (!thresholding) { return; }

	//check to make sure we are the same size as rawImage
	if(rawImage.getWidth() != width || 
	   rawImage.getHeight() != height){
            TOOL.CONSOLE.error("EdgeImage cannot be generated for"+
			       "an incompatible size");
	    return;
	}
	
	int[] lastPixel = new int[3];
	
	//scan each column VERTICALLY
	for(int w = 0; w < width; w++){
	    //set memory to top of each column
	    lastPixel = rawImage.getYCbCr(w,0);
	    
	    for(int h = 0; h < height; h++ ){
		//find the values for the current pixel
		int[] newPixel = rawImage.getYCbCr(w,h);
		
		//check to see if the jump is large enough
		if(isTransition(lastPixel,newPixel)){
		    setOverlay(w,h,EDGE);//then its an edge
		}
		//update the memory
		lastPixel = newPixel;
	    }
	}

	//scan each column HORIZONTALLY
	for(int h = 0; h < height; h++){
	    //set memory to top of each column
	    lastPixel = rawImage.getYCbCr(0,h);
	    
	    for(int w = 0; w < width; w++ ){
		//find the values for the current pixel
		int[] newPixel = rawImage.getYCbCr(w,h);
		
		//check to see if the jump is large enough
		if(isTransition(lastPixel,newPixel)){
		    setOverlay(w,h,EDGE);//then its an edge
		}
		//update the memory
		lastPixel = newPixel;
	    }
	}	
    }

    /**
     * Determines if two pixels represent a transition or not.
     * @param oldPixel an int array of length 3 representing a pixel
     * @param newPixel same format as oldPixel
     * @return true - if the pixels are a transition
     * @return false - otherwise
     */
    public boolean isTransition(int[] oldPixel, int[] newPixel){
        int threshold;
        if (thresholding) { threshold = edgeThresh;  }
        else { threshold = DEFAULT_THRESH; }
        

	if(oldPixel.length == newPixel.length)
	    return (Math.abs(oldPixel[0] - newPixel[0]) > edgeThresh ||
		    Math.abs(oldPixel[1] - newPixel[1]) > edgeThresh ||
		    Math.abs(oldPixel[2] - newPixel[2]) > edgeThresh);
	return false;
    }

   
		
    public boolean isDefined(int x, int y){

	return pixels[x][y] != TRANSPARENT;
    }

    /**
     * @return the currently thresholded value at position (x, y).
     * @precondition 0 <= x < width, 0 <= y < height
     */
    public byte getThreshColor(int x, int y) {
	return pixels[x][y];
    }
    
  

    public boolean isEdge(int x, int y){
	return pixels[x][y] == EDGE;
    }


    public boolean isOverlayColor(int x, int y, byte color) {
	return pixels[x][y] == color;
    }

    //draws a box
    /*public void setRectOverlay(int x, int y, int w, int h, int thickness, byte color){
	if ( w == 0 || h == 0) return;
	//horizontal lines
    	for (int k = 1; k <= thickness; k++)
	    for (int i = x; i <= x+w && i <= this.width; i++) {
		if (y-k > 0) setOverlay(i, y-k, color);
		if (y+h+k < height) setOverlay(i, y+h+k, color);
	    }
	//vertical lines
	for (int k = 1; k <= thickness; k++)
	    for (int i = y; i <= y+h && i <= this.height; i++) {
		if (x-k > 0) setOverlay(x-k, i, color);
		if (x+w+k > width) setOverlay(x+w+k, i, color);
	}
	}*/
    

    /**
     * Changes the pixels array to reflect the thresholded color and
     * sets the literal pixel in the BufferedImage to be that color.
     * @param x x-coord in picture (0 <= x < width)
     * @param y y-coord in picture (0 <= y < height)
     * @param color the thresholded value of the pixel at (x, y); must be
     * either EDGE, TRANSPARENT, or between 0 and COLORS.length - 1, 
     * inclusive.
     * 
     */
    public void setOverlay(int x, int y, byte color){
	pixels[x][y] = color;
	int rgbVal;

	if (color == TRANSPARENT) {
	    rgbVal = 0x0;
	}
	else if (color == EDGE) {
	    rgbVal = Color.black.getRGB();
	}
	else {
	    rgbVal = COLORS[color].getRGB();
	}
	setRGB(x, y, rgbVal);
	
    }

    

    /**
     * Takes in an ImageOverlayAction and iterates over it, changing
     * all of the entries as necessary.
     * Recall that an ImageOverlayAction consists of both (x,y) coords
     * and a pair of colors, the new one and the old one.  Basically we
     * take each (x,y) pair in turn and set the value at that location to
     * new color. Revert works in precisely the same way except that
     * we set the value to oldColor instead
     * 
     * @param toExecute the ImageOverlayAction, a collection of pairs of
     * (x,y) coords and pairs of (old, new) colors.
     */
    @SuppressWarnings("unchecked")
    public void execute(ImageOverlayAction toExecute) {
	
	if (toExecute == null) {
	    return;
	}

	Iterator coords = toExecute.getCoordinateIterator();
	Iterator theColors = toExecute.getColorsIterator();


	while (coords.hasNext()) {
	    
	    Pair <Integer, Integer> coord = (Pair <Integer, Integer>)
		coords.next();
	    int x = coord.getFirst();
	    int y = coord.getSecond();
	    Pair <Byte, Byte> theColorPair = 
		(Pair <Byte, Byte>) theColors.next();
	    byte newColor = theColorPair.getSecond();
	    setOverlay(x, y, newColor);
	}
    }

    /**
     * Undoes the ImageOverlayAction by iterating over all the changes
     * and reverting from the new color to the old color.
     * @param toUndo the ImageOverlayAction to undo.
     */
    @SuppressWarnings("unchecked")
    public void revert(ImageOverlayAction toUndo) {
	
	if (toUndo == null) {
	    return;
	}

	Iterator coords = toUndo.getCoordinateIterator();
	Iterator theColors = toUndo.getColorsIterator();
	while (coords.hasNext()) {
	    Pair <Integer, Integer> coord = (Pair <Integer, Integer>)
		coords.next();
	    int x = coord.getFirst();
	    int y = coord.getSecond();
	    Pair <Byte, Byte> theColorPair = 
		(Pair <Byte, Byte>) theColors.next();
	    byte oldColor = theColorPair.getFirst();
	    setOverlay(x, y, oldColor);

	}

    }
    

}
