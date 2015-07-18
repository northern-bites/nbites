package nbtool.images;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;

import nbtool.util.Logger;
import nbtool.images.Y8image;

/* DebugImage class. Unlike our other image classes, this one is
 * a combination of multiple images - one a regular image, the other
 * an overlay with drawing colors specified.
 */

public class DebugImage extends ImageParent {

	BufferedImage originalImage;

	/* Constructor takes the width, height, the original image,
	 * and an overlay drawing image.
	 */
	public DebugImage(int w, int h, byte[] d, BufferedImage o) {
		super(w, h, d);
		originalImage = o;
	}

	// Table to convert values in overlay into drawing colors
	private static final Color[] colorMap = {
			Color.WHITE,              // Will not actually be used
			Color.BLACK,
			Color.WHITE,
			Color.RED,
			Color.ORANGE,
			Color.YELLOW,
			Color.GREEN,
			Color.BLUE,
			Color.MAGENTA,
			Color.GRAY,
			Color.PINK};

	@Override
	public BufferedImage toBufferedImage() {

		BufferedImage ret = new BufferedImage(width, height,
											  BufferedImage.TYPE_INT_ARGB);

		// first just set up the image as the original image
		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				// the original image is extra large
				ret.setRGB(x, y, originalImage.getRGB(x*2, y*2));
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
						ret.setRGB(x, y/2,
								   colorMap[data[y * width + x]].getRGB());
					} else {
						ret.setRGB(x, y, Color.BLACK.getRGB());
					}
				}
			}
		}
		return ret;
	}

	@Override
	public String encoding() {
		return "n/a";
	}

}
