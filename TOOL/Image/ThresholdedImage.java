package edu.bowdoin.robocup.TOOL.Image;

import java.awt.image.BufferedImage;
import java.awt.Color;
import java.util.LinkedList;

import edu.bowdoin.robocup.TOOL.TOOL;
import edu.bowdoin.robocup.TOOL.Calibrate.Pair;
import edu.bowdoin.robocup.TOOL.Calibrate.ColorTableUpdate;
import edu.bowdoin.robocup.TOOL.Calibrate.CalibratePanel;
import edu.bowdoin.robocup.TOOL.Vision.Vision;

public class ThresholdedImage extends TOOLImage {

    private static final Color[] COLORS = Vision.COLORS;

    protected TOOLImage baseImage;
    protected ColorTable colorTable;
    protected byte[][] thresholded;

    public ThresholdedImage(TOOLImage image, ColorTable cTable){
        this(image.getWidth(), image.getHeight());

        thresholdImage(cTable, image);
    }

    public ThresholdedImage(byte[] rawImage, int width, int height) {
        this(width, height);

        readByteArray(rawImage);
    }

    public ThresholdedImage(byte[][] _thresholded, int width, int height) {
        this(width, height);

        thresholded = _thresholded;
        //An instance of this class created directly from a thresholded
        //array is not able to be re-classified at a later date
        baseImage = null;
    }

    public ThresholdedImage(int width, int height) {
        super(width, height);

        thresholded = new byte[getHeight()][getWidth()];
    }

    /**
     * thresholdes the image based on the color table
     * and the raw image. Needs to be called when
     * either the image or colortable has changed.
     */
    public void thresholdImage(ColorTable table, TOOLImage image){
        baseImage = image;
        if(table == null)
            return;
        colorTable = table;

        for (int y = 0; y < getHeight(); y++)
            for (int x = 0; x < getWidth(); x++)
                thresholded[y][x] = colorTable.getColor(image.getYCbCr(x,y));
    }
    public void thresholdImage() {
        if (colorTable == null) {
            TOOL.CONSOLE.error("ThresholdedImage.thresholdImage() called on" +
                               " an image without associated color table");
        }else if(baseImage == null) {
            TOOL.CONSOLE.error("ThresholedImage.thresholdImage() called on" +
                               "an image without associated base image");
        }else
            thresholdImage(colorTable, baseImage);
    }

    public void setBaseImage(TOOLImage image) {
        baseImage = image;
    }
    public void setColorTable(ColorTable table) {
        colorTable = table;
    }

    public byte getThresholded(int x, int y){
        return thresholded[y][x];
    }

    public int rawImageSize() {
        return getWidth() * getHeight();
    }

    protected void initImage(BufferedImage img) {
        for (int y = 0; y < getHeight(); y++)
            for (int x = 0; x < getWidth(); x++)
                img.setRGB(x, y, COLORS[thresholded[y][x]].getRGB());
    }

    public void readByteArray(byte[] rawImage) {
        int i = 0;
        for (int y = 0; y < getHeight(); y++)
            for (int x = 0; x < getWidth(); x++)
                thresholded[y][x] = rawImage[i++];
    }

    public void writeByteArray(byte[] rawImage) {
        int i = 0;
        for (int y = 0; y < getHeight(); y++)
            for (int x = 0; x < getWidth(); x++)
                rawImage[i++] = thresholded[y][x];
    }

    /**
     * Looks for places where there are holes in the thresholded image.
     * Heavily based on the code in AiboConnect.  A hole is defined to be
     * an undefined pixel surrounded by 8 identically colored (defined)
     * pixels.  Sends the updates to the colortable so that these pixels
     * STAY thresholded (rather than disappear on next mouse click).
     * @return a Vector of updates for this specific action.
     */
    public Pair <LinkedList<ColorTableUpdate>, Integer>
        fillHoles() {
        if (colorTable == null) {
            TOOL.CONSOLE.error("ThreasholdedImage.fillHoles() called on an " +
                               "image without associated color table");
            return null;
        }
        if (baseImage == null) {
            TOOL.CONSOLE.error("ThresholedImage.fillHoles() called on an " +
                               "image without associated base image");
            return null;
        }

        ColorTableUpdate[] updates = new ColorTableUpdate[COLORS.length];
        for (int i = 0; i < COLORS.length; i++) {
            updates[i] = new ColorTableUpdate((byte) i);
        }


        int holeCount = 0;

        byte UNDEFINED = Vision.GREY;
        int height = getHeight();
        int width = getWidth();

        int lastJ = 0;


        // Skip the very edges of the image. (to avoid AOB errors)
        for (int x = 1; x < width - 1; x++) {
            for (int y = 1; y < height - 1; y++) {
                // undefined, so check if it's surrounded by like pixels
                if (thresholded[y][x] == UNDEFINED) {
                    if (isSurrounded(x, y)) {
                        holeCount++;
                        // take the pixel directly left, since surrounded
                        byte color = thresholded[y][x-1];
                        // set this pixel
                        thresholded[y][x] = color;
                        // Add the filled in pixel to the update so that
                        // ColorTable is updated correctly.
                        updates[color].addPixel(baseImage.getYCbCr(x, y),
                                                UNDEFINED);

                        // Since we had a hole, it's impossible to have a
                        // hole one pixel down.  Skip it.
                        y++;
                    }
                }
            }
        }

        LinkedList <ColorTableUpdate> nonEmpty =
            new LinkedList<ColorTableUpdate>();

        // Find those updates that have actually filled in holes.  Add them
        // to a vector and modify the color table.
        for (int i = 0; i < updates.length; i++) {
            if (updates[i].getSize() > 0) {
                nonEmpty.add(updates[i]);
                colorTable.modifyTable(updates[i]);
            }
        }

        return new Pair<LinkedList<ColorTableUpdate>, Integer>(nonEmpty,
                                                               new Integer(holeCount));

    }// method

    /**
     * @return true if and only if the pixel at (x,y) is surrounded by
     *  8 identically colored pixels, which are NOT undefined.
     */
    private boolean isSurrounded(int x, int y) {

        int UNDEFINED = Vision.GREY;
        int color = thresholded[y-1][x-1];

        // We only want isSurrounded to return true if all 8 pixels around
        // (x,y) are the same color AND not undefined.
        if (color == UNDEFINED) return false;

        // Check clockwise around (x,y) starting at pixel above
        // and stopping right before the one above and to the left
        return (thresholded[y  ][x-1] == color &&
                thresholded[y+1][x-1] == color &&
                thresholded[y+1][x  ] == color &&
                thresholded[y+1][x+1] == color &&
                thresholded[y  ][x+1] == color &&
                thresholded[y-1][x+1] == color &&
                thresholded[y-1][x  ] == color);
    }


}
