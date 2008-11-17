package edu.bowdoin.robocup.TOOL.Calibrate;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.KeyListener;
import java.awt.event.KeyEvent;
import java.awt.Color;
import javax.swing.JPanel;
import javax.swing.JFrame;
import javax.swing.ButtonGroup;
import javax.swing.KeyStroke;
import javax.swing.AbstractAction;
import javax.swing.Action;

import java.awt.GridLayout;
import java.awt.Dimension;
import javax.swing.BorderFactory;
import java.util.HashMap;

import edu.bowdoin.robocup.TOOL.ColorEdit.ColorEditPanel;
import edu.bowdoin.robocup.TOOL.Vision.Vision;

/**
 * A simple class which holds individual ColorSwatches.  Each
 * ColorSwatch when clicked changes the currently selected color
 * to that of the ColorSwatch.  The ColorSwatchPanel combines
 * all of the constituent buttons into a ButtonGroup, meaning
 * at most one can be selected at a time. 
 * @author Nicholas Dunn
 * @version 1.0 11/18/2007
 */

public class ColorSwatchPanel extends JPanel implements ActionListener, KeyListener {

    private Color[] theColors = Vision.COLORS;
    private ColorSwatch[] theSwatches; 
    private ColorSwatchParent parent;
    
    public static final int BORDER_SIZE = 5;

    public ColorSwatchPanel(ColorSwatchParent parent, int size) {
	super();
	this.parent = parent;
	
	ButtonGroup a = new ButtonGroup();
        setLayout(new GridLayout(2, theColors.length/2));

        Dimension staticSize = new Dimension(size * theColors.length/2 + 
                                             2 * BORDER_SIZE,
                                             size * 2 + 2 * BORDER_SIZE);
        setMinimumSize(staticSize);
        setMaximumSize(staticSize);
        setPreferredSize(staticSize);

	theSwatches = new ColorSwatch[theColors.length];
	for (int i = 0; i < theColors.length; i++) {
	    // Set the text in the box to be 1 based indexing, but set the
	    // tool text based on the constant in Tool.Vision.Vision
	    theSwatches[i] = new ColorSwatch(size, ""+ (i+1), 
			     Vision.COLOR_STRINGS[i],
			     theColors[i]);
	    add(theSwatches[i]);
	    a.add(theSwatches[i]);
	    theSwatches[i].addActionListener(this);
	}
        
        setBorder(BorderFactory.createEmptyBorder(BORDER_SIZE, BORDER_SIZE,
                                                  BORDER_SIZE, BORDER_SIZE));
        
    }

    public void setCrossedOut(boolean choice) {
	for (int i = 0; i < theSwatches.length; i++) {
	    theSwatches[i].setCrossedOut(choice);
	}
    }

    /**
     * Sets the currently selected color in the Calibrate module to reflect
     * the color of the swatch user clicked on.
     */
    public void actionPerformed(ActionEvent e) {
        ColorSwatch picked = (ColorSwatch) e.getSource();

	byte j = (byte) (Byte.parseByte(picked.getLabelString()) - 1);
	parent.setColor(j);
	
    }
    
    /**
     * Sets the swatch at index i to be selected.
     */
    public void setSelected(int i) {
	theSwatches[i].setSelected(true);
    }
    
    public void setColor(int i) {
        theSwatches[i].doClick();
    }


    /**
     * Sets the swatch with label corresponding to the
     * KeyEvent to be selected.  (Must be between 1 and 0 on keyboard 
     * to select a button.)
     */
    public void keyPressed(KeyEvent e) {

	ColorSwatch picked;
	byte color;
	if (e.getKeyCode() == KeyEvent.VK_0) {
	    picked = theSwatches[9];
	    color = 9;
	}

	else if (e.getKeyCode() >= KeyEvent.VK_1 &&
	    e.getKeyCode() <= KeyEvent.VK_9) {
	    color = (byte) (e.getKeyCode() - KeyEvent.VK_1);
	    picked = theSwatches[color];
	}
	else {
	    return;
	}
        // Programmatically click on the button
        picked.doClick();
	parent.setColor(color);
	   
    }
    public void keyReleased(KeyEvent e) {}
    public void keyTyped(KeyEvent e){}

}

	    
	    
