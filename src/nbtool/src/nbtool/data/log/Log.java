package nbtool.data.log;

public class Log {
	
	public static final int[] CIRCULATED_VERSIONS = {6,7,8};
	
	public int version() { return 8; }
	
	public boolean isJsonLog() { return version() >= 8; }
	public boolean isLispLog() { return version() <= 7; }
	
	

}
