package nbtool.images;

import java.awt.Color;
import java.awt.image.BufferedImage;

public final class Y16image extends ImageParent {

	public int pixelSize() {return 2;}
	public Y16image(int w, int h, byte[] d) {
		super(w, h, d);
	}

	public BufferedImage toBufferedImage() {
   	    BufferedImage ret = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
		System.out.println("height " + height + " width " + width);


		for (int r = 0; r < height; r++) {
		    for (int c = 0; c < width; c++) {
			//int y = (data[r*width*2 + 2*c]) & 0xFF;//<< 8 + data[r*width + c + 1];
			int y = (data[r*width*2 + 2*c] & 0xFF) + (data[r*width*2 + 2*c + 1] << 8);
			
			y = y >> 2;
			// y must but in the range 0-255
			Color color = new Color(y, y, y);
			ret.setRGB(c, r, color.getRGB());
		    }
		}
		return ret;
	}

	public String encoding() {
		// TODO Auto-generated method stub
		return "[Y16]";
	}
}
