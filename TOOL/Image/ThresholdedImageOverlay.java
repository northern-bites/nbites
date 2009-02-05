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
 * This class is used to draw the shapes of objects recognized + other information on
 * the thresholded image panel (display panel) in the calibration part of the tool
 * 
 * Inspired by ImageOverlay, but just a very basic version 
 * @author Octavian Neamtu 2009
 */
public class ThresholdedImageOverlay extends BufferedImage{
    private static final Color[] COLORS = Vision.COLORS;

    private int width, height;

    /**
     * Constructs an ThresholdedImageOverlay of width and height as specified by
     * creating a buffered image of type ARGB (alpha channel specified)
     * and initializes the array of pixels to be TRANSPARENT to start.
     * @param width number of pixels wide the image will be
     * @param height number of pixels high
     */ 
    public ThresholdedImageOverlay(int width, int height){
	
	super(width, height, BufferedImage.TYPE_INT_ARGB);

	this.width = width;
	this.height = height;
	resetPixels();	
    }

    
    /**
     * sets the individual pixels of the buffered image to be 
     * literally transparent
     */
    public void resetPixels(){
	for(int w = 0; w < width; w++){
	    for(int h = 0; h < height; h++){
		setRGB(w,h,0x0);
	    }
	}
    }

    public void setRectOverlay(int x, int y, int w, int h, int thickness, byte color){
	Graphics2D g = createGraphics();
	
	g.setColor(new Color(COLORS[color].getRGB()));
	for (int k = 1; k <= thickness; k++) 
	    g.drawRect(x-k, y-k, w+2*k, h+2*k);
    }

}
