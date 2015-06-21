// package nbtool.gui.logviews.images;

// import java.awt.image.BufferedImage;
// import java.awt.image.Raster;
// import java.awt.image.WritableRaster;
// import java.util.ArrayList;
// import java.util.Vector;

// import nbtool.data.Log;
// import nbtool.images.YUYV8888image;
// import nbtool.util.U;

// // TODO better logic when there are more than two detected peaks
// // TODO refactor GradientCalculator into a Gradient class
// public class PostDetector extends Detector {
// 	YUYV8888image original;
	
// 	BufferedImage gradient;
// 	BufferedImage yellow;
// //	BufferedImage field;
// 	BufferedImage post;
	
// 	double[] rawScores;
// 	double[] processedScores;
	
// 	ArrayList<Integer> candidates;
	
// 	public PostDetector(Log log_) {
// 		super(log_);
								
// 		original = (YUYV8888image) U.imageFromLog(log);
		
// 		buildGradientImg();
// //		StructuringElement cross = new StructuringElement(0);
// //      MathMorphology mm = new MathMorphology(cross); 
// //      mm.opening(gradient);
        
// 		buildYellowImg();
// 		StructuringElement verticalBandOf5Pixels = new StructuringElement(2);
//         MathMorphology mm = new MathMorphology(verticalBandOf5Pixels); 
// 		//mm.opening(yellow);
        
// //      buildFieldImg();
// //      StructuringElement square = new StructuringElement(4);
// //		mm.setStructuringElement(square);
// //		mm.opening(field);
        
//         post = GrayscaleLib.and(gradient, yellow);
        
//         buildHistogram();
//         processHistogram();
//         detectPeaks();
// 	}

// 	private double calculateColorScore(int y, int u, int v) {
// 		FuzzyThreshold sigmaY = new FuzzyThreshold(70, 100);
// 		FuzzyThreshold sigmaU = new FuzzyThreshold(110, 150);
// 		FuzzyThreshold sigmaV = new FuzzyThreshold(125, 150);
		
// 		double yScore = sigmaY.f((double) y);
// 		double uScore = 1 - sigmaU.f((double) u);
// 		double vScore = sigmaV.f((double) v);
		
// 		double smallestScore = uScore;
// 		if (yScore < uScore) {
// 			smallestScore = yScore;
// 		}
// 		if (vScore < smallestScore) {
// 			smallestScore = vScore;
// 		}
		
// 		return smallestScore;
// 	}

// 	private void buildYellowImg() {
// 		BufferedImage yellowImg = new BufferedImage(original.width / 2 - 1, original.height - 1, BufferedImage.TYPE_BYTE_GRAY);
//     	WritableRaster raster = yellowImg.getRaster();
//         for (int i = 1; i < original.height - 1; i++) {
// 			for (int j = 1; j < (original.width / 2 - 1); j += 1) {
// 				int[] pixel = new int[1];
// 				int y = original.yPixelAt(j, i);
// 				int u = original.uPixelAt(j, i);
// 				int v = original.vPixelAt(j, i);
// 				pixel[0] = (int)(255*calculateColorScore(y, u, v));
// 				raster.setPixel(j-1, i-1, pixel);
// 			}
// 		}
//         yellow = yellowImg;
// 	}
	
// 	private double calculateGradientScore(double[] grad) {
// 		FuzzyThreshold sigmaMagnitudeLow = new FuzzyThreshold(3, 7);
// 		FuzzyThreshold sigmaDirection = new FuzzyThreshold(-60, -35);
		
// 		double magn = GradientCalculator.calculateGradMagn(grad);
// 		double dir = GradientCalculator.calculateGradDir(grad);
// 		if (dir > 90) dir -= 180;
// 		if (dir > 0) dir *= -1;
		
// 		double magnScoreLow = sigmaMagnitudeLow.f(magn);
// 		double dirScore = sigmaDirection.f(dir);
		
// 		if (magn > 80)
// 			return 0;
		
// 		if (magnScoreLow < dirScore)
// 			return magnScoreLow;
// 		return dirScore;
// 	}
	
// 	private void buildGradientImg() {
// 		BufferedImage gradientImg = new BufferedImage(original.width / 2 - 1, original.height - 1, BufferedImage.TYPE_BYTE_GRAY);
// 		WritableRaster raster = gradientImg.getRaster();
//         for (int i = 1; i < original.height - 1; i++) {
// 			for (int j = 1; j < (original.width / 2 - 1); j += 1) {
// 				int[] pixel = new int[1];
// 				double[] grad = GradientCalculator.calculateGrad(original.yPixelsCenteredAt(j, i));
// 				pixel[0] = (int)(255*calculateGradientScore(grad));
// 				raster.setPixel(j-1, i-1, pixel);
// 			}
// 		}
//         gradient = gradientImg;
// 	}
	
// //	private double calculateFieldScore(int yMode, int y) {
// //		int diff = y - yMode;
// //		if (diff < 0)
// //			return 0.0;
// //		FuzzyThreshold sigma = new FuzzyThreshold(40, 60);
// //		return sigma.f(diff);
// //	}
// //	
// //	private int calculateYMode() {
// //		int[] frequencies = new int[256];
// //        for (int i = original.height / 2; i < original.height; i++) {
// //			for (int j = 0; j < (original.width / 2); j += 1) {
// //				frequencies[original.yPixelAt(j, i)] += 1;
// //			}
// //        }
// //        
// //        int mode = 0, frequencyOfMode = 0;
// //        for (int i = 0; i < 256; i++) {
// //        	if (frequencies[i] > frequencyOfMode) {
// //        		mode = i;
// //        		frequencyOfMode = frequencies[i];
// //        	}
// //        }
// //        return mode;
// //	}
// //	
// //	private void buildFieldImg() {
// //		BufferedImage fieldImg = new BufferedImage(original.width / 2 - 1, original.height - 1, BufferedImage.TYPE_BYTE_GRAY);
// //		int yMode = calculateYMode();
// //		WritableRaster raster = fieldImg.getRaster();
// //        for (int i = 1; i < original.height - 1; i++) {
// //			for (int j = 1; j < (original.width / 2 - 1); j += 1) {
// //				int[] pixel = new int[1];
// //				int y = original.yPixelAt(j, i);
// //				pixel[0] = (int)(255*calculateFieldScore(yMode, y));
// //				raster.setPixel(j-1, i-1, pixel);
// //			}
// //		}
// //        field = fieldImg;
// //	}
	
// 	private void buildHistogram() {
// 		Raster raster = post.getData();
// 		int width = post.getWidth();
// 		int height = post.getHeight();
				
// 		double[] hist = new double[width];
		
// 		for (int x = 25; x < width - 25; x++) {
// //			boolean inRun = false;
// //			double score = 0;
			
// 			for (int y = 0; y < height; y++) {
// 				int[] pixel = new int[1];
// 				raster.getPixel(x, y, pixel);
// 				hist[x] += pixel[0];
				
// //				if (pixel[0] > 0) {
// //					if (!inRun) {
// //						inRun = true;
// //						score += (double) pixel[0];
// //					} else {
// //						score += (double) pixel[0];
// //					}
// //				} else if (inRun) {
// //					inRun = false;
// //					if (score > hist[x])
// //						hist[x] = score;
// //					score = 0;
// //				}
// 			}
// 		}
		
// 		double maxScore = 0;
// 		for (int i = 25; i < width - 25; i++) {
// 			if (hist[i] > maxScore)
// 				maxScore = hist[i];
// 		}
// 		for (int i = 25; i < width - 25; i++) {
// 			hist[i] = hist[i] / maxScore * 255;
// 		}
		
// 		rawScores = hist;
// 	}
	
// 	private void processHistogram() {
// 		double narrowGaussian[] = new double[] {0.000003,	0.000229,	0.005977,	0.060598,	0.24173,	0.382925,	0.24173,	0.060598,	0.005977,	0.000229,	0.000003};
// 		double wideGaussian[] = new double[] {0.01554, 0.015969, 0.016392, 0.016807, 0.017213, 0.017609, 0.017995, 0.018369, 0.018729, 0.019076, 0.019407,
// 										      0.019722, 0.020021, 0.0203, 0.020562, 0.020803, 0.021024, 0.021223, 0.021401, 0.021556, 0.021688, 0.021796,
// 										      0.021881, 0.021942, 0.021979, 0.021991, 0.021979, 0.021942, 0.021881, 0.021796, 0.021688, 0.021556, 0.021401,
// 										      0.021223, 0.021024, 0.020803, 0.020562, 0.0203, 0.020021, 0.019722, 0.019407, 0.019076, 0.018729, 0.018369,
// 										      0.017995, 0.017609, 0.017213, 0.016807, 0.016392, 0.015969, 0.01554};
		
// 		processedScores = new double[rawScores.length];
// 		for (int i = 25; i < processedScores.length - 25; i++) {
// 			processedScores[i] = convolve(rawScores, narrowGaussian, i) - convolve(rawScores, wideGaussian, i);
// 		}
// 	}
	
// 	private void detectPeaks() {
// 		candidates = new ArrayList<Integer>();
		
// 		int threshold = 40;
// 		int leftLimit = 0;
// 		boolean inPeak = false;
		
// 		for (int i = 0; i < processedScores.length; i++) {
// 			if (processedScores[i] >= threshold & !inPeak) {
// 				leftLimit = i;
// 				inPeak = true;
// 			} else if (processedScores[i] < threshold && inPeak) {
// 				candidates.add((leftLimit + i) / 2);
// 				inPeak = false;
// 			}
// 		}
// 	}
	
// 	// TODO rename
// 	private double convolve(double[] array, double[] kernel, int index) {
// 		double sum = 0;
// 		for (int i = 0; i < kernel.length; i++) {
// 			int j = index - kernel.length / 2 + i;
// 			if (j >= 0 && j < array.length)
// 				sum += array[j]*kernel[i];
// 		}
// 		return sum;
// 	}
// }