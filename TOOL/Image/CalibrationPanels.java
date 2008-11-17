
package edu.bowdoin.robocup.TOOL.Image;

import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;

/**
 * An organizational and utility class, CalibrationPanels supplies a set of
 * display panels that are linked together via listener methods to perform
 * mimicked actions that are useful for calibration of images.  It instantiates
 * one PixleSelectionPanel and one ImageMarkerPanel, meant to display a source
 * image and some resulting image (like a thresholded image) whose image
 * markers and pixel selection ImageSwatch are connected.  This allows for
 * mouse motions on either panel to be automatically translated and displayed
 * on both ImagePanels simultaneously.
 *
 * This class does not set or update the images in either panel itself, but
 * simply propegates MouseEvents from each panel into setting marker positions
 * on the other panel.
 *
 * @author Jeremy R. Fishman
 * @see PixelSelectionPanel, ImageMarkerPanel
 */
public class CalibrationPanels implements MouseMotionListener {

    private PixelSelectionPanel selector;
    private ImageMarkerPanel displayer;

    public CalibrationPanels() {
        selector = new PixelSelectionPanel();
        displayer = new ImageMarkerPanel();

        selector.addMouseMotionListener(this);
        displayer.addMouseMotionListener(this);
    }

    public PixelSelectionPanel getSelector() {
        return selector;
    }

    public ImageMarkerPanel getDisplayer() {
        return displayer;
    }

    // MouseMotionListener methods

    public void mouseDragged(MouseEvent e) {
    }

    public void mouseMoved(MouseEvent e) {
        int x = e.getX();
        int y = e.getY();

        if (e.getSource() == selector) {
            displayer.setMarkerImagePosition(selector.getImageX(x),
                selector.getImageY(y));
        }else if (e.getSource() == displayer) {
            selector.setMarkerImagePosition(displayer.getImageX(x),
                displayer.getImageY(y));
        }
    }
}
