
package TOOL.Image;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.awt.image.BufferedImageOp;
import java.awt.image.ColorModel;
import java.awt.geom.AffineTransform;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;

import TOOL.TOOL;

/**
 * The ImagePanel class is intended to be an easy-to-implement all around
 * object for displaying images in the TOOL.  It has many features from simple
 * to complex intended to be useful in displaying images in any way needed.  It
 * is also intended to be fast, both in adding the image functionality to your
 * code and in drawing and updating the images themselves.
 *
 * Avoid calling any drawing methods explicitly.  The class is designed to
 * update the image any time and only when it is needed.
 *
 * The ImagePanel was written with the YCbCrImage class in mind, as that is what
 * most images in the TOOL will be on the back-end.  However, any BufferedImage
 * should be useable.  Java provides many facilities for loading different
 * types of images, all of which can be buffered for use in this efficient
 * drawing class.
 *
 * @author Jeremy R. Fishman
 * @see YCbCrImage
 */
public class ImagePanel extends JPanel implements ActionListener,
        MouseListener {

    public static final int INTERPOLATION_TYPE = AffineTransformOp.TYPE_NEAREST_NEIGHBOR;
    //public static final int INTERPOLATION_TYPE = AffineTransformOp.TYPE_BILINEAR;
    //public static final int INTERPOLATION_TYPE = AffineTransformOp.TYPE_BICUBIC;

    // Scale setting constants.  Specify which type of scaling to use.
    public static final int SCALE_NONE = 0;
    public static final int SCALE_FIXED_SIZE = 1;
    public static final int SCALE_FIXED_RATIO = 2;
    public static final int SCALE_AUTO_WIDTH = 3;
    public static final int SCALE_AUTO_HEIGHT = 4;
    public static final int SCALE_AUTO_BOTH = 5;

    public static final int SCALE_RATIO_SIZE_HYBRID = 6;


    // Property name constants, for propegating property changes
    public static final String X_SCALE_CHANGE = "ImagePanel.xScale";
    public static final String Y_SCALE_CHANGE = "ImagePanel.yScale";

    // A few default setting
    public static final int DEFAULT_SCALE_SETTING = SCALE_AUTO_BOTH;
    public static final int DEFAULT_FIXED_WIDTH = 400;
    public static final int DEFAULT_FIXED_HEIGHT = 300;

    // The mouse button used for opening the popup menu
    //  (right mouse button)
    public static final int POPUP_MENU_BUTTON = MouseEvent.BUTTON3;
    public static final int POPUP_MENU_CTL_BUTTON = MouseEvent.BUTTON1;

    protected BufferedImage image;
    protected BufferedImageOp imageOp, overlayOp;
    protected AffineTransform imageTransform, overlayTransform;

    protected BufferedImage overlay;

    protected int xOffset;
    protected int yOffset;
    protected double xImageScale, yImageScale, xOverlayScale, yOverlayScale;
    protected int scaleSetting;

    protected int fixedWidth;
    protected int fixedHeight;

    protected int lastWidth;
    protected int lastHeight;

    protected JPopupMenu popupMenu;
    
    public ImagePanel() {
        image = null;

        xOffset = 0;
        yOffset = 0;
        xImageScale = yImageScale = xOverlayScale = yOverlayScale = 1;

        scaleSetting = DEFAULT_SCALE_SETTING;

        fixedWidth = DEFAULT_FIXED_WIDTH;
        fixedHeight = DEFAULT_FIXED_HEIGHT;


        lastWidth = getWidth();
        lastHeight = getHeight();

        imageTransform = new AffineTransform();
        overlayTransform = new AffineTransform();

        imageOp = new AffineTransformOp(imageTransform, INTERPOLATION_TYPE);
        overlayOp = new AffineTransformOp(overlayTransform, INTERPOLATION_TYPE);

        createPopupMenu();

        addMouseListener(this);
    }





    public void createPopupMenu() {
        popupMenu = new JPopupMenu("Settings");

        JMenuItem item;
        JMenu menu;

        item = new JMenuItem("Actual size");
        item.setActionCommand(String.valueOf(SCALE_NONE));
        item.addActionListener(this);
        popupMenu.add(item);

        menu = new JMenu("Fixed");
        item = new JMenuItem("Size");
        item.setActionCommand(String.valueOf(SCALE_FIXED_SIZE));
        item.addActionListener(this);
        menu.add(item);
        item = new JMenuItem("Ratio");
        item.setActionCommand(String.valueOf(SCALE_FIXED_RATIO));
        item.addActionListener(this);
        menu.add(item);
        popupMenu.add(menu);

        menu = new JMenu("Auto scale");
        item = new JMenuItem("Width");
        item.setActionCommand(String.valueOf(SCALE_AUTO_WIDTH));
        item.addActionListener(this);
        menu.add(item);
        item = new JMenuItem("Height");
        item.setActionCommand(String.valueOf(SCALE_AUTO_HEIGHT));
        item.addActionListener(this);
        menu.add(item);
        item = new JMenuItem("Both");
        item.setActionCommand(String.valueOf(SCALE_AUTO_BOTH));
        item.addActionListener(this);
        menu.add(item);
        item = new JMenuItem("Ratio and Size");
        item.setActionCommand(String.valueOf(SCALE_RATIO_SIZE_HYBRID));
        item.addActionListener(this);
        menu.add(item);


        menu.add(item);
        popupMenu.add(menu);
    }

    public BufferedImage getImage() {
        return image;
    }

    public void updateImage(TOOLImage toolImage) {
        if (image != null && image.getWidth() == toolImage.getWidth() &&
                image.getHeight() == toolImage.getHeight()) {
            // re-initialize the image with the new data
            toolImage.initImage(image);
            // set image again (to the current one, but performs updates)
            setImage(image);

        }else
            // create a new image
            setImage(toolImage.createImage());
    }

    public void setImage(BufferedImage image) {
        this.image = image;

        calculateScaling();
        updateAffineTransform();
        repaint();
    }

    public void setOverlayImage(BufferedImage o){
        overlay = o;

        calculateScaling();
        updateAffineTransform();
        repaint();
    }

    public BufferedImageOp getImageOp() {
        return imageOp;
    }

    public void paint(Graphics g) {

        super.paint(g);

        Graphics2D g2d = (Graphics2D)g;

	// Window has been resized, calculate the new scale
        if (getWidth() != lastWidth || getHeight() != lastHeight) {

            calculateScaling();
            updateAffineTransform();
        }

        if (image != null) {
            g2d.drawImage(image, imageOp, 0, 0);
        }
        if (overlay != null) {
            g2d.drawImage(overlay, overlayOp, 0, 0);
        }
        lastWidth = getWidth();
        lastHeight = getHeight();
    }



    protected void calculateScaling() {
        if (image != null) {

            double oldXImageScale = xImageScale;
            double oldYImageScale = yImageScale;
            switch (scaleSetting) {
            case SCALE_NONE:
                xImageScale = 1;
                yImageScale = 1;
                break;
            case SCALE_FIXED_SIZE:
                xImageScale = ((double)fixedWidth) / image.getWidth();
                yImageScale = ((double)fixedHeight) / image.getHeight();
                break;
            case SCALE_FIXED_RATIO:
                break;
            case SCALE_AUTO_WIDTH:
                xImageScale = ((double)(getWidth() - xOffset)) / image.getWidth();
                yImageScale = xImageScale;
                break;
            case SCALE_AUTO_HEIGHT:
                yImageScale = ((double)(getHeight() - yOffset)) / image.getHeight();
                xImageScale = yImageScale;
                break;
            case SCALE_AUTO_BOTH:
                xImageScale = ((double)(getWidth() - xOffset)) / image.getWidth();
                yImageScale = ((double)(getHeight() - yOffset)) / image.getHeight();
                // actually, scale to the smaller dimension
                xImageScale = yImageScale = Math.min(xImageScale, yImageScale);
                break;


                // This allows a buffered image to keep an arbitrary
                // aspect ratio, as defined by the fixedWidth and fixedHeight,
                // yet scale this fixed ratio with the screen.  For instance,
                // in ColorEdit, the scales vary widely on the amount of entries
                // displayed, but I want all the entries to be shown on the same
                // size rectangle; this is the mode to use.
            case SCALE_RATIO_SIZE_HYBRID:
                // xImageScale is the amount we need to scale the raw image to be the
                // fixedWidth, similarly with yImageScale
                xImageScale = ((double)fixedWidth) / (double)image.getWidth();
                yImageScale = ((double)fixedHeight) / (double)image.getHeight();

                // Now we determine how much we need to stretch the "fixed width"
                // and "fixed height" to match the screen size, using same rationale
                // as SCALE_AUTO_BOTH
                double scalingFactor1 = getWidth()/(double)fixedWidth;
                double scalingFactor2 = getHeight()/(double)fixedHeight;
                double scale = Math.min(scalingFactor1, scalingFactor2);

                // ImageScale the x and y scale by the new scaling factor
                xImageScale *= scale;
                yImageScale *= scale;
                break;
            }

            if (xImageScale == 0)
                xImageScale = 1;
            if (yImageScale == 0)
                yImageScale = 1;

            if (xImageScale != oldXImageScale)
                firePropertyChange(X_SCALE_CHANGE, new Double(oldXImageScale),
                                   new Double(xImageScale));
            if (yImageScale != oldYImageScale)
                firePropertyChange(Y_SCALE_CHANGE, new Double(oldYImageScale),
                                   new Double(yImageScale));

        }

        if (overlay != null) {

            double oldXOverlayScale = xOverlayScale;
            double oldYOverlayScale = yOverlayScale;
            switch (scaleSetting) {
            case SCALE_NONE:
                xOverlayScale = 1;
                yOverlayScale = 1;
                break;
            case SCALE_FIXED_SIZE:
                xOverlayScale = ((double)fixedWidth) / overlay.getWidth();
                yOverlayScale = ((double)fixedHeight) / overlay.getHeight();
                break;
            case SCALE_FIXED_RATIO:
                break;
            case SCALE_AUTO_WIDTH:
                xOverlayScale = ((double)(getWidth() - xOffset)) / overlay.getWidth();
                yOverlayScale = xOverlayScale;
                break;
            case SCALE_AUTO_HEIGHT:
                yOverlayScale = ((double)(getHeight() - yOffset)) / overlay.getHeight();
                xOverlayScale = yOverlayScale;
                break;
            case SCALE_AUTO_BOTH:
                xOverlayScale = ((double)(getWidth() - xOffset)) / overlay.getWidth();
                yOverlayScale = ((double)(getHeight() - yOffset)) / overlay.getHeight();
                // actually, scale to the smaller dimension
                xOverlayScale = yOverlayScale = Math.min(xOverlayScale, yOverlayScale);
                break;


                // This allows a buffered overlay to keep an arbitrary
                // aspect ratio, as defined by the fixedWidth and fixedHeight,
                // yet scale this fixed ratio with the screen.  For instance,
                // in ColorEdit, the scales vary widely on the amount of entries
                // displayed, but I want all the entries to be shown on the same
                // size rectangle; this is the mode to use.
            case SCALE_RATIO_SIZE_HYBRID:
                // xOverlayScale is the amount we need to scale the raw overlay to be the
                // fixedWidth, similarly with yOverlayScale
                xOverlayScale = ((double)fixedWidth) / (double)overlay.getWidth();
                yOverlayScale = ((double)fixedHeight) / (double)overlay.getHeight();

                // Now we determine how much we need to stretch the "fixed width"
                // and "fixed height" to match the screen size, using same rationale
                // as SCALE_AUTO_BOTH
                double scalingFactor1 = getWidth()/(double)fixedWidth;
                double scalingFactor2 = getHeight()/(double)fixedHeight;
                double scale = Math.min(scalingFactor1, scalingFactor2);

                // OverlayScale the x and y scale by the new scaling factor
                xOverlayScale *= scale;
                yOverlayScale *= scale;
                break;
            }

            if (xOverlayScale == 0)
                xOverlayScale = 1;
            if (yOverlayScale == 0)
                yOverlayScale = 1;

            if (xOverlayScale != oldXOverlayScale)
                firePropertyChange(X_SCALE_CHANGE, new Double(oldXOverlayScale),
                                   new Double(xOverlayScale));
            if (yOverlayScale != oldYOverlayScale)
                firePropertyChange(Y_SCALE_CHANGE, new Double(oldYOverlayScale),
                                   new Double(yOverlayScale));
        }
    }

    public double getXScale(){
        return xImageScale;
    }
    public double getYScale(){
	return yImageScale;
    }

    private void updateAffineTransform() {
        imageTransform.setToScale(xImageScale, yImageScale);
        imageOp = new AffineTransformOp(imageTransform, INTERPOLATION_TYPE);

        overlayTransform.setToScale(xOverlayScale, yOverlayScale);
        overlayOp = new AffineTransformOp(overlayTransform, INTERPOLATION_TYPE);
    }

    public void changeSettings(int setting) {
        if (setting != scaleSetting) {
            scaleSetting = setting;

            calculateScaling();
        }

        updateAffineTransform();
        repaint();
    }

    public void getFixedSizeSettings() {
    }

    public void getFixedRatioSettings() {
    }

    public int getImageX(int x) {
	if(image == null) {
	    return -1;
	}

        x = (x - xOffset);
        if (x < 0)
            return -1;
        else {
            x = (int)(x / xImageScale);
            if (x >= image.getWidth()) {
	    	return -1;
	    }
            else {
                return x;
	    }
        }
    }

    public int getImageY(int y) {
	if(image == null)
	    return -1;
        y = (y - yOffset);
        if (y < 0)
            return -1;
        else {
            y = (int)(y / yImageScale);
            if (y >= image.getHeight())
                return -1;
            else
                return y;
        }
    }

    public int getPanelX(int x) {
        x = (int)(x * xImageScale) - xOffset;

        if (x < 0 || x > getWidth())
            return -1;
        return x;
    }

    public int getPanelY(int y) {
        y = (int)(y * yImageScale) - yOffset;

        if (y < 0 || y > getWidth())
            return -1;
        return y;
    }

    /**
     * Returns the image's value at a given image xy coordinate.

    public int getImageValue(int x, int y){
	return image.getRGB(x,y);
    }

    public int[] getImageYUV(int x, int y){
	ColorModel cm = image.getColorModel();
	int rgbValue = image.getRGB(x,y);
	int[] rgb = {cm.getRed(rgbValue),
		     cm.getGreen(rgbValue),
		     cm.getBlue(rgbValue)};

	int[] yuv = new int[3];
	YCbCrImage.COLOR_SPACE.fromRGB(rgb[0],rgb[1],rgb[2], yuv);
	return yuv;
	}*/

    // ActionListener methods

    /**
     * Responds to the ActionEvent generated when a pop-up menu item has been
     * selected, or 'clicked'.  Depending on the action command of the menu
     * item, will call methods to change the current setting, and may call
     * methods to display OptionPane pop-up frames for more settings.
     *
     * @param e The ActionEvent that has occured, signalling a selected
     * JMenuItem.
     */
    public void actionPerformed(ActionEvent e) {
        // hide menu
        //popupMenu.setVisible(false);

        int setting = Integer.parseInt(e.getActionCommand());
        if (setting == SCALE_FIXED_SIZE) {
            getFixedSizeSettings();
        }else if (setting == SCALE_FIXED_RATIO) {
            getFixedRatioSettings();
        }

        changeSettings(Integer.parseInt(e.getActionCommand()));
    }

    // MouseListener methods

    public void mouseClicked(MouseEvent e) {
        if (e.getButton() == POPUP_MENU_BUTTON ||
                e.getButton() == POPUP_MENU_CTL_BUTTON &&
                e.isControlDown())
             popupMenu.show(this, e.getX(), e.getY());
    }

    public void mouseEntered(MouseEvent e) {
    }

    public void mouseExited(MouseEvent e) {
    }

    public void mousePressed(MouseEvent e) {
    }

    public void mouseReleased(MouseEvent e) {

    }
}
    
