package nbtool.gui.logviews.sound2;

import nbtool.data.Log;

public abstract class Buffer<T extends Number> {
	public int frames;
	public int channels;
	
	public abstract T get(int i, int c);
	
	public abstract void parse(Log soundLog);
	public abstract Log toLog();	
}
