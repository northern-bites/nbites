package TOOL.Image;


import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;

import TOOL.TOOL;

/**
 * @author Nicholas Dunn
 * @date February 4, 2008
 * @see YCbCrImage
 * Code mainly copied and moved from Jeremy Fishman's YCbCr implementation.
 *
 * All picture formats we rig up for the TOOL should extend this class.
 */
public abstract class TOOLImage {
    protected static final YCbCrColorSpace YCbCr_CS = new YCbCrColorSpace();
    protected byte[][][] pixels;

    protected int width;
    protected int height;

    protected TOOLImage(int w, int h){
        width = w;
        height = h;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public abstract int rawImageSize();

    /**
     * Fills in the pixels array from a one-dimensional byte array.
     * This method will vary depending on the file format, due the to
     * differences in how pixel data is stored when packed in an array
     * (YCbCr row-by-row or interleaved, YUV422, RGB interleaved, etc.)
     */
    public abstract void readByteArray(byte[] rawImage);

    /**
     * Fills a one-dimensional byte array from the pixels array.
     * This method will vary depending on the file format, due the to
     * differences in how pixel data is stored when packed in an array
     * (YCbCr row-by-row or interleaved, YUV422, RGB interleaved, etc.)
     */
    public abstract void writeByteArray(byte[] rawImage);


    /**
     * Allocates an array of bytes big enough to hold the file information, and
     * fills it with the raw data.  Then calls readByteArray to process the
     * raw information.
     */
    protected void readInputStream(DataInputStream input) throws IOException {
	byte[] buffer = new byte[rawImageSize()];
        input.readFully(buffer);
	readByteArray(buffer);
    }

    public byte[] getByteArray() {
        byte[] buffer = new byte[rawImageSize()];
        writeByteArray(buffer);
        return buffer;
    }

    public void writeOutputStream(DataOutputStream output) throws IOException {

        byte[] buffer = new byte[rawImageSize()];

        writeByteArray(buffer);
        output.write(buffer);
    }

    /**
     * Initializes a BufferedImage based on the contents of the pixels array
     * and the file format.  For instance, NaoSim frames are already in RGB
     * format whereas Aibo frames need to convert from YUV to RGB colorspace.
     */
    abstract void initImage(BufferedImage img);

    public BufferedImage createImage() {
        BufferedImage img = new BufferedImage(getWidth(), getHeight(),
            BufferedImage.TYPE_INT_RGB);
        initImage(img);
        return img;
    }

    /**
    *
    */
    public byte[][][] getPixels() {
        return pixels;
    }


    public int[] getYCbCr(int x, int y) {
        return getPixel(x,y);
    }

    public int[] getPixel(int x, int y) {
        if (pixels == null) {
            System.err.println("Error, attempting to retrieve a pixel value "+
                               "when pixels array is null");
        }

        int[] values = new int[3];

        values[0] = getComponent(x, y, 0);
        values[1] = getComponent(x, y, 1);
        values[2] = getComponent(x, y, 2);

        return values;
    }

    public int getComponent(int x, int y, int c) {
        return pixels[y][x][c] & 0xff; // Convert the byte pixel value into an int.
    }

    /**
     * In JAVA, a byte is always considered as signed when converted
     * to another type (-127 to 127).
     * But aibo use bytes to represent YUV values (0-255).
     * This method converts a signed java byte to unsigned 0-255
     *
     * @param b  The Java byte to be converted
     * @return   An integer containing the unsigned value
     */
    public static int signedByteToUnsigned(byte b){
        if (b>=0)
            return b;
        return ((int)b+255);
    }

}
