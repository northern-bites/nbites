
package edu.bowdoin.robocup.TOOL.Image;

import java.awt.Color;
import java.awt.Graphics;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import javax.swing.JPanel;

import edu.bowdoin.robocup.TOOL.Vision.Vision;

public class ColorSwatch extends JPanel implements PropertyChangeListener {

    public static final Color[] COLORS = Vision.COLORS;
    public static final int DEFAULT_BRUSH_SIZE = 10;

    private double xScale;
    private double yScale;
    private int brushSize;
    private int currentColor;

    public ColorSwatch() {
        this(DEFAULT_BRUSH_SIZE, 0);
    }
    
    public ColorSwatch(int size, int color) {
        super();

        brushSize = size;
        currentColor = color;

        xScale = 1;
        yScale = 1;
    }

    public void setScaling(double x, double y) {
        xScale = x;
        yScale = y;
    }

    public void paint(Graphics g) {
        super.paint(g);

        //draw color swatch
	g.setColor(COLORS[currentColor]);
	g.fillRect(0, 0, (int)Math.round(brushSize*xScale),
            (int)Math.round(brushSize*xScale));
    }

    public void propertyChange(PropertyChangeEvent e) {
        if (e.getPropertyName().equals(ImagePanel.X_SCALE_CHANGE))
            xScale = (Double)e.getNewValue();
        else if (e.getPropertyName().equals(ImagePanel.Y_SCALE_CHANGE))
            yScale = (Double)e.getNewValue();
    }
}

