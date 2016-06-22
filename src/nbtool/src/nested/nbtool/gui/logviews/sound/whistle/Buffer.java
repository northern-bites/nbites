package nbtool.gui.logviews.sound.whistle;

import nbtool.data.log.Block;

public abstract class Buffer<T extends Number> {
	public int frames;
	public int channels;

	public abstract T get(int i, int c);

	/* assume little endian */
	public abstract void parse(Block fromBlock);
	/* little endian */
	public abstract Block toBlock();
}
