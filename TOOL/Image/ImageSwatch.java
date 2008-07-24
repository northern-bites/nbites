
package TOOL.Image;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.awt.image.BufferedImageOp;
import java.awt.geom.AffineTransform;
import javax.swing.JPanel;

/**
 * An ImageSwatch is a JPanel that represents a subsection of another
 * BufferedImage.  The swatch can use any amount of the underlying image, but 
 * is meant to be used to blow up a small section (a few pixels on a side) of
 * the master image.  This section can be expanded any amount, but will lose
 * clarity upon being expanded.  By default, we use a bicubic image expansion 
 * algorithm, which will look nice but add false (not-specified) data to the
 * image -- BEWARE.
 *
 * The paint() method can be used to draw the ImageSwatch to a Graphics object.
 * This will draw the swatch at the (0,0) positon in the Graphics context, so
 * classes will need to translate their Graphics, or create a child Graphics
 * object, to choose the location of the ImageSwatch.
 *
 * @author Jeremy R. Fishman
 * @see PixelSelectionPanel
 */
public class ImageSwatch extends JPanel {

    //public static final int INTERPOLATION_TYPE = AffineTransformOp.TYPE_BICUBIC;
    public static final int INTERPOLATION_TYPE = AffineTransformOp.TYPE_NEAREST_NEIGHBOR;
    public static final int EMPTY_COLOR = Color.black.getRGB();

    private BufferedImage sourceImage;
    private BufferedImage drawImage;
    private AffineTransformOp imageOp;
    private AffineTransform transform;

    private int expand_w;
    private int expand_h;
    private int source_x;
    private int source_y;

    public ImageSwatch(int w, int h) {
        this(w, h, w, h);
    }
    
    public ImageSwatch(int w, int h, int ex_w, int ex_h) {
        drawImage = new BufferedImage(w, h, BufferedImage.TYPE_INT_RGB);

        sourceImage = null;
        expand_w = ex_w;
        expand_h = ex_h;
        source_x = -1;
        source_y = -1;

        setTransform();
    }

    public BufferedImage getSourceImage() {
        return sourceImage;
    }

    /**
     * Sets the underlying source image and calls on the method to determine
     * which pixels are currently under the mouse and thus should be drawn
     * enlarged
     * @param image the BufferedImage to set as underlying image
     */
    public void setSourceImage(BufferedImage image) {
        sourceImage = image;
        setSourcePosition(source_x, source_y);
    }


    /**
     * Updates the location of the area on which to zoom in.  Changes the
     * BufferedImage representing the blown up area in order to match the
     * area centered around (x, y).
     * @param x xCoordinate to center the swatch around
     * @param y yCoordinate to center swatch around
     */
    public void setSourcePosition(int x, int y) {
        source_x = x;
        source_y = y;

	if (sourceImage == null || source_x < 0 || source_y < 0) {
	    return;
	}

        int d_w = drawImage.getWidth();
        int d_h = drawImage.getHeight();
        int s_w = sourceImage.getWidth();
        int s_h = sourceImage.getHeight();
	int x_len = d_w / 2;
	int y_len = d_h / 2;

        for (int r = 0; r < d_h; r++) {
            for (int c = 0; c < d_w; c++) {
                x = source_x + c - x_len;
                y = source_y + r - y_len;
		
                if (x < 0 || y < 0 || x >= s_w || y >= s_h) {
                    drawImage.setRGB(c, r, EMPTY_COLOR);
		}
                else {
                    drawImage.setRGB(c, r, sourceImage.getRGB(x, y));
		}
            }
        }
    }

    public int getExpandWidth() {
        return expand_w;
    }

    public int getExpandHeight() {
        return expand_h;
    }

    public void setExpandWidth(int w) {
        expand_w = w;
        setTransform();
    }

    public void setExpandHeight(int h) {
        expand_h = h;
        setTransform();
    }

    public void setExpandDims(int w, int h) {
        expand_w = w; 
        expand_h = h;
        setTransform();
    }

    private void setTransform() {
        transform = AffineTransform.getScaleInstance(
            ((double)expand_w) / drawImage.getWidth(),
            ((double)expand_h) / drawImage.getHeight()
            );
        imageOp = new AffineTransformOp(transform, INTERPOLATION_TYPE);
    }


    

    public void paint(Graphics g) {
        super.paint(g);

        Graphics2D g2d = null;
        try {
            g2d = (Graphics2D)g;
        }catch (ClassCastException e) {
            System.err.println("ImageSwatch cannot draw to a non-Graphics2D " +
                               "Graphics object");
        }
	
        
        if (g2d != null) {
	    //g2d.setColor(Color.RED);
	    //g2d.fillRect(0,0,10,10);
	    g2d.drawImage(drawImage, imageOp, 0, 0);
	}
	



    }
}
