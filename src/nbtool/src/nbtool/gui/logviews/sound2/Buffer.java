package nbtool.gui.logviews.sound2;

import nbtool.data.log._Log;

public abstract class Buffer<T extends Number> {
	public int frames;
	public int channels;
	
	public abstract T get(int i, int c);
	
	public abstract void parse(_Log soundLog);
	public abstract _Log toLog();	
}
