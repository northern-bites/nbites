package nbtool.gui.logviews.sound.whistle;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import nbtool.data.log.Block;
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

	public FloatBuffer(Block fb, int chan, int frm) {
		this.channels = chan;
		this.frames = frm;
		data = new float[channels][frames];

		parse(fb);
	}

	@Override
	public void parse(Block fromBlock) {
		Debug.info("found %d expected %d\n", fromBlock.data.length, 4 * channels * frames);
		assert(fromBlock.data.length == 4 * channels * frames);
		ByteBuffer buf = ByteBuffer.wrap(fromBlock.data);
		buf.order(ByteOrder.LITTLE_ENDIAN);

		max = Float.NEGATIVE_INFINITY;
		for (int i = 0; i < channels; ++i) {
			for (int j = 0; j < frames; ++j) {
				float f = buf.getFloat();
				data[i][j] = f;
				assert(f >= 0);
				if (f > max) max = f;
			}
		}
	}

	@Override
	public Block toBlock() {
		// TODO Auto-generated method stub
		return null;
	}

//	@Override
//	public void parse(_Log soundLog) {
//		this.frames = soundLog.sexprForContentItem(0).firstValueOf("frames").valueAsInt();
//		this.channels = soundLog.sexprForContentItem(0).firstValueOf("channels").valueAsInt();
//		String format = soundLog.sexprForContentItem(0).firstValueOf("format").value();
//
//		assert(format.equalsIgnoreCase("float"));
//		assert(soundLog.bytes.length == 4 * channels * frames);
//


}
