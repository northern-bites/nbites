package nbtool.gui.logviews.images;

import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;

public class GrayscaleLib {
	public static BufferedImage copy(BufferedImage img) {
        ColorModel cm = img.getColorModel();
        WritableRaster raster = img.copyData(null);
        boolean isAlphaPremultiplied = cm.isAlphaPremultiplied();
        return new BufferedImage(cm, raster, isAlphaPremultiplied, null);
	}

	public static BufferedImage and(BufferedImage img1, BufferedImage img2) {
		BufferedImage anded = copy(img1);
		WritableRaster andedRaster = anded.getRaster();
		Raster img1Raster = img1.getRaster();
		Raster img2Raster = img2.getRaster();
		for (int i = 0; i < img1.getHeight(); i++) {
			for (int j = 0; j < img1.getWidth(); j += 1) {
				int[] img1Pixel = new int[1];
				img1Pixel = img1Raster.getPixel(j, i, img1Pixel);
				int[] img2Pixel = new int[1];
				img2Pixel = img2Raster.getPixel(j, i, img2Pixel);
				int[] newPixel = new int[1];
				newPixel[0] = Math.min(img1Pixel[0], img2Pixel[0]);
				andedRaster.setPixel(j, i, newPixel);
			}
		}
        return anded;
	}
	
	public static BufferedImage or(BufferedImage img1, BufferedImage img2) {
		BufferedImage ored = copy(img1);
		WritableRaster oredRaster = ored.getRaster();
		Raster img1Raster = img1.getRaster();
		Raster img2Raster = img2.getRaster();
		for (int i = 0; i < img1.getHeight(); i++) {
			for (int j = 0; j < img1.getWidth(); j += 1) {
				int[] img1Pixel = new int[1];
				img1Pixel = img1Raster.getPixel(j, i, img1Pixel);
				int[] img2Pixel = new int[1];
				img2Pixel = img2Raster.getPixel(j, i, img2Pixel);
				int[] newPixel = new int[1];
				newPixel[0] = Math.max(img1Pixel[0], img2Pixel[0]);
				oredRaster.setPixel(j, i, newPixel);
			}
		}
        return ored;
	}
	
	public static BufferedImage add(BufferedImage img1, BufferedImage img2) {
		BufferedImage added = copy(img1);
		WritableRaster addedRaster = added.getRaster();
		Raster img1Raster = img1.getRaster();
		Raster img2Raster = img2.getRaster();
		for (int i = 0; i < img1.getHeight(); i++) {
			for (int j = 0; j < img1.getWidth(); j += 1) {
				int[] img1Pixel = new int[1];
				img1Pixel = img1Raster.getPixel(j, i, img1Pixel);
				int[] img2Pixel = new int[1];
				img2Pixel = img2Raster.getPixel(j, i, img2Pixel);
				int[] newPixel = new int[1];
				newPixel[0] = (img1Pixel[0] + img2Pixel[0]) / 2;
				addedRaster.setPixel(j, i, newPixel);
			}
		}
        return added;
	}
	
	public static BufferedImage subtract(BufferedImage img1, BufferedImage img2) {
		BufferedImage subtracted = copy(img1);
		WritableRaster subtractedRaster = subtracted.getRaster();
		Raster img1Raster = img1.getRaster();
		Raster img2Raster = img2.getRaster();
		for (int i = 0; i < img1.getHeight(); i++) {
			for (int j = 0; j < img1.getWidth(); j += 1) {
				int[] img1Pixel = new int[1];
				img1Pixel = img1Raster.getPixel(j, i, img1Pixel);
				int[] img2Pixel = new int[1];
				img2Pixel = img2Raster.getPixel(j, i, img2Pixel);
				int[] newPixel = new int[1];
				newPixel[0] = (img1Pixel[0] - img2Pixel[0]) / 2 + 127;
				subtractedRaster.setPixel(j, i, newPixel);
			}
		}
        return subtracted;
	}
	
	public static BufferedImage threshold(BufferedImage img, FuzzyThreshold sigma) {
		BufferedImage thresholded = copy(img);
		WritableRaster raster = thresholded.getRaster();
		for (int i = 0; i < img.getHeight(); i++) {
			for (int j = 0; j < img.getWidth(); j += 1) {
				int[] oldPixel = new int[1];
				oldPixel = raster.getPixel(j, i, oldPixel);
				int[] newPixel = new int[1];
				newPixel[0] = (int)(255*sigma.f((double)oldPixel[0]));
				raster.setPixel(j, i, newPixel);
			}
		}
        return thresholded;
	}
}
