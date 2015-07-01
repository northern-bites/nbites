package nbtool.gui.logviews.sound;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.Logger;

public class SoundView extends ViewParent {

	private int channels = 2;
	private int frames = 1024;
	
	private Integer[] left = new Integer[1024];
	private Integer[] right = new Integer[1024];
	
	@Override
	public void setLog(Log newlog) {
		ByteBuffer bb = ByteBuffer.allocate(1024 * 2 * Integer.BYTES);
		bb.order(ByteOrder.LITTLE_ENDIAN);
		bb.put(newlog.bytes);
		bb.order(ByteOrder.BIG_ENDIAN);
		bb.rewind();
		
		int max = 0;
		for (int i = 0; i < 1024; ++i) {
			left[i] = bb.getInt();
			right[i] = bb.getInt();
			if (left[i] > max) max = left[i];
			if (right[i] > max) max = right[i];
		}
		
		Logger.printf("Max is %d", max);
		
		this.repaint();
		
		SoundPane<Integer> sp = new SoundPane<>(left, right, 2, new SoundPane.Scaler<Integer>(){
			public int pixelsFor(Integer val, int pixels) {
				return 0;
			}
		});
	}
	
	public static enum Format {
		NBS_S32_LE,
		NBS_S16_LE
	}
	
	
	private final int y_range = 200;
	private final int left_s = 50;
	private final int rght_s = 300;
	@Override
	public void paintComponent(Graphics g) {
		/*for (int i = 0; i < 1024; ++i) {
			g.drawLine(i, 50, i, 50 + i);
		}*/
		super.paintComponent(g);

		g.setColor(Color.RED);
		
		for (int i = 0; i < 1024; ++i) {
			int scaled = (int) scaleInt(left[i], y_range);
			g.drawLine(i, left_s, i, left_s + scaled);
		}
		
		g.setColor(Color.GREEN);
		
		for (int i = 0; i < 1024; ++i) {
			int scaled = (int) scaleInt(right[i], y_range);
			g.drawLine(i, rght_s, i, rght_s + scaled);
		}
		
		g.setColor(Color.BLACK);
		g.drawLine(0, left_s, 1024, left_s);
		g.drawLine(0, rght_s, 1024, rght_s);
		
		//------------------------
		
//		for (int i = 0; i < 1024; ++i) {
//			int orig = left[i];
//			int scaled = scaleInt(orig, 100000);
//			Logger.printf("%d from %d", scaled, orig);
//			g.drawLine(i, 150, i, 150 + scaled );
//		}
//		g.setColor(Color.RED);
//		g.drawLine(0, 150, 0, 150 + scaleInt(Integer.MAX_VALUE, 100));
	}
	
	@Override
	public Dimension getPreferredSize() {
		return new Dimension(1050, 300);
	}
	
	@Override
	public Dimension getMinimumSize() {
		return new Dimension(1050, 300);
	}
	
	private static final int EGV = 20000000;
	private static long scaleInt(int ts, int range) {
		//double perc = (double) ts / Integer.MAX_VALUE;
		return  ( (long) ts * range) / (EGV);
	}
}