// package nbtool.gui.logviews.images;

// import java.awt.Color;
// import java.awt.Graphics;
// import java.awt.image.BufferedImage;
// import java.nio.ByteBuffer;
// import java.nio.ByteOrder;
// import java.util.ArrayList;
// import java.util.Arrays;

// import nbtool.data.Log;
// import nbtool.gui.logviews.misc.ViewParent;
// import nbtool.io.CppIO;
// import nbtool.io.CppIO.CppFuncCall;
// import nbtool.io.CppIO.CppFuncListener;
// import nbtool.util.U;

// public class PostViewCross extends ViewParent implements CppFuncListener{
// 	BufferedImage img;
// 	double[] unfilteredHistogram;
// 	double[] filteredHistogram;
// 	ArrayList<Integer> candidates;
	
// 	static final int scale = 2;
	
// 	public void paintComponent(Graphics g) {
// 		super.paintComponent(g);
		
// 		if (img != null) {
// 			// Display image
// 			g.drawImage(img, 0, 0, PostViewCross.scale*img.getWidth(), PostViewCross.scale*img.getHeight(), null);
		
// 			// Display histograms
// 			drawHistogram(g, unfilteredHistogram, img.getWidth(), 0, PostViewCross.scale);
// 			drawHistogram(g, filteredHistogram, img.getWidth(), img.getHeight(), PostViewCross.scale);
// 			drawHistogram(g, filteredHistogram, 0, img.getHeight(), PostViewCross.scale);
			
// 			// Display candidate posts
// 	        g.setColor(Color.red);
// 	        for (int i = 0; i < candidates.size(); i++) {
// 	        	g.fillRect(PostViewCross.scale*candidates.get(i),
// 	     		   	       0,
// 	     		           2, 
// 	     		           2*PostViewCross.scale*img.getHeight());
// 	        }
// 		}
//     }
	
// 	private void drawHistogram(Graphics g, double[] histogram, int x, int y, int scale) {
// 		int barWidth = img.getWidth() / histogram.length;
// 		for(int i = 0; i < histogram.length; i++) {
// 			int barHeight = (int) histogram[i];
// 			g.fillRect(scale*(x + i*barWidth), 
// 					   scale*(y + (img.getHeight()-(int) histogram[i])),
//         		       scale*barWidth,
//         		       scale*barHeight);
// 		}
// 	}
	
// 	@Override
// 	public void setLog(Log newlog) {
// 		log = newlog;
		
// 		int fi = CppIO.current.indexOfFunc("PostDetector");
		
// 		if (fi < 0) return;
		
// 		CppFuncCall fc = new CppFuncCall();
		
// 		fc.index = fi;
// 		fc.name = "PostDetector";
// 		fc.args = new ArrayList<Log>(Arrays.asList(log));
// 		fc.listener = this;
		
// 		CppIO.current.tryAddCall(fc);
// 	}
	
// 	public PostViewCross() {
// 		super();
// 	}

// 	@Override
// 	public void returned(int ret, Log... out) {
// 		img = U.biFromLog(out[0]);
		
// 		unfilteredHistogram = new double[out[1].bytes.length / 8];
// 		parseHistogram(out[1], unfilteredHistogram);
		
// 		filteredHistogram = new double[out[2].bytes.length / 8];
// 		parseHistogram(out[2], filteredHistogram);
		
// 		candidates = new ArrayList<Integer>();
// 		byte[] bytes = out[3].bytes;
// 		ByteBuffer bb = ByteBuffer.wrap(bytes);
// 		bb.order(ByteOrder.LITTLE_ENDIAN);
// 		for (int i = 0; i < bytes.length / 4; i++) {
// 			candidates.add(bb.getInt());
// 		}
		
// 		repaint();
// 	}
	
// 	private void parseHistogram(Log log, double histogram[]) {
// 		// TODO use ByteBuffer and LITTLE_ENDIAN constant
// 		for (int i = 0; i < histogram.length; i++) {
// 			byte[] doubleArray = new byte[8];
// 			for (int j = 0; j < 8; j++)
// 				doubleArray[j] = log.bytes[8*i+7-j];
			
// 			histogram[i] = ByteBuffer.wrap(doubleArray).getDouble();
// 		}
// 	}
// }
