package nbtool.images;

import java.awt.image.BufferedImage;

public final class Y16image extends ImageParent {

	public int pixelSize() {return 2;}
	public Y16image(int w, int h, byte[] d) {
		super(w, h, d);
	}

	public BufferedImage toBufferedImage() {
		// TODO Auto-generated method stub
		return null;
	}

	public String encoding() {
		// TODO Auto-generated method stub
		return "[Y16]";
	}
}
