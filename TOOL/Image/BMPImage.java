package TOOL.Image;    

import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.io.DataInputStream;
import java.io.InputStream;
import java.io.EOFException;
import java.io.IOException;
import java.awt.image.MemoryImageSource;
import java.io.FileInputStream;
import java.awt.Component;
import javax.swing.JButton;
import javax.swing.*;

import TOOL.TOOL;

/** 
 *
 * Reads in uncompressed bitmaps.  All loading code taken or modified from
 * http://www.javaworld.com/javaworld/javatips/jw-javatip43.html
 *
 * Created so that we can calibrate off of GermanTeam pictures.
 * @author Nicholas Dunn
 * @date 4/17/08
 */ 
public class BMPImage extends RGBImage {
    protected static final boolean DEBUG = false;
    protected int[] tempPixels;
    

    public static final int R = 0;
    public static final int G = 1;
    public static final int B = 2;

    public static final int BLUE_MASK = (1<<8) - 1;
    public static final int GREEN_MASK = BLUE_MASK << 8;
    public static final int RED_MASK = GREEN_MASK << 8;


    public BMPImage(InputStream input, int width, int height) 
        throws IOException {
        super(new DataInputStream(input), width, height);
        tempPixels = loadbitmap(input);
        pixels = new byte[height][width][COLOR_DEPTH];
        readPixels();
    }

    private void readPixels() {
        if (tempPixels == null) {
            TOOL.CONSOLE.error("Pixels array was null in " +
                               "BMPImage::readPixels()");
            return;
        }
        int rgb_val;

        for (int r = 0; r < getHeight(); r++) {
            for (int c = 0; c < getWidth(); c++) {
                // rgb is an int with left most 8 bits representing alpha,
                // next 8 bits being red, next 8 bits being green, then
                // final 8 bits being blue.
                rgb_val = tempPixels[r * getWidth() + c];

                pixels[r][c][0] = (byte) ((rgb_val & RED_MASK)   >> 16);
                pixels[r][c][1] = (byte) ((rgb_val & GREEN_MASK) >>  8);
                pixels[r][c][2] = (byte) ( rgb_val & BLUE_MASK        );
            }
        }

        // No longer needed
        tempPixels = null;
    }

    public void initImage(BufferedImage img) {
        
        byte[] rgb = new byte[COLOR_DEPTH];

        for (int r = 0; r < getHeight(); r++) {
            for (int c = 0; c < getWidth(); c++) {
                Color color = new Color(pixels[r][c][0],
                                        pixels[r][c][1],
                                        pixels[r][c][2]);
                // Change the pixel in the image
                img.setRGB(c, r, color.getRGB());
            }
        }
    }
    
    public static int getWidth(InputStream fs) {
        if (!fs.markSupported()) { 
            System.err.println("Error: BMPImage::getWidth(): fs does not support necessary mark operation");
            return 0;
        }

        try {
            // Allow up to 100 bytes to be read before resetting file position
            fs.mark(100);

            int bflen=14;  // 14 byte BITMAPFILEHEADER
            byte bf[]=new byte[bflen];
            fs.read(bf,0,bflen);
            int bilen=40; // 40-byte BITMAPINFOHEADER
            byte bi[]=new byte[bilen];
            fs.read(bi,0,bilen);
          
            int nwidth = (((int)bi[7]&0xff)<<24)
                | (((int)bi[6]&0xff)<<16)
                | (((int)bi[5]&0xff)<<8)
                | (int)bi[4]&0xff;

            // Puts file pointer back to where we started
            fs.reset();

            return nwidth;
        }
        catch (java.io.IOException e) {
            System.err.println("Error in BMPImage::getWidth()");
            e.printStackTrace();
            return 0;
        }
    }
    public static int getHeight(InputStream fs) {
        if (!fs.markSupported()) { 
            System.err.println("Error: BMPImage::getWidth(): fs does not support necessary mark operation");
            return 0;
        }

        try {
            // Allow up to 100 bytes to be read before resetting file position
            fs.mark(100);

            int bflen=14;  // 14 byte BITMAPFILEHEADER
            byte bf[]=new byte[bflen];
            fs.read(bf,0,bflen);
            int bilen=40; // 40-byte BITMAPINFOHEADER
            byte bi[]=new byte[bilen];
            fs.read(bi,0,bilen);
          
            int nheight = (((int)bi[11]&0xff)<<24)
                | (((int)bi[10]&0xff)<<16)
                | (((int)bi[9]&0xff)<<8)
                | (int)bi[8]&0xff;
            // Puts file pointer back to where we started
            fs.reset();

            return nheight;
        }
        catch (java.io.IOException e) {
            System.err.println("Error in BMPImage::getHeight()");
            e.printStackTrace();
            return 0;
        }
    }




    /**
     * Cleaned up the indentation and fixed a few typos.  Moved some print
     * statements around and enclosed in brackets.  Changed from returning
     * an image to the array of data.
     * Original comments:
     *
    loadbitmap() method converted from Windows C code.  Reads only
    uncompressed 24- and 8-bit images.  Tested with images saved using
    Microsoft Paint in Windows 95.  If the image is not a 24- or 8-bit
    image, the program refuses to even try.  I guess one could include
    4-bit images by masking the byte by first 1100 and then 0011.  I
    am not really interested in such images.  If a compressed image is
    attempted, the routine will probably fail by generating an
    IOException.  Look for variable ncompression to be different from
    0 to indicate compression is present.  Arguments: sdir and sfile
    are the result of the FileDialog() getDirectory() and getFile()
    methods.  Returns: Image Object, be sure to check for (Image)null
    !!!!
    */
    public static int[] loadbitmap(InputStream fs) {
        
        try {
            int bflen=14;  // 14 byte BITMAPFILEHEADER
            byte bf[]=new byte[bflen];
            fs.read(bf,0,bflen);
            int bilen=40; // 40-byte BITMAPINFOHEADER
            byte bi[]=new byte[bilen];
            fs.read(bi,0,bilen);
            // Interperet data.
            int nsize = (((int)bf[5]&0xff)<<24) 
                | (((int)bf[4]&0xff)<<16)
                | (((int)bf[3]&0xff)<<8)
                | (int)bf[2]&0xff;
            
            int nbisize = (((int)bi[3]&0xff)<<24)
                | (((int)bi[2]&0xff)<<16)
                | (((int)bi[1]&0xff)<<8)
                | (int)bi[0]&0xff;
            int nwidth = (((int)bi[7]&0xff)<<24)
                | (((int)bi[6]&0xff)<<16)
                | (((int)bi[5]&0xff)<<8)
                | (int)bi[4]&0xff;
            int nheight = (((int)bi[11]&0xff)<<24)
                | (((int)bi[10]&0xff)<<16)
                | (((int)bi[9]&0xff)<<8)
                | (int)bi[8]&0xff;
            int nplanes = (((int)bi[13]&0xff)<<8) | (int)bi[12]&0xff;
            int nbitcount = (((int)bi[15]&0xff)<<8) | (int)bi[14]&0xff;
            
            

            // Look for non-zero values to indicate compression
            int ncompression = (((int)bi[19])<<24)
                | (((int)bi[18])<<16)
                | (((int)bi[17])<<8)
                | (int)bi[16];
            int nsizeimage = (((int)bi[23]&0xff)<<24)
                | (((int)bi[22]&0xff)<<16)
                | (((int)bi[21]&0xff)<<8)
                | (int)bi[20]&0xff;
            int nxpm = (((int)bi[27]&0xff)<<24)
                | (((int)bi[26]&0xff)<<16)
                | (((int)bi[25]&0xff)<<8)
                | (int)bi[24]&0xff;
            int nypm = (((int)bi[31]&0xff)<<24)
                | (((int)bi[30]&0xff)<<16)
                | (((int)bi[29]&0xff)<<8)
                | (int)bi[28]&0xff;
            int nclrused = (((int)bi[35]&0xff)<<24)
                | (((int)bi[34]&0xff)<<16)
                | (((int)bi[33]&0xff)<<8)
                | (int)bi[32]&0xff;
            int nclrimp = (((int)bi[39]&0xff)<<24)
                | (((int)bi[38]&0xff)<<16)
                | (((int)bi[37]&0xff)<<8)
                | (int)bi[36]&0xff;
            

            if (DEBUG) {
                System.out.println("File type is :"+(char)bf[0]+(char)bf[1]);
                System.out.println("Size of file is :"+nsize);
                System.out.println("Size of bitmapinfoheader is :"+nbisize);
                System.out.println("Width is :"+nwidth);
                System.out.println("Height is :"+nheight);
                System.out.println("Planes is :"+nplanes);
                System.out.println("BitCount is :"+nbitcount);
                System.out.println("Compression is :"+ncompression);
                System.out.println("SizeImage is :"+nsizeimage);
                System.out.println("X-Pixels per meter is :"+nxpm);
                System.out.println("Y-Pixels per meter is :"+nypm);
                System.out.println("Colors used are :"+nclrused);
                System.out.println("Colors important are :"+nclrimp);
            }


            if (nbitcount == 32) {
                return read32Bit(fs, nwidth, nheight);
            }


            else if (nbitcount==24) {
                
                // No Palatte data for 24-bit format but scan lines are
                // padded out to even 4-byte boundaries.
                int npad = (nsizeimage / nheight) - nwidth * 3;
                int ndata[] = new int [nheight * nwidth];
                byte brgb[] = new byte [( nwidth + npad) * 3 * nheight];
                fs.read (brgb, 0, (nwidth + npad) * 3 * nheight);
                int nindex = 0;
                for (int j = 0; j < nheight; j++) {
                    for (int i = 0; i < nwidth; i++) {
                        ndata [nwidth * (nheight - j - 1) + i] =
                            (255&0xff)<<24
                            | (((int)brgb[nindex+2]&0xff)<<16)
                            | (((int)brgb[nindex+1]&0xff)<<8)
                            | (int)brgb[nindex]&0xff;
                        if (DEBUG) {
                            System.out.println("Encoded Color at ("
                                               +i+","+j+")is:"+brgb+" (R,G,B)= ("
                                               +((int)(brgb[2]) & 0xff)+","
                                               +((int)brgb[1]&0xff)+","
                                               +((int)brgb[0]&0xff)+")");
                        }
                        nindex += 3;
                    }
                    nindex += npad;
                }
                return ndata;
                
            }
            else if (nbitcount == 8) {
                // Have to determine the number of colors, the clrsused
                // parameter is dominant if it is greater than zero.  If
                // zero, calculate colors based on bitsperpixel.
                int nNumColors = 0;
                if (nclrused > 0) {
                    nNumColors = nclrused;
                }
                else {
                    nNumColors = (1&0xff)<<nbitcount;
                }
                System.out.println("The number of Colors is"+nNumColors);
                // Some bitmaps do not have the sizeimage field calculated
                // Ferret out these cases and fix 'em.
                if (nsizeimage == 0) {
                    nsizeimage = ((((nwidth*nbitcount)+31) & 31 ) >> 3);
                    nsizeimage *= nheight;
                    System.out.println("nsizeimage (backup) is"+nsizeimage);
                }
                // Read the palatte colors.
                int  npalette[] = new int [nNumColors];
                byte bpalette[] = new byte [nNumColors*4];
                fs.read (bpalette, 0, nNumColors*4);
                int nindex8 = 0;
                for (int n = 0; n < nNumColors; n++) {
                    npalette[n] = (255&0xff)<<24
                        | (((int)bpalette[nindex8+2]&0xff)<<16)
                        | (((int)bpalette[nindex8+1]&0xff)<<8)
                        | (int)bpalette[nindex8]&0xff;
                    if (DEBUG) {
                        System.out.println ("Palette Color "+n
                                            +" is:"+npalette[n]+" (res,R,G,B)= ("
                                            +((int)(bpalette[nindex8+3]) & 0xff)+","
                                            +((int)(bpalette[nindex8+2]) & 0xff)+","
                                            +((int)bpalette[nindex8+1]&0xff)+","
                                            +((int)bpalette[nindex8]&0xff)+")");
                    }
                    nindex8 += 4;
                }
                // Read the image data (actually indices into the palette)
                // Scan lines are still padded out to even 4-byte
                // boundaries.
                int npad8 = (nsizeimage / nheight) - nwidth;
                if (DEBUG) { System.out.println("nPad is:"+npad8); }
                int  ndata8[] = new int [nwidth*nheight];
                byte bdata[] = new byte [(nwidth+npad8)*nheight];
                fs.read (bdata, 0, (nwidth+npad8)*nheight);
                nindex8 = 0;
                for (int j8 = 0; j8 < nheight; j8++) {
                    for (int i8 = 0; i8 < nwidth; i8++) {
                        ndata8 [nwidth*(nheight-j8-1)+i8] =
                            npalette [((int)bdata[nindex8]&0xff)];
                        nindex8++;
                    }
                    nindex8 += npad8;
                }
                
                return ndata8;
                
            }
            else {
                System.err.println ("Not a 32-bit, 24-bit or 8-bit Windows Bitmap, aborting...");
                System.err.println("Found " + nbitcount + " bit color depth.");
            }
            fs.close();
            return null;
        }
        catch (Exception e) {
            System.err.println("Caught exception in loadbitmap!");
        }
        
        return null;
    }

    /**
     * Reads in the data stored in fs; header should already be skipped by
     * the time this method is called.
     */
    public static int[] read32Bit(InputStream fs, int nwidth, int nheight) {
        int ndata[] = new int [nheight * nwidth];
        byte brgb[] = new byte[nwidth *  nheight * 4];
        try {
            fs.read (brgb, 0, nwidth * nheight * 4);
        }
        catch (java.io.IOException e) {
            System.err.println("Error, could not read in the correct " +
                               "number of bits in BMPImage::read32Bits()");
            return null;
        }

        int nindex = 0;
        for (int j = 0; j < nheight; j++) {
            for (int i = 0; i < nwidth; i++) {
                ndata [nwidth * (nheight - j - 1) + i] =
                    (255&0xff)<<24
                    | (((int)brgb[nindex+2]&0xff)<<16)
                    | (((int)brgb[nindex+1]&0xff)<<8)
                    | (int)brgb[nindex]&0xff;
                if (DEBUG) {
                    System.out.println("Encoded Color at ("
                                       +i+","+j+")is:"+brgb+" (R,G,B)= ("
                                       +((int)(brgb[2]) & 0xff)+","
                                       +((int)brgb[1]&0xff)+","
                                       +((int)brgb[0]&0xff)+")");
                }
                nindex += 4;
            };
        }
        return ndata;


    }

}
