
package TOOL.Image;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.awt.image.BufferedImageOp;
import java.awt.geom.AffineTransform;
import java.awt.Color;


/**
 * Extended version of ImageMarkerPanel that also displays the subsection of
 * the image under the curent marker location.  The subsection display is
 * handled by the ImageSwatch class, which requires this class only to pass it
 * the underlying image and the current location.  Subsection image is drawn
 * via standard BufferedImage drawing faclilities.  Size of the subsection (in
 * terms of the original image, and expanded size) is determined by constants.
 * In the case that the default swatch positon is in the way of some action or
 * view desired on the underlying image, the colorSwatch is draggable to a new
 * position via the MouseListener methods in this class.
 *
 * An important feature of the PixelSelectionPanel is the addition of a source
 * image, or the separation of the display image and the pixel selection image.
 * This allows for updating of the display image to represent results of
 * actions on the image, without altering the pixel source image.
 *
 * @author Jeremy R. Fishman
 * @see ImageSwatch, ImageMarkerPanel
 */
public class PixelSelectionPanel extends ImageMarkerPanel {

    public static final int NUM_SWATCH_PIXELS = 3;
    public static final int COLOR_SWATCH_WIDTH = 50;
    public static final double DEFAULT_SWATCH_X_RATIO = .2;
    public static final double DEFAULT_SWATCH_Y_RATIO = .9;
    

    public static final int MAX_BRUSH_SIZE = TOOL.Calibrate.Calibrate.MAX_BRUSH_SIZE;

    private double swatch_x_ratio,swatch_y_ratio,
	cur_swatch_x_ratio,cur_swatch_y_ratio;

    private ImageSwatch swatch;
    private boolean drag_swatch;

    private Color[] COLORS = TOOL.Vision.Vision.COLORS;
	
    private int currentColor,brushSize;

    public PixelSelectionPanel() {
        super();

	//shows the pixels under the mouse
        swatch_x_ratio = DEFAULT_SWATCH_X_RATIO;
        swatch_y_ratio = DEFAULT_SWATCH_Y_RATIO;
        swatch = new ImageSwatch(NUM_SWATCH_PIXELS, NUM_SWATCH_PIXELS,
            COLOR_SWATCH_WIDTH, COLOR_SWATCH_WIDTH);



	//shows the current calibration color
	//NUM_SWATCH_PIXELS is irrelevant in this case
	cur_swatch_x_ratio = 1-DEFAULT_SWATCH_X_RATIO;
	cur_swatch_y_ratio = DEFAULT_SWATCH_Y_RATIO;
	
	setCalibrateColor(5);
	setBrushSize(10);
    }

    
    

    public void setCalibrateColor(int newColor){
	currentColor = newColor;
    }
    public void setBrushSize(int newSize){
	brushSize = newSize;
    }

    /**
     * Sets the image both for this context AND for the underlying blowup
     * swatch; otherwise it has no idea of the picture to draw
     */
    public void setImage(BufferedImage image) {
        super.setImage(image);
	swatch.setSourceImage(image);
        paintSwatch();
    }

    public void paint(Graphics g) {
        if (!isVisible()) { return; }
        super.paint(g);
        paintSwatch(g);
    }

    public void paintSwatch() {
        Graphics g = getGraphics();
        if (g != null && isShowing()) {
            paintSwatch(g);
            g.dispose();
        }
    }

    private void paintSwatch(Graphics g) {
        int x = (int)(getWidth() * swatch_x_ratio)
            - swatch.getExpandWidth() / 2;
        int y = (int)(getHeight() * swatch_y_ratio)
            - swatch.getExpandHeight() / 2;
        Graphics sub_g = g.create(x, y, swatch.getExpandWidth() + 1,
            swatch.getExpandHeight() + 1);
        
        swatch.paint(sub_g);
        sub_g.dispose();
    }

   

    public void setSwatchCoordinates(int x, int y) {
        swatch.setSourcePosition(x, y);
    }

    public void setMarkerPosition(int x, int y) {
        swatch.setSourcePosition(getImageX(x), getImageY(y));
        super.setMarkerPosition(x, y);
    }

    // MouseListener methods
    //  (others are implemented by ImagePanel)

    public void mousePressed(MouseEvent e) {
        int x = e.getX();
        int y = e.getY();
        int minx = (int)(swatch_x_ratio * getWidth())
            - swatch.getExpandWidth() / 2;
        int miny = (int)(swatch_y_ratio * getHeight())
            - swatch.getExpandHeight() / 2;
        if (x >= minx && x < minx + swatch.getExpandWidth() &&
                y >= miny && y < miny + swatch.getExpandHeight())
            drag_swatch = true;
    }

    public void mouseReleased(MouseEvent e) {
        drag_swatch = false;
    }

    // MouseMotionListener methods

    public void mouseDragged(MouseEvent e) {
        if (drag_swatch) {
            swatch_x_ratio = ((double)e.getX()) / getWidth();
            swatch_y_ratio = ((double)e.getY()) / getHeight();
            repaint();
        }
    }

    public void mouseMoved(MouseEvent e) {
	swatch.setSourcePosition(getImageX(e.getX()), getImageY(e.getY()));
	paintSwatch();
	
    }
}
