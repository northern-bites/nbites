package nbtool.images;

import java.awt.Color;
import java.awt.image.BufferedImage;

public class ColorSegmentedImage extends ImageParent {

    public int pixelSize() {return 1;}

    public ColorSegmentedImage(int w, int h, byte[] d) {
        super(w, h, d);
    }

    @Override
    public BufferedImage toBufferedImage() {
        BufferedImage ret = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);

        for (int r = 0; r < height; r++) {
            for (int c = 0; c < width; c++) {
                
                byte i = data[r * width * pixelSize() + (c * pixelSize())];

                ret.setRGB(c, r, getColor(i).getRGB());
            //   System.out.printf("%d, %d\n", c, r);
            }
        }
        return ret;
    }

    @Override
    public String encoding() {
        return "[ColorSegmented]";
    }

    public Color getColor(byte thresh) {
        if (thresh == (byte)0x01) {    // White
            return Color.white;
        } else if (thresh == (byte)0x02) {    // Green
            return Color.green;

        } else if (thresh == (byte)0x04) {    // Blue
            return Color.blue;

        } else if (thresh == (byte)0x08) {    // Yellow
            return Color.yellow;

        } else if (thresh == (byte)0x10) {    // Orange
            return Color.orange;

        } else if (thresh == (byte)0x20) {    // Red
            return Color.red;

        } else if (thresh == (byte)0x40) {    // Navy
            return Color.black;

        } else {
            return Color.black;
        }
    }
}