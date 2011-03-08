package TOOL.Image;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.io.DataInputStream;
import java.io.EOFException;
import java.io.IOException;

import TOOL.TOOL;

/**
 * Reads the Nao's packed YUV422 format and converts into usable form for us.
 * @author Nicholas Dunn
 * @author Jeremy R. Fishman
 */

public class YUV422Image extends TOOLImage {

    public static final YCbCrColorSpace COLOR_SPACE = new YCbCrColorSpace();

    public static final String extension = ".raw";
    public static final int RAW_HEADER_SIZE = 0;

    public static final int BYTES_PER_TWO_PIXELS = 4;
    // Note that we expand the YUV422 image such that every pixel has a Y,U,and
    // V component when we load it from a file, even though the image packs
    // 2 pixels into 4 bytes of data.
    public static final int COLOR_DEPTH = 3;

    public static final int Y1_OFFSET = 0;
    public static final int U_OFFSET = 1;
    public static final int Y2_OFFSET = 2;
    public static final int V_OFFSET = 3;


     public YUV422Image(byte[] rawImage, int w, int h) {
        super(w, h);

        pixels = new byte[h][w][COLOR_DEPTH];

        readByteArray(rawImage);
    }

    public YUV422Image(DataInputStream input, int width, int height)
            throws IOException {
        super(width,height);

        pixels = new byte[height][width][COLOR_DEPTH];

        readInputStream(input);
    }

    // Number of bytes in the raw image
    public int rawImageSize() {
        return getWidth() * getHeight() * 2;
    }

    /**
     * Allocates an array of bytes big enough to hold the file information, and
     * fills it with the raw data.  Then calls readByteArray to process the
     * raw information.
     */
    protected void readInputStream(DataInputStream input) throws IOException {
        // YUV422 packs 6 bytes of RGB information into 4 bytes
    byte[] buffer = new byte[rawImageSize()];

    try{
        input.read(buffer);
        } catch (IOException e) {
            TOOL.CONSOLE.error(e);
        }
    readByteArray(buffer);
    }

    // Nao camera outputs in YUV422 format; this means that the YUV
    // information
    // for two adjacent pixels (6 bytes of color info) are packed into 4 bytes
    // Camera output    YUV422
    // Byte order   Y1, U, Y2, V
    // see http://en.wikipedia.org/wiki/YUV
    //
    public void readByteArray(byte[] rawImage) {
        int i = 0;
        byte y1, u, y2, v;

        for (int r = 0; r < getHeight(); r++) {
            for (int c = 0; c < getWidth(); c+=2, i+=BYTES_PER_TWO_PIXELS) {
                y1 = rawImage[i + Y1_OFFSET];
                u =  rawImage[i + U_OFFSET];
                y2 = rawImage[i + Y2_OFFSET];
                v =  rawImage[i + V_OFFSET];

                pixels[r][c][0] = y1;
                pixels[r][c][1] = u;
                pixels[r][c][2] = v;

                pixels[r][c+1][0] = y2;
                pixels[r][c+1][1] = u;
                pixels[r][c+1][2] = v;
            }
        }
    }

    public void writeByteArray(byte[] rawImage) {
        int i = 0;

        for (int r = 0; r < getHeight(); r++) {
            for (int c = 0; c < getWidth(); c+=2, i+=BYTES_PER_TWO_PIXELS) {
                rawImage[i + Y1_OFFSET] = pixels[r][c  ][0];
                rawImage[i + U_OFFSET ] = pixels[r][c  ][1];
                rawImage[i + V_OFFSET ] = pixels[r][c  ][2];
                rawImage[i + Y2_OFFSET] = pixels[r][c+1][0];
            }
        }
    }

    public void initImage(BufferedImage img) {
        ColorModel cm = img.getColorModel();

        int[] rgb = new int[3];

        for (int r = 0; r < getHeight(); r++) {
            for (int c = 0; c < getWidth(); c++) {
                // Convert YUV to RGB with using the YCbCrColorSpace
                COLOR_SPACE.toRGB(pixels[r][c], rgb);

                // Set the BufferedImage pixel to the rgb value of this pixel
                // (use the ColorModel to convert from components to int)
                img.setRGB(c, r, cm.getDataElement(rgb, 0));

            }
        }
    }

}
