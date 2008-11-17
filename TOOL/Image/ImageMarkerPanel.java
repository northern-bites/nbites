
package edu.bowdoin.robocup.TOOL.Image;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;

/**
 * Extended version of ImagePanel that can listen to mouse motions and draw a
 * crosshair marker on the image.  The crosshair marker will automatically 
 * follow the mouse's motion, but can also be set manually independently of 
 * mouse action.
 *
 * @author jeremy R. Fishman
 * @see ImagePanel
 */
public class ImageMarkerPanel extends ImagePanel
        implements MouseMotionListener {
    public static final int DFLT_WINDOW_W = 416;
    public static final int DFLT_WINDOW_H = 320;

    public static final Color MARKER_COLOR = new Color(139, 69, 19);
    public static final int MARKER_HALF_WIDTH = 10;
    public static final int MARKER_HALF_HEIGHT = 10;

    private int marker_x;
    private int marker_y;

    public ImageMarkerPanel() {
        super();

        marker_x = -1;
        marker_y = -1;

        addMouseMotionListener(this);
    }

    public void setMarkerImagePosition(int x, int y) {
        setMarkerPosition(getPanelX(x), getPanelY(y));
    }

    public void setMarkerPosition(int x, int y) {
        int old_x = marker_x, old_y = marker_y;
        marker_x = x;
        marker_y = y;
        repaint(old_x - MARKER_HALF_WIDTH, old_y - MARKER_HALF_HEIGHT,
            2 * MARKER_HALF_WIDTH + 1, 2 * MARKER_HALF_HEIGHT + 1);
    }

    public void paint(Graphics g) {
        super.paint(g);

        // Draw the marker
        if (marker_x >= 0 && marker_y >= 0) {
            Color old = g.getColor();
            g.setColor(MARKER_COLOR);
            g.drawLine(marker_x - MARKER_HALF_WIDTH, marker_y,
                marker_x + MARKER_HALF_WIDTH, marker_y);
            g.drawLine(marker_x, marker_y - MARKER_HALF_HEIGHT, marker_x,
                marker_y + MARKER_HALF_HEIGHT);
            g.setColor(old);
        }
    }

    // MouseMotionListener methods

    public void mouseDragged(MouseEvent e) {
	setMarkerPosition(e.getX(), e.getY());

    }

    public void mouseMoved(MouseEvent e) {
        setMarkerPosition(e.getX(), e.getY());
    }
}
