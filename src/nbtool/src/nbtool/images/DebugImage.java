package nbtool.images;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;

import nbtool.util.Logger;
import nbtool.images.Y8image;

public class DebugImage extends ImageParent {

	BufferedImage originalImage;

	public DebugImage(int w, int h, byte[] d, BufferedImage o) {
		super(w, h, d);
		originalImage = o;
	}


	private static final Color[] colorMap = initColorMap();

	private static Color[] initColorMap(){

		Color[] ret = {
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
		return ret;
	};

	@Override
	public BufferedImage toBufferedImage() {

		BufferedImage ret = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
		System.out.println("Width "+width+" "+height);

		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				ret.setRGB(x, y, originalImage.getRGB(x*2, y*2));
			}
		}
		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				if (data[y * width + x] > 0) {
					int col = data[y * width + x];
					if (col < 10 && col >= 0) {
						ret.setRGB(x, y / 2, colorMap[data[y * width + x]].getRGB());
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
