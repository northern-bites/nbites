package nbtool.gui.logviews.sound;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.util.Logger;

public class SoundBuffer {
	
	public int[] left;
	public int[] right;
	
	public int nframes;
	public int max;
	
	public SoundBuffer(Log from) {		
		SExpr c1 = from.tree().find(Log.LOG_CONTENTS_S).get(1);
		int channels = c1.firstValueOf("channels").valueAsInt();
		int frames = c1.firstValueOf("frames").valueAsInt();
		assert(channels == 2);
		assert(frames == 1024);
		this.nframes = frames;

		Logger.printf("%s bytes wanted %d", from.bytes.length, nframes * 2 * Integer.BYTES);
		assert(from.bytes.length == nframes * 2 * Integer.BYTES);
		
		
		ByteBuffer bb = ByteBuffer.allocate(nframes * 2 * Integer.BYTES);
		bb.order(ByteOrder.LITTLE_ENDIAN);
		bb.put(from.bytes);
		bb.order(ByteOrder.BIG_ENDIAN);
		bb.rewind();
		
		max = 0;
		left = new int[nframes];
		right = new int[nframes];
		for (int i = 0; i < 1024; ++i) {
			left[i] = bb.getInt();
			right[i] = bb.getInt();
			if (left[i] > max) max = left[i];
			if (right[i] > max) max = right[i];
		}
		
		Logger.printf("Max is %d", max);
	}
	
	public double[] left() {
		return convertToDouble(left, Integer.MAX_VALUE);
	}

	public double[] right() {
		return convertToDouble(right, Integer.MAX_VALUE);
	}
	
	public double[] left(int d) {
		return convertToDouble(left, d);
	}
	
	public double[] right(int d) {
		return convertToDouble(right, d);
	}
	
	private static double[] convertToDouble(int[] array, int denom) {
		double[] ret = new double[array.length];
		for (int i = 0; i < ret.length; ++i) {
			ret[i] = array[i] / ((double) denom);
		}
		
		return ret;
	}
}
