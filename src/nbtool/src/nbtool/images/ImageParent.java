package nbtool.images;

import java.awt.image.BufferedImage;

public abstract class ImageParent {
	
	//IN PIXELS
	public int width;
	public int height;
	
	//IN BYTES (has to be method so that subclasses can/must set.  Silly java)
	public abstract int pixelSize();
	
	public byte[] data;
	
	public ImageParent(int w, int h, byte[] d) {
		this.width = w; this.height = h; this.data = d;
	}
	
	public int rowLength() {return pixelSize() * width;}
	public byte[] pixelAt(int x, int y) {
		byte[] ret = new byte[pixelSize()];
		System.arraycopy(data, x * pixelSize() + (y * width * pixelSize()), ret, 0, pixelSize());
		
		return ret;
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
