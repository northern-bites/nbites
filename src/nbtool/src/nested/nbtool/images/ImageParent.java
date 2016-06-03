package nbtool.images;

import java.awt.image.BufferedImage;

public abstract class ImageParent {
	
	//IN PIXELS
	public int width;
	public int height;
	
	public byte[] data;
	
	public ImageParent(int w, int h, byte[] d) {
		this.width = w; this.height = h; this.data = d;
	}
	
	public int pixelAt(int x, int y) {
		return data[width*y + x] & 0xFF;
	}
	
	public abstract BufferedImage toBufferedImage();
	public abstract String encoding();
	//example: [Y8(U8/V8)]
	//		   [Y8]
	//		   [Y16]
	//		   [U8V8]
	//		   [R8G8B8]
	//		   [A8R8G8B8]
}
