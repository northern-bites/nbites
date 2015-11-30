package nbtool.data.temop;

import java.io.File;

public class LogSource {
	
	public LogSource(From f, String d, File p) {
		this.from = f; this.details = d; this.file = p;
	}
	
	public enum From {
		FILE, NETWORK, GENERATED
	}
	
	public From from;
	//address or generation source.
	public String details;
	//file source of log if applicable..
	public File file;
	
	public String filename() {
		if (from == From.FILE && file != null)
			return file.getName();
		else return null;
	}
}
