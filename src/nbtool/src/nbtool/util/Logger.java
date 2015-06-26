package nbtool.util;

public class Logger {
	
	public static enum LogLevel {
		levelEVENT(0, "EVENT"),
		levelINFO(1, "INFO"),
		levelWARN(2, "WARN"),
		levelERROR(3, "ERROR");
		
		private int internal;
		private String name;
		private LogLevel(int i, String name) {
			this.name = name;
			internal = i;
		}
		
		public String toString() {
			return name;
		}
		
		public boolean shows(LogLevel l) {
			return (l.internal >= this.internal);
		}
	}
	
	public static LogLevel EVENT = LogLevel.levelEVENT;
	public static LogLevel INFO = LogLevel.levelINFO;
	public static LogLevel WARN = LogLevel.levelWARN;
	public static LogLevel ERROR = LogLevel.levelERROR;
	
	public static LogLevel level = EVENT;
	
	/*
	 * Don't synchronize output functions, we can assume the JVM / JDK does this for us.
	 * */

	public static void println(String m) {
		System.out.println(m);
	}
	
	public static void printf(String f, Object... args) {
		System.out.printf(f + "\n", args);
	}
	
	public static void log(LogLevel l, String m) {
		if (level.shows(l)) {
			println(String.format("[%-5s] %s", l, m));
		}
	}
	
	public static void logf(LogLevel l, String f, Object ... args) {
		if (level.shows(l)) {
			println(String.format("[%-5s] %s", l, String.format(f, args)));
		}
	}
	
	public static void warnf(String f, Object ... args) {
		logf(WARN, f, args);
	}
	
	public static void infof(String f, Object ... args) {
		logf(INFO, f, args);
	}
	
	public static void errorf(String f, Object ... args) {
		logf(ERROR, f, args);
	}
}
