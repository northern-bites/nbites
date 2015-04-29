package nbtool.images;

import java.awt.image.BufferedImage;

public final class UV88image extends ImageParent{
	public int pixelSize() {return 2;}
	public UV88image(int w, int h, byte[] d) {
		super(w, h, d);
		// TODO Auto-generated constructor stub
	}

	//UNDEFINED
	public BufferedImage toBufferedImage() {
		return null;
	}

	public String encoding() {
		// TODO Auto-generated method stub
		return "[U8V8]";
	}

}
