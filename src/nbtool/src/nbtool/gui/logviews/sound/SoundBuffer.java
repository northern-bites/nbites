package nbtool.gui.logviews.sound;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.util.Logger;

public class SoundBuffer {
	
	public short[] left;
	public short[] right;
	
	public int nframes;
	public int max;
	
	public SoundBuffer(Log from) {		
		SExpr c1 = from.tree().find(Log.LOG_CONTENTS_S).get(1);
		int channels = c1.firstValueOf("channels").valueAsInt();
		int frames = c1.firstValueOf("frames").valueAsInt();
		assert(channels == 2);
		assert(frames == 1024);
		this.nframes = frames;

		Logger.printf("%s bytes wanted %d", from.bytes.length, nframes * 2 * Short.BYTES);
		assert(from.bytes.length == nframes * 2 * Short.BYTES);
		
		ByteBuffer bb = ByteBuffer.allocate(nframes * 2 * Short.BYTES);
		bb.order(ByteOrder.LITTLE_ENDIAN);
		bb.put(from.bytes);
		bb.order(ByteOrder.BIG_ENDIAN);
		bb.rewind();
		
		max = 0;
		left = new short[nframes];
		right = new short[nframes];
		for (int i = 0; i < 1024; ++i) {
			left[i] = bb.getShort();
			right[i] = bb.getShort();
			if (left[i] > max) max = left[i];
			if (right[i] > max) max = right[i];
		}
		
		Logger.printf("Max is %d", max);
	}
	
	public double[] left() {
		return convertToDouble(left, Short.MAX_VALUE);
	}

	public double[] right() {
		return convertToDouble(right, Short.MAX_VALUE);
	}
	
	public double[] left(short d) {
		return convertToDouble(left, d);
	}
	
	public double[] right(short d) {
		return convertToDouble(right, d);
	}
	
	private static double[] convertToDouble(short[] array, short denom) {
		double[] ret = new double[array.length];
		for (int i = 0; i < ret.length; ++i) {
			ret[i] = array[i] / ((double) denom);
		}
		
		return ret;
	}
	
	public static Short[] objectify(short[] vals) {
		Short[] io = new Short[vals.length];
		for (int i = 0; i < vals.length; ++i)
			io[i] = vals[i];
		return io;
	}
	
	public static Double[] objectify(double[] vals) {
		Double[] io = new Double[vals.length];
		for (int i = 0; i < vals.length; ++i)
			io[i] = vals[i];
		return io;
	}
}
