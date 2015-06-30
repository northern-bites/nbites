package nbtool.images;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;

import nbtool.util.Logger;

public class EdgeImage extends ImageParent {

	public EdgeImage(int w, int h, byte[] d) {
		super(w, h, d);
	}

	
	private static final Color[] angleMap = initColorMap();
	
	private static Color[] initColorMap(){
		
		Color[] ret = new Color[256];
		Color[] top = {Color.RED,
				Color.ORANGE,
				Color.YELLOW,
				Color.GREEN,
				Color.BLUE,
				Color.MAGENTA,
				Color.GRAY,
				Color.PINK};
		assert(top.length == 8);
		
		for (int i = 0; i < 256; ++i) {
			int bi = i / 32;
			int ni = (bi + 1) % 8;
			
			int dist = i % 32;
			
			Color bc = top[bi];
			Color nc = top[ni];
			
			int dr = nc.getRed() - bc.getRed();
			int dg = nc.getGreen() - bc.getGreen();
			int db = nc.getBlue() - bc.getBlue();
			
			Color tc = new Color(
					bc.getRed() + (dist * dr) / 32,
					bc.getGreen() + (dist * dg) / 32,
					bc.getBlue() + (dist * db) / 32
					);
			ret[i] = tc;
		}
		
		return ret;
	};

	@Override
	public BufferedImage toBufferedImage() {

		BufferedImage ret = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
		
		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				ret.setRGB(x, y, Color.BLACK.getRGB());
			}
		}

		try {
			DataInputStream dis = new DataInputStream(new ByteArrayInputStream(data));
			int n_edges = data.length / (4 * 4);
			Logger.logf(Logger.INFO, "%d edges expected.", n_edges);
			
			for (int i = 0; i < n_edges; ++i) {
				int x = dis.readInt();
				int y = dis.readInt();
				int mag = dis.readInt();
				int ang = dis.readInt();
				
				Color base = angleMap[ang];
				ret.setRGB(x, y, base.getRGB());
			}

		} catch (Exception e) {
			Logger.logf(Logger.ERROR, "Conversion from bytes to EdgeImage to BufferedImage failed.");
			e.printStackTrace();
			return null;
		}

		return ret;
	}

	@Override
	public String encoding() {
		return "n/a";
	}

}
