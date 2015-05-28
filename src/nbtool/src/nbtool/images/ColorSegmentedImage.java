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

        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; c += 2) {
                
                byte i = data[r * width * pixelSize() + (c * pixelSize())];

                ret.setRGB(c, r, 16711680);
                ret.setRGB(c + 1, r, 16711680);
            }
        }
        return ret;
    }

    @Override
    public String encoding() {
        return "[ColorSegmented]";
    }
}