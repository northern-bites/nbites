package nbtool.gui.logviews.images;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.util.Vector;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.images.YUYV8888image;
import nbtool.util.U;

// TODO add via layouts?
// TODO refactor GradientCalculator into a Gradient class
// TODO rename PostPanel
// TODO directory structure
// TODO more refactoring

public class PostPanel extends ViewParent {
	YUYV8888image original;
	
	BufferedImage gradient;
	BufferedImage yellow;
	BufferedImage field;
	BufferedImage post;
	
	double[] scores;
	WeightedFit line;
	
	public void setLog(Log newlog) {
		log = newlog;
		
		original = (YUYV8888image) U.imageFromLog(newlog);
		        
        gradient = buildGradientImg(original);
		StructuringElement cross = new StructuringElement(0);
        MathMorphology mm = new MathMorphology(cross); 
        gradient = mm.opening(gradient);
        
		yellow = buildYellowImg(original);
		StructuringElement verticalBandOf5Pixels = new StructuringElement(2);
		mm.setStructuringElement(verticalBandOf5Pixels);
		yellow = mm.opening(yellow);
        
        field = buildFieldImg(original);
        StructuringElement square = new StructuringElement(4);
		mm.setStructuringElement(square);
		field = mm.opening(field);
        
        FuzzyThreshold sigma = new FuzzyThreshold(100, 150);
        post = GrayscaleLib.threshold(GrayscaleLib.subtract(GrayscaleLib.add(gradient, yellow), field), sigma);
        
        scores = buildHistogram(post);
        line = detectPeaks(scores);
		
		repaint();
	}
	
	public void paintComponent(Graphics g) {
		if (original == null)
			return;
		
		BufferedImage bOriginal = original.toBufferedImage();
		g.drawImage(bOriginal, bOriginal.getWidth(), 0, null);
		g.drawImage(gradient, 0, 0, null);
		//g.drawImage(yellow, 0, 2*bOriginal.getHeight(), null);
		g.drawImage(field, bOriginal.getWidth(), bOriginal.getHeight(), null);
		g.drawImage(post, 0, bOriginal.getHeight(), null);
		
//        int barWidth = bOriginal.getWidth() / scores.length;
//        for(int i = 0; i < scores.length; i++){
//            int barHeight = (int)(scores[i]);
//            g.fillRect(bOriginal.getWidth() + i*barWidth, 
//            		   bOriginal.getHeight() + (int)(bOriginal.getHeight()-scores[i]), 
//            		   barWidth,
//            		   barHeight);
//        }
    }
	
	protected void useSize(Dimension s) {}
	
	public static Boolean shouldLoadInParallel() { return true; }
	
	public PostPanel() {
		super();
		
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		setLayout(null);
	}

	private double calculateColorScore(int idealU, int idealV, int u, int v) {
		FuzzyThreshold sigmaColor = new FuzzyThreshold(30, 50);
		
		u -= 128;
		v -= 128;
		
		int uDiff = java.lang.Math.abs(idealU - u);
		int vDiff = java.lang.Math.abs(idealV - v);
		int diff = (int) java.lang.Math.sqrt(uDiff*uDiff + vDiff*vDiff);
		
		return -1*(sigmaColor.f((double)diff) - 1);
	}

	private BufferedImage buildYellowImg(YUYV8888image yuvImg) {
		BufferedImage yellowImg = new BufferedImage(yuvImg.width / 2 - 1, yuvImg.height - 1, BufferedImage.TYPE_BYTE_GRAY);
    	WritableRaster raster = yellowImg.getRaster();
        for (int i = 1; i < yuvImg.height - 1; i++) {
			for (int j = 1; j < (yuvImg.width / 2 - 1); j += 1) {
				int[] pixel = new int[1];
				int u = yuvImg.uPixelAt(j, i);
				int v = yuvImg.vPixelAt(j, i);
				pixel[0] = (int)(255*calculateColorScore(-80, 10, u, v));
				raster.setPixel(j-1, i-1, pixel);
			}
		}
        return yellowImg;
	}
	
	private double calculateGradientScore(double[] grad) {
		FuzzyThreshold sigmaMagnitude = new FuzzyThreshold(3, 7);
		FuzzyThreshold sigmaDirection = new FuzzyThreshold(-55, -35);
		
		double magn = GradientCalculator.calculateGradMagn(grad);
		double dir = GradientCalculator.calculateGradDir(grad);
		if (dir > 90) dir -= 180;
		if (dir > 0) dir *= -1;
		
		return sigmaMagnitude.f(magn)*sigmaDirection.f(dir);
	}
	
	private BufferedImage buildGradientImg(YUYV8888image yuvImg) {
		BufferedImage gradientImg = new BufferedImage(yuvImg.width / 2 - 1, yuvImg.height - 1, BufferedImage.TYPE_BYTE_GRAY);
		WritableRaster raster = gradientImg.getRaster();
        for (int i = 1; i < yuvImg.height - 1; i++) {
			for (int j = 1; j < (yuvImg.width / 2 - 1); j += 1) {
				int[] pixel = new int[1];
				double[] grad = GradientCalculator.calculateGrad(yuvImg.yPixelsCenteredAt(j, i));
				pixel[0] = (int)(255*calculateGradientScore(grad));
				raster.setPixel(j-1, i-1, pixel);
			}
		}
        return gradientImg;
	}
	
	private double calculateFieldScore(int yMode, int y) {
		int diff = java.lang.Math.abs(y - yMode);
		FuzzyThreshold sigma = new FuzzyThreshold(20, 40);
		return -1*(sigma.f((double)diff) - 1);
	}
	
	private int calculateYMode(YUYV8888image yuvImg) {
		int[] frequencies = new int[256];
        for (int i = yuvImg.height / 2; i < yuvImg.height; i++) {
			for (int j = 0; j < (yuvImg.width / 2); j += 1) {
				frequencies[yuvImg.yPixelAt(j, i)] += 1;
			}
        }
        
        int mode = 0, frequencyOfMode = 0;
        for (int i = 0; i < 256; i++) {
        	if (frequencies[i] > frequencyOfMode) {
        		mode = i;
        		frequencyOfMode = frequencies[i];
        	}
        }
        return mode;
	}
	
	private BufferedImage buildFieldImg(YUYV8888image yuvImg) {
		BufferedImage gradientImg = new BufferedImage(yuvImg.width / 2 - 1, yuvImg.height - 1, BufferedImage.TYPE_BYTE_GRAY);
		int yMode = calculateYMode(yuvImg);
		WritableRaster raster = gradientImg.getRaster();
        for (int i = 1; i < yuvImg.height - 1; i++) {
			for (int j = 1; j < (yuvImg.width / 2 - 1); j += 1) {
				int[] pixel = new int[1];
				int y = yuvImg.yPixelAt(j, i);
				pixel[0] = (int)(255*calculateFieldScore(yMode, y));
				raster.setPixel(j-1, i-1, pixel);
			}
		}
        return gradientImg;
	}
	
	private double[] buildHistogram(BufferedImage postImg) {
		Raster raster = postImg.getData();
		int width = postImg.getWidth();
		int height = postImg.getHeight();
		double[] hist = new double[width];
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				int[] pixel = new int[1];
				raster.getPixel(x, y, pixel);
				hist[x] += pixel[0];
			}
			hist[x] /= height;
		}
		return hist;
	}
	
	private WeightedFit detectPeaks(double[] scores) {
		double totalDensity = 0;
		WeightedFit line = new WeightedFit();
		for (int i = 0; i < scores.length - 1; i++) {
			totalDensity += scores[i];
			line.add(i, scores[i]);
		}
		double m = line.getFirstPrincipalAxisV() / line.getFirstPrincipalAxisU();
		
		boolean stop = false;
		for (int i = 255; i >= 0; i--) {
			Vector<int[]> peaks = new Vector<int[]>();
			boolean inPeak = false;
			int peakIndex = -1;
			for (int j = 0; j < scores.length - 1; j++) {
				int yOnLine = (int)(m*j - m*line.getCenterX() + line.getCenterY() + i);
				if (!inPeak && scores[j] > yOnLine) {
					inPeak = true;
					int[] peak = new int[3];
					peak[0] = j;
					peak[2] = 0;
					peak[2] += (int)scores[j] - yOnLine;
					peaks.add(peak);
					peakIndex++;
				}
				else if (inPeak && scores[j] > yOnLine) {
					peaks.get(peakIndex)[2] += (int)scores[j] - yOnLine;
				}
				else if (inPeak) {
					inPeak = false;
					peaks.get(peakIndex)[1] = j;
					if (peaks.get(peakIndex)[2] / totalDensity >= 0.02) {
						stop = true;
					} else if (peaks.get(peakIndex)[2] / totalDensity < 0.003) {
						peaks.remove(peakIndex);
						peakIndex--;
					}
				}
			}
			if (stop) {
				for (int k = 0; k < peaks.size(); k++) {
					scores[peaks.get(k)[0]] = 100000;
					scores[peaks.get(k)[1]] = 100000;
				}
				return line;
			}
		}
		return line;
	}
}