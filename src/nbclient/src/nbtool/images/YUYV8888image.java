package nbtool.images;

import java.awt.image.BufferedImage;

public final class YUYV8888image extends ImageParent {
	public static final YUYV8888image INSTANCE = new YUYV8888image(0,0, null);
	public int pixelSize() {return 2;}

	public YUYV8888image(int w, int h, byte[] d) {
		super(w, h, d); //Pixel size is two, macropixel size is 4
		// TODO Auto-generated constructor stub
	}

	public BufferedImage toBufferedImage() {
		BufferedImage ret = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
		
		for (int r = 0; r < height; ++r) {
			for (int c = 0; c < width; c += 2) {
				int i = (r * width * pixelSize()) + (c * pixelSize());
				
				byte y1 = data[i];
				byte u = data[i + 1];
				byte y2 = data[i + 2];
				byte v = data[i + 3];
				
				ret.setRGB(c, r, yuv444ToARGB888Pixel(y1, u, v));
				ret.setRGB(c + 1, r, yuv444ToARGB888Pixel(y2, u, v));
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
		byte[] bytes = new byte[this.width * this.height];
		for (int i = 0; i < this.width * this.height; ++i) {
			bytes[i] = this.data[i * 2];
		}
		Y8image ret = new Y8image(this.width, this.height, bytes);
		
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
