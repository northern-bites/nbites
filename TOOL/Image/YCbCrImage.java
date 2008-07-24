
package TOOL.Image;

import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.io.DataInputStream;
import java.io.EOFException;
import java.io.IOException;

import TOOL.TOOL;

/**
 * The YCbCrImage class is a wrapper for the image data contained in a YCrCb
 * image retrieved from an Aibo's vision system, or from some other source.  It
 * provides easy functionality for loading the image data from less convenient
 * sources, and extends the BufferedImage image implementation for ease of use
 * in Java drawing application.
 *
 * @author Jeremy R. Fishman
 * @see ImagePanel
 */
public class YCbCrImage extends TOOLImage {

    public static final YCbCrColorSpace COLOR_SPACE = new YCbCrColorSpace();
    public static final int RAW_HEADER_SIZE = 100;
		public static final int RAW_IMAGE_SIZE = 160*208*3;
    
    public YCbCrImage(byte[] rawImage, int w, int h) {
        super(w, h);
                
        pixels = new byte[h][w][COLOR_DEPTH];

        readByteArray(rawImage);
    }

    public YCbCrImage(DataInputStream input, int width, int height)
            throws IOException {
        super(width, height);
        
        pixels = new byte[height][width][COLOR_DEPTH];

        readInputStream(input);
    }

    public void readByteArray(byte[] rawImage) {
        int i = 0; 
        for (int r = 0; r < getHeight(); r++) {
            for (int c = 0; c < getWidth(); c++)
		pixels[r][c][0] = rawImage[i++];
		//pixels[r][c][0] = signedByteToUnsigned(rawImage[i++]);
            for (int c = 0; c < getWidth(); c++)
                pixels[r][c][2] = rawImage[i++];
                //pixels[r][c][2] = signedByteToUnsigned(rawImage[i++]);
            for (int c = 0; c < getWidth(); c++)
                pixels[r][c][1] = rawImage[i++];
                //pixels[r][c][1] = signedByteToUnsigned(rawImage[i++]);
        }
    }

    public void readIntArray(int[][][] image) {
        for (int r = 0; r < getHeight(); r++) {
            for (int c = 0; c < getWidth(); c++) {
                pixels[r][c][0] = (byte)image[r][c][0];
                pixels[r][c][1] = (byte)image[r][c][1];
                pixels[r][c][2] = (byte)image[r][c][2];
            }
        }
    }
       
    public void writeByteArray(byte[] rawImage) {
        int i = 0; 
        for (int r = 0; r < getHeight(); r++) {
            for (int c = 0; c < getWidth(); c++)
                rawImage[i++] = pixels[r][c][0];
            for (int c = 0; c < getWidth(); c++)
                rawImage[i++] = pixels[r][c][2];
            for (int c = 0; c < getWidth(); c++)
                rawImage[i++] = pixels[r][c][1];
        }
    }

    public int rawImageSize() {
        return getWidth() * getHeight() * COLOR_DEPTH;
    }

    /**
     * Takes the current YCbCr data in the pixels integer array and fills in
     * pixels of the BufferedImage data buffer.  Converts all YCbCr to RGB
     * components and to a single RGB pixel value in the data buffer,
     * sequentially.
     *
     * I can't believe how easy this was once I set up the ColorSpace and
     * realized how to use it and the ColorModel....
     */
    public void initImage(BufferedImage img) {
        ColorModel cm = img.getColorModel();
        int[] rgb = new int[3];

        for (int r = 0; r < getHeight(); r++) {
            for (int c = 0; c < getWidth(); c++) {
                // Convert YCbCr to RGB with using the YCbCrColorSpace
                COLOR_SPACE.toRGB(pixels[r][c], rgb);

                // Set the BufferedImage pixel to the rgb value of this pixel
                // (use the ColorModel to convert from components to int)
                img.setRGB(c, r, cm.getDataElement(rgb, 0));
            }
        }
		
    }

}
