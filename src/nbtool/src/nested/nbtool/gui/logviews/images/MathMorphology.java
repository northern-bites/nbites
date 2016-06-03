package nbtool.gui.logviews.images;

import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;

public class MathMorphology {
	StructuringElement s;
	
	public MathMorphology(StructuringElement s_) {
		s = s_;
	}
	
	public void setStructuringElement(StructuringElement s_) {
		s = s_;
	}
	
	public BufferedImage erosion(BufferedImage img) {
		WritableRaster forReading = img.copyData(null);
		WritableRaster forWriting = img.getRaster();
		for (int y = 0; y < forReading.getHeight(); y++) { 
			for (int x = 0; x < forReading.getWidth(); x++) {
				int[] pixel = new int[1];
				pixel[0] = s.min(forReading, x, y);
				forWriting.setPixel(x, y, pixel);
			}
		}
		return img;
	}
	
	public BufferedImage dilation(BufferedImage img) {
		WritableRaster forReading = img.copyData(null);
		WritableRaster forWriting = img.getRaster();
		for (int y = 0; y < forReading.getHeight(); y++) { 
			for (int x = 0; x < forReading.getWidth(); x++) {
				int[] pixel = new int[1];
				pixel[0] = s.max(forReading, x, y);
				forWriting.setPixel(x, y, pixel);
			}
		}
		return img;
	}
	
	public BufferedImage opening(BufferedImage img) {
		return dilation(erosion(img));
	}
	
	public BufferedImage closing(BufferedImage img) {
		return erosion(dilation(img));
	}
}
