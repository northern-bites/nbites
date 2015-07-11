package nbtool.gui.logviews.sound2;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import nbtool.data.Log;
import nbtool.data.SExpr;

public class ShortBuffer extends Buffer<Short> {
	
	short[] peaks;
	short[][] data;
	
	public ShortBuffer() {	}
	public ShortBuffer(short[][] d) {
		this.channels = d.length;
		this.frames = d[0].length;
		this.data = d;
	}

	@Override
	public Short get(int c, int i) {
		return data[c][i];
	}
	
	public Short left(int i) {
		return data[0][i];
	}
	
	public Short right(int i) {
		return data[1][i];
	}
	
	@Override
	public void parse(Log soundLog) {
		SExpr c1 = soundLog.tree().firstValueOf(Log.LOG_CONTENTS_S);
		channels = c1.firstValueOf("channels").valueAsInt();
		frames = c1.firstValueOf("frames").valueAsInt();
		
		data = new short[channels][frames];
		peaks = new short[channels];
		assert(soundLog.bytes.length >= (Short.BYTES * channels * frames));
		
		ByteBuffer buffer = ByteBuffer.wrap(soundLog.bytes);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		
		for (int f = 0; f < frames; ++f) {
			for (int c = 0; c < channels; ++c) {
				data[c][f] = buffer.getShort();
				if ( Math.abs(data[c][f]) > peaks[c] )
					peaks[c] = data[c][f];
			}
		}
	}

	@Override
	public Log toLog() {
		byte[] internal = new byte[Short.BYTES * channels * frames];
		ByteBuffer buffer = ByteBuffer.wrap(internal);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		
		for (int f = 0; f < frames; ++f) {
			for (int c = 0; c < channels; ++c) {
				buffer.putShort(data[c][f]);
			}
		}
		
		return Log.logWithTypePlus("sound", buffer.array(),
				SExpr.pair("channels", channels),
				SExpr.pair("frames", frames));
	}

}
