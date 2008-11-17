package edu.bowdoin.robocup.TOOL.Image;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.io.DataInputStream;
import java.io.EOFException;
import java.io.IOException;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;

import edu.bowdoin.robocup.TOOL.TOOL;

/**
 * Reads the Webots simulator's packed RGB format and converts into usable form
 * for us.
 * @author Northern Bites Team
 * @author http://robocup.bowdoin.edu/blog/history
 */

public class RGBImage extends TOOLImage {
    
    public static final String extension = ".raw";
    public static final int RAW_HEADER_SIZE = 0;

    public static final int BYTES_PER_TWO_PIXELS = 6;


    public static final int Y1_OFFSET = 0;
    public static final int U_OFFSET = 1;
    public static final int Y2_OFFSET = 2;
    public static final int V_OFFSET = 3;


     public RGBImage(byte[] rawImage, int w, int h) {
        super(w, h);
                
        pixels = new byte[h][w][COLOR_DEPTH];

        readByteArray(rawImage);
    }

    public RGBImage(DataInputStream input, int width, int height)
            throws IOException {
        super(width, height);
        pixels = new byte[height][width][COLOR_DEPTH];

        readInputStream(input);
    }

    public int rawImageSize() {
        return getWidth() * getHeight() * COLOR_DEPTH;
    }
    
    // Color Format: RGB
    // Byte order: Interleaved, RGB...RGB
    //
    public void readByteArray(byte[] rawImage) {
        int i = 0;
        for (int r = 0; r < getHeight(); r++) {
            for (int c = 0; c < getWidth(); c++) {
                pixels[r][c][0] = rawImage[i++];
                pixels[r][c][1] = rawImage[i++];
                pixels[r][c][2] = rawImage[i++];
            }
        }
    }

    protected void writeByteArray(byte[] rawImage) {
        int i = 0;
        for (int r = 0; r < getHeight(); r++) {
            for (int c = 0; c < getWidth(); c+=2, i+=3) {
                rawImage[i  ] = pixels[r][c][0];
                rawImage[i+1] = pixels[r][c][1];
                rawImage[i+2] = pixels[r][c][2];
            }
        }
    }

    public void initImage(BufferedImage img) {
        ColorModel cm = img.getColorModel();
        int[] rgb = new int[3];

        for (int r = 0; r < getHeight(); r++) {
            for (int c = 0; c < getWidth(); c++) {
                // Fill the integer rgb array with byte values
                rgb[0] = pixels[r][c][0];
                rgb[1] = pixels[r][c][1];
                rgb[2] = pixels[r][c][2];

                // Set the BufferedImage pixel to the rgb value of this pixel
                // (use the ColorModel to convert from components to int)
                img.setRGB(c, r, cm.getDataElement(rgb, 0));
                
            }
        }
    }

    // Overridden getYCbCr() method, to convert values
    public int[] getYCbCr(int x, int y) {
        int[] yCbCr = new int[COLOR_DEPTH];
        YCbCr_CS.fromRGB(pixels[y][x], yCbCr);
        return yCbCr;
    }
    
}
