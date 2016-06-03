package nbtool.gui.logviews.images;

import java.awt.image.WritableRaster;

public class StructuringElement {
	int[][] indices;
	
	public StructuringElement(int which) {
		if (which == 0) { // cross
			indices = new int[5][2];
			indices[0][0] = 0;
			indices[0][1] = 0;
			indices[1][0] = 0;
			indices[1][1] = -1;
			indices[2][0] = 0;
			indices[2][1] = 1;
			indices[3][0] = -1;
			indices[3][1] = 0;
			indices[4][0] = 1;
			indices[4][1] = 0;
		}
		else if (which == 1) { // vertical band of 3 pixels
			indices = new int[3][2];
			indices[0][0] = 0;
			indices[0][1] = 0;
			indices[1][0] = 0;
			indices[1][1] = -1;
			indices[2][0] = 0;
			indices[2][1] = 1;
		}
		else if (which == 2) { // vertical band of 5 pixels
			indices = new int[5][2];
			indices[0][0] = 0;
			indices[0][1] = 0;
			indices[1][0] = 0;
			indices[1][1] = -1;
			indices[2][0] = 0;
			indices[2][1] = 1;
			indices[3][0] = 0;
			indices[3][1] = -2;
			indices[4][0] = 0;
			indices[4][1] = 2;
		}
		else if (which == 3) { // horizontal band
			indices = new int[3][2];
			indices[0][0] = 0;
			indices[0][1] = 0;
			indices[1][0] = -1;
			indices[1][1] = 0;
			indices[2][0] = 1;
			indices[2][1] = 0;
		}
		else if (which == 4) { // 3x3 square
			indices = new int[9][2];
			indices[0][0] = 0;
			indices[0][1] = 0;
			indices[1][0] = 0;
			indices[1][1] = -1;
			indices[2][0] = 0;
			indices[2][1] = 1;
			indices[3][0] = -1;
			indices[3][1] = 0;
			indices[4][0] = 1;
			indices[4][1] = 0;
			indices[5][0] = -1;
			indices[5][1] = -1;
			indices[6][0] = -1;
			indices[6][0] = 1;
			indices[7][0] = 1;
			indices[7][0] = -1;
			indices[8][0] = 1;
			indices[8][0] = 1;
		}
		else { // identity
			indices = new int[1][2];
			indices[0][0] = 0;
			indices[0][1] = 0;
		}
	}
	
	public StructuringElement(int[][] indices_) {
		indices = indices_;
	}
	
	public int min(WritableRaster r, int x, int y) {
		double min = 255;
		
		for (int[] index : indices) {
			double[] pixel = new double[1];
			
			try {
				pixel = r.getPixel(x + index[0], y + index[1], pixel);
			} catch (java.lang.ArrayIndexOutOfBoundsException e) {
				pixel[0] = 255;
			}
			
			if (pixel[0] < min)
				min = pixel[0];
		}
		
		return (int)min;
	}
	
	public int max(WritableRaster r, int x, int y) {
		double max = 0;
		
		for (int[] index : indices) {
			double[] pixel = new double[1];
			
			try {
				pixel = r.getPixel(x + index[0], y + index[1], pixel);
			} catch (java.lang.ArrayIndexOutOfBoundsException e) {
				pixel[0] = 0;
			}
			
			if (pixel[0] > max)
				max = pixel[0];
		}
		
		return (int)max;
	}
}