package edu.bowdoin.robocup.TOOL.Calibrate;

import java.awt.Cursor;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Toolkit;
import java.awt.Point;
import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.AlphaComposite;
import java.awt.GradientPaint;

import edu.bowdoin.robocup.TOOL.TOOL;


public class CalibrationCursor {
    
    public static final float TRANSPARENCY_LEVEL = .6f;
    public static final float OPAQUE = 1.0f;
    public static final int MIN_SIZE = 3;
    
    /**
     * Creates a cursor that is a box used for thresholding multiple
     * pixels at once.  Most of the box is drawn translucent
     * in the color of currentColor.  The outline and the crosshair within
     * the box are drawn in a contrasting color which helps it stand out
     * in a highly thresholded image.
     * Transparency stuff from
     * http://www.ibm.com/developerworks/library/j-begjava/index.html
     * Contrasting color code from 
     * http://www.codeproject.com/tips/JbColorContrast.asp?df=100&
     * forumid=39709&exp=0&select=812333
     * @param color the standard color to fill the box with, from which
     *        contrasting color will be calculated.
     * @param size the width of the box in pixels
     * @param gradient if true, will draw a gradient from color to gray
     *
     */
    public static Cursor getCursor(Color color, int size, 
                                   boolean gradient) {
        int realSize = Math.max(MIN_SIZE, size);
        BufferedImage swatch = new BufferedImage(realSize, realSize, 
						 BufferedImage.TYPE_INT_ARGB);
	Graphics2D g2d = swatch.createGraphics();

	// Add a gradient to show we're going to gray
	if (gradient) {
	    Color startColor = color;
	    Color endColor = Color.GRAY;
	    GradientPaint colorGradient = 
                new GradientPaint(0, realSize/2, startColor, 2*realSize/3, 
                                                            realSize/2,
                                                            endColor);
	    g2d.setPaint(colorGradient);
	 
	}
	else {
	    // Normal solid color
	    g2d.setColor(color);
	}
	// Make semi transparent for the box
	g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER,
						    TRANSPARENCY_LEVEL));
	g2d.fillRect(0,0,realSize,realSize);
	
	// Draw the contrasting parts, namely the crosshair and the border
	int contrastingColor;
	if (color == Color.GRAY) {
	    contrastingColor = Color.WHITE.getRGB();
	}
	else {
	    // Standard method of calculating a contrasting color.
	     contrastingColor = color.getRGB() ^ 0xFFFFFF;
	}
	g2d.setColor(new Color(contrastingColor));
	// Make completely opaque
	g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 
						    OPAQUE));
	
	// Draw the crosshair and outline
	g2d.drawLine(realSize/3, realSize/2, 2*realSize/3, realSize/2);
	g2d.drawLine(realSize/2, realSize/3, realSize/2, 2*realSize/3);
	g2d.drawRect(0,0,realSize-1,realSize-1);

	// Set the middle of cursor to be where a click registers
	Point cursorHotPoint = new Point(realSize/2, realSize/2);

        Toolkit toolkit;
        if (TOOL.instance == null || TOOL.instance.getFrame() == null)
            toolkit = Toolkit.getDefaultToolkit();
        else
            toolkit = TOOL.instance.getFrame().getToolkit();
	Cursor customCursor = toolkit.createCustomCursor(swatch, cursorHotPoint,
							 "CalibrationCursor");
	return customCursor;
    }

    

}
