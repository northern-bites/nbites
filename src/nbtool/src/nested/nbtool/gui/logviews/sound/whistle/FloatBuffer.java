package nbtool.gui.logviews.sound.whistle;


import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import nbtool.data._log._Log;
import nbtool.gui.logviews.sound2.Buffer;
import nbtool.util.Debug;

public class FloatBuffer extends Buffer<Float> {
	
	float[][] data;
	float max;
	
	public Float index(int channel, int frame) {
		if (channel < data.length && frame < data[0].length) {
			return data[channel][frame];
		} else {
			throw new RuntimeException(String.format("OOB: channel:%d frame:%d", channel, frame));
		}
	}

	@Override
	public Float get(int i, int c) {
		return data[c][i];
	}
	
	public Float max() {
		return max;
	}

	@Override
	public void parse(_Log soundLog) {
		this.frames = soundLog.sexprForContentItem(0).firstValueOf("frames").valueAsInt();
		this.channels = soundLog.sexprForContentItem(0).firstValueOf("channels").valueAsInt();
		String format = soundLog.sexprForContentItem(0).firstValueOf("format").value();
		
		assert(format.equalsIgnoreCase("float"));
		assert(soundLog.bytes.length == 4 * channels * frames);
		
		ByteBuffer buf = ByteBuffer.wrap(soundLog.bytes);
		buf.order(ByteOrder.LITTLE_ENDIAN);
		
		max = Float.NEGATIVE_INFINITY;
		data = new float[2][];
		for (int i = 0; i < channels; ++i) {
			data[i] = new float[frames];
			for (int j = 0; j < frames; ++j) {
				float f = buf.getFloat();
				data[i][j] = f;
				assert(f >= 0);
				if (f > max) max = f;
				
				//Logger.printf("channel %d v %f", i, f);
			}
		}
	}

	@Override
	public _Log toLog() {
		// TODO Auto-generated method stub
		return null;
	}

}
