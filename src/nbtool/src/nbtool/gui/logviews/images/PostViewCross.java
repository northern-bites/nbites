package nbtool.gui.logviews.images;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.io.CppIO;
import nbtool.io.CppIO.CppFuncCall;
import nbtool.io.CppIO.CppFuncListener;
import nbtool.util.U;

public class PostViewCross extends ViewParent implements CppFuncListener{
	BufferedImage img;
	double[] unfilteredHistogram;
	
	static final int scale = 2;
	
	public void paintComponent(Graphics g) {
		if (img != null) {
			// Display image
			g.drawImage(img, 0, 0, PostViewCross.scale*img.getWidth(), PostViewCross.scale*img.getHeight(), null);
		
			// Display unfiltered histogram
			g.setColor(Color.black);
			int barWidth = img.getWidth() / unfilteredHistogram.length;
			for(int i = 0; i < unfilteredHistogram.length; i++){
				int barHeight = (int)(unfilteredHistogram[i]);
				g.fillRect(PostViewCross.scale*(img.getWidth() + i*barWidth), 
						   PostViewCross.scale*(img.getHeight()-(int) unfilteredHistogram[i]),
            		       PostViewCross.scale*barWidth,
            		       PostViewCross.scale*barHeight);
			}
		}
    }
	
	@Override
	public void setLog(Log newlog) {
		log = newlog;
		
		int fi = CppIO.current.indexOfFunc("PostDetector");
		
		if (fi < 0) return;
		
		CppFuncCall fc = new CppFuncCall();
		
		fc.index = fi;
		fc.name = "PostDetector";
		fc.args = new ArrayList<Log>(Arrays.asList(log));
		fc.listener = this;
		
		CppIO.current.tryAddCall(fc);
	}
	
	public PostViewCross() {
		super();
	}

	@Override
	public void returned(int ret, Log... out) {
		this.img = U.biFromLog(out[0]);
		
		Log unfilteredHistogramLog = out[1];
		this.unfilteredHistogram = new double[unfilteredHistogramLog.bytes.length / 8];
		
		for (int i = 0; i < unfilteredHistogramLog.bytes.length / 8; i++) {
			byte[] doubleArray = new byte[8];
			for (int j = 0; j < 8; j++)
				doubleArray[j] = unfilteredHistogramLog.bytes[8*i+7-j];
			
			this.unfilteredHistogram[i] = ByteBuffer.wrap(doubleArray).getDouble();
			System.out.println(this.unfilteredHistogram[i]);
		}
		
		repaint();
	}
}
