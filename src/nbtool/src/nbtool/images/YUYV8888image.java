package nbtool.images;

import java.awt.image.BufferedImage;

public final class YUYV8888image extends ImageParent {
	public int pixelSize() {return 2;}

	public YUYV8888image(int w, int h, byte[] d) {
		super(w, h, d);
	}
	
	public int yPixelAt(int x, int y) {
		return pixelAt(2*x, y);
	}
	
	public int uPixelAt(int x, int y) {
		return pixelAt(2*x + 1, y);
	}
	
	public int vPixelAt(int x, int y) {
		return pixelAt(2*x + 3, y);
	}
	
	public int[] yPixelsCenteredAt(int x, int y) {
    	int[] pixels = new int[9];
    	pixels[0] = pixelAt(2*(x-1), y-1);
    	pixels[1] = pixelAt(2*x    , y-1);
    	pixels[2] = pixelAt(2*(x+1), y-1);
    	pixels[3] = pixelAt(2*(x-1), y);
    	pixels[4] = pixelAt(2*x    , y);
    	pixels[5] = pixelAt(2*(x+1), y);
    	pixels[6] = pixelAt(2*(x-1), y+1);
    	pixels[7] = pixelAt(2*x    , y+1);
    	pixels[8] = pixelAt(2*(x+1), y+1);
    	return pixels;
	}
	
	public BufferedImage toBufferedImage() {
		BufferedImage ret = new BufferedImage(width / 2, height, BufferedImage.TYPE_INT_ARGB);
		
		for (int r = 0; r < height; ++r) {
			for (int c = 0; c < width; c += 4) {
				int i = width*r + c;
				
				byte y1 = data[i];
				byte u = data[i + 1];
				byte y2 = data[i + 2];
				byte v = data[i + 3];
				
				ret.setRGB(c / 2, r, yuv444ToARGB888Pixel(y1, u, v));
				ret.setRGB(c / 2 + 1, r, yuv444ToARGB888Pixel(y2, u, v));
			}
		}
		
		return ret;
	}

	public String encoding() {
		return "[Y8(U8/V8)]";
	}
	
	public UV88image toUV() {
		return null;
	}
	
	public Y16image toY16() {
		return null;
	}
	
	public Y8image toY8() {
		int sizeOfY8image = width * height / 2;
		byte[] bytes = new byte[sizeOfY8image];
		for (int i = 0; i < sizeOfY8image; ++i) {
			bytes[i] = this.data[i * 2];
		}
		Y8image ret = new Y8image(width / 2, height, bytes);
		
		return ret;
	}
	
	public static int clamp(int val, int low, int high) {
		return Math.max(low, Math.min(high, val));
	}
	
	public static int yuv444ToARGB888Pixel(byte y, byte u, byte v) {
		int c = (int)(y & 0xFF) - 16;
		int d = (int)(u & 0xFF) - 128;
		int e = (int)(v & 0xFF) - 128;
		
		int r2 = clamp((298*c + 409*e + 128) >> 8, 0, 255);
		int g2 = clamp((298*c - 100*d - 208*e + 128) >> 8, 0, 255);
		int b2 = clamp((298*c + 516*d + 128) >> 8, 0, 255);
		
		return (255 << 24) + (r2 << 16) + (g2 << 8) + (b2);
	}
}
