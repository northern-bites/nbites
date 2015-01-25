package nbtool.images;

import java.awt.Color;
import java.awt.image.BufferedImage;

public class Y8image extends ImageParent {

	public int pixelSize() {return 1;}
	public Y8image(int w, int h, byte[] d) {
		super(w, h, d);
		// TODO Auto-generated constructor stub
	}

	@Override
	public BufferedImage toBufferedImage() {
		BufferedImage ret = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
		for (int r = 0; r < height; ++r) {
			for (int c = 0; c < width; ++c) {
				
				int y = (data[r * width + c]) & 0xFF;				
				Color color = new Color(y, y, y);
				ret.setRGB(c, r, color.getRGB());
			}
		}
		return ret;
	}

	@Override
	public String encoding() {
		return "[Y8]";
	}

}
