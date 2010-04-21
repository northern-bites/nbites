package TOOL.Calibrate;

import javax.swing.JRadioButton;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Dimension;
import java.awt.AlphaComposite;
import java.awt.BasicStroke;
import java.awt.GradientPaint;


/**
 * A simple button class to allow user to click on a color in order
 * to select it, similar to how MS Paint works.  The button is 
 * an extension of a JRadioButton, meaning it is a toggle button
 * rather than a normal JButton.  The button is drawn differently
 * depending on whether it is selected or not.  If it is selected,
 * it is drawn at full opacity in order to stand out, and a border is
 * drawn around it.  When unselected, the button is semi-transparent
 * and has no border drawn around it.
 * The label is drawn in full opacity on top of the button.
 * This class should always be used in conjunction with a ColorSwatchPanel
 * so that multiple ColorSwatches are linked together and have access
 * to the Calibrate module
 * @author Nicholas Dunn 
 * @version 1.0 11/18/2007
 */

public class ColorSwatch extends JRadioButton {

    private int size;
    private Color color, borderColor, textColor;
    private String label;
    private boolean crossedOut = false;
   
    private static final int VERY_DARK_THRESH = 200;
    private static final int VERY_LIGHT_THRESH = 400;
    // 0.0 = completely transparent, 1.0 = completely opaque.
    private static final float TRANSPARENCY_LEVEL = .6f;
    private static final int TEXT_SPACING = 5;


    /**
     * Constructs a ColorSwatch of width and height <code>size</code>.
     * Determines whether the color is dark enough to warrant a special
     * border color or light enough to have a special text color for
     * the label.
     * @param size the width and height of the box, in pixels
     * @param text the String to draw inside of the box
     * @param toolTip the String to display within the toolTip
     * @param color the Color to draw in the box.
d     */
    public ColorSwatch(int size, String text, String toolTip, Color color) {
	super();
	this.size = size;
	this.color = color;
	label = text;

	int rgbTotal = color.getRed() + color.getGreen() + color.getBlue();
	// Change the border and text based on how dark or light swatch is
	borderColor = (rgbTotal < VERY_DARK_THRESH ? Color.RED : Color.BLACK);
	textColor = (rgbTotal > VERY_LIGHT_THRESH ? Color.BLACK : Color.WHITE);

	setPreferredSize(new Dimension(size, size));
	setSize(new Dimension(size, size));

	// Don't consume focus so that keyboard can do it
	setFocusable(false);

	setToolTipText(toolTip);

    }

    /** @return label */
    public String getLabelString() { return label; }


    public void setCrossedOut(boolean choice) {
	crossedOut = choice;
    }


    /**
     *Overrides default drawing of a JRadioButton to reflect the
     * size and shape of a ColorSwatch (a rectangle of width and height
     * <code>size</code>).  An unselected button is drawn as semi-transparent,
     * while a selected button is drawn fully opaque and outlined in 
     * <code>borderColor</code>
     * @param g the Graphics context in which the ColorSwatch is being drawn.
     */
    public void paint(Graphics g) {
	
	Graphics2D g2d = (Graphics2D) g;
	g2d.setColor(color);
	
       
	if (isSelected()) {
	    // Draw a transition to Gray to indicate we're undefining that color
	    if (crossedOut) {
		Color startColor = color;
		Color endColor = Color.GRAY;

		GradientPaint gradient = new GradientPaint(0, size/2, startColor,
						       2*size/3, size/2,
						       endColor);
		g2d.setPaint(gradient);

	    }



	    g2d.fillRect(0,0,size,size);
	    
	    g2d.setColor(borderColor);
	    // Draw a border two pixels wide 
	    g2d.setStroke(new BasicStroke(2));
	    g2d.drawRect(0,0,size-1,size-1);
	}
	else {
	    // Necessary to clear the rect so the transparencies don't
	    // pile up over each other.
	    g2d.clearRect(0,0,size,size);
	    // Draw unselected as slightly transparent
	    g2d.setComposite(AlphaComposite.getInstance(
			     AlphaComposite.SRC_OVER, TRANSPARENCY_LEVEL));
	    g2d.fillRect(0,0,size, size);
	}

	

	
	// Make sure the numbers are completely opaque
	g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 1.0f));
	g2d.setColor(textColor);
	g2d.drawString(label, TEXT_SPACING, size-TEXT_SPACING);
	g2d.dispose();
    }
}
