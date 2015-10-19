package nbtool.images;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;

import nbtool.util.Logger;
import nbtool.images.Y8image;

public class RobotImage extends ImageParent {

    BufferedImage original;

    /* Constructor takes the width, height, the original image,
     * and an overlay drawing image.
     */
    public RobotImage(int w, int h, byte[] d, BufferedImage o) {
        super(w, h, d);
        original = o;
    }

    @Override
    public BufferedImage toBufferedImage(){
        BufferedImage r_img = new BufferedImage(width, height,
                                                BufferedImage.TYPE_INT_ARGB);

        // first just set up the image as the original image
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                // the original image is extra large
                r_img.setRGB(x, y, original.getRGB(x*2, y*2));
            }
        }
        // now loop through our drawing overlay, painting when nonzero
        int count = 0;
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height * 2; ++y) {
                //if (data[y * width + x] > 0) {
                if (pixelAt(x, y) > 0) {
                    int col = pixelAt(x, y); //data[y * width + x];
                    // if the debug image has a valid color, paint it
                    if (col < 10 && col >= 0) {
                        count++;
                        r_img.setRGB(x, y/2, Color.PINK.getRGB());
                    } else {
                        r_img.setRGB(x, y, Color.BLACK.getRGB());
                    }
                }
            }
        }
        return r_img;


    }

    // why do i need this
    @Override
    public String encoding() {
        return "n/a";
    }

}