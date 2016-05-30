package nbtool.util;

import java.util.Arrays;

public class Debug {
	
	public static void notRefactored() {
		global.error("\n\n*************** THIS HAS NOT BEEN REFACTORED YET ***************\n");
	}
	
	/* cannot code escape char in java string, this is a work-around.
	 * NON PORTABLE (UNIX only, only some terminals) */
	public static final String COLOR_RED = (char) 27 + "[31m";
	public static final String COLOR_GREEN = (char) 27 + "[32m";
	public static final String COLOR_YELLOW = (char) 27 + "[33m";
	public static final String COLOR_BLUE = (char) 27 + "[34m";
	public static final String COLOR_MAGENTA = (char) 27 + "[35m";
	public static final String COLOR_CYAN = (char) 27 + "[36m";
	
	public static final String COLOR_BOLD_YELLOW = (char) 27 + "[33;1m";
	public static final String COLOR_BOLD_RED = (char) 27 + "[31;1m";
	
	public static final String COLOR_RESET = (char) 27 + "[0m";
	
	/* this is so close to a hack the difference is negligible:
	 * ** update function with all edge cases immediately */
	private static boolean mayUseColor() {
		String term = System.getenv("TERM");
		if (term == null)
			return false;
		term = term.toLowerCase();
		return term.contains("xterm") || term.contains("color");
	}
	
	public static final boolean colorAllowed = mayUseColor();
	
	public static void set_red() {
		if (colorAllowed) System.out.print(COLOR_RED);
	}
	
	public static void set_green() {
		if (colorAllowed) System.out.print(COLOR_GREEN);
	}
	
	public static void set_yellow() {
		if (colorAllowed) System.out.print(COLOR_YELLOW);
	}
	
	public static void set_blue() {
		if (colorAllowed) System.out.print(COLOR_BLUE);
	}
	
	public static void set_magenta() {
		if (colorAllowed) System.out.print(COLOR_MAGENTA);
	}
	
	public static void set_cyan() {
		if (colorAllowed) System.out.print(COLOR_CYAN);
	}
	
	public static void set_bold_yellow() {
		if (colorAllowed) System.out.print(COLOR_BOLD_YELLOW);
	}
	
	public static void set_bold_red() {
		if (colorAllowed) System.out.print(COLOR_BOLD_RED);
	}
	
	public static void set_reset() {
		if (colorAllowed) System.out.print(COLOR_RESET);
	}
	
	public static enum LogLevel {
		levelEVENT(0, "EVENT", '-'),
		levelINFO(1, "INFO", 'I'),
		levelWARN(2, "WARN", 'W'),
		levelERROR(3, "ERROR", 'E'),
		levelALWAYS(4, "NONE", ' ');
		
		private int internal;
		private String name;
		public final char symbol;
		private LogLevel(int i, String name, char symbol) {
			this.name = name;
			this.internal = i;
			this.symbol = symbol;
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
	public static LogLevel ALWAYS = LogLevel.levelALWAYS;
	
	private static String[] LOG_COLORS = {
		"",
		COLOR_GREEN,
		COLOR_BOLD_YELLOW,
		COLOR_BOLD_RED,
		COLOR_CYAN
	};
		
	public static LogLevel level = EVENT;
	
	/*
	 * Don't synchronize output functions, we can assume the JVM does this for us.
	 * */
	
	private static final DebugSettings global =
			new DebugSettings(true, true, true, null, null);
	
	
	
	public static DebugSettings createSettings(boolean checks, boolean asserts,
			boolean colors, LogLevel lev, String dp ) {
		return new DebugSettings( checks,  asserts,
			 colors,  lev,  dp );
	}
	
	public static DebugSettings createSettings(LogLevel lev) {
		return createSettings( true,  true,
				 true,  lev,  null );
	}
	
	public static DebugSettings createSettings() {
		return createSettings(null);
	}
	
	public static class DebugSettings {
		public boolean useChecks;
		public boolean useAsserts;
		public boolean useColorOverride;
		
		public LogLevel slevel;
		
		public String debugPrefix;
		
		public DebugSettings(boolean checks, boolean asserts,
				boolean colors, LogLevel lev, String dp
				) {
			useChecks = checks;
			useAsserts = asserts;
			useColorOverride = colors;
			slevel = lev;
			
			debugPrefix = dp;
		}
		
		private LogLevel usedLevel() {
			return slevel == null ? level : slevel;
		}
		
		public String format(String location, LogLevel msgLev, String f, Object ... args) {
			if (msgLev == null)
				return null;
			
			LogLevel usedLev = usedLevel();
			
			if (!usedLev.shows(msgLev) )
				return null;
			
			String pre = "", post = "";
			if (useColorOverride && colorAllowed) {
				pre = LOG_COLORS[msgLev.internal];
				post = COLOR_RESET;
			}
			
			String printLoc = debugPrefix;
			if (printLoc == null) printLoc = location;
			assert(printLoc != null);
			
			String formatInside = String.format(f, args);
			String formatted = String.format("%s[%C][%s] %s\n%s",
					pre, msgLev.symbol,
					printLoc, formatInside, post);
			
			return formatted;
		}
		
		private void printIfNotNull(String s) {
			if (s != null) System.out.print(s);
		}
		
		public void print(String f, Object ... args) {
			internal(ALWAYS, f, args);
		}
		
		public void event(String f, Object ... args) {
			internal(EVENT, f, args);
		}
		
		public void info(String f, Object ... args) {
			internal(INFO, f, args);
		}
		
		public void warn(String f, Object ... args) {
			internal(WARN, f, args);
		}
		
		public void error(String f, Object ... args) {
			internal(ERROR, f, args);
		}
		
		protected String instanceLocation() {
			StackTraceElement rel = Utility.codeLocation(4);
			return formatLocation(rel);
		}
		
		protected String globalLocation() {
			StackTraceElement rel = Utility.codeLocation(5);
			return formatLocation(rel);
		}
		
		protected final String formatLocation(StackTraceElement rel) {
			String file = rel.getFileName();
			file = file.substring(0, file.lastIndexOf('.'));
			return String.format("%s:%d", file, rel.getLineNumber());
		}
		
		protected void internal(LogLevel lev, String format, Object ...args) {
			printIfNotNull(format(
					(this == global) ? globalLocation() : instanceLocation()
					, lev, format, args));
		}
	}
	
	//private static String outputFormat(LogLevel lev, )
	
	public static void plain(String f, Object... args) {
		System.out.printf(f + "\n", args);
	}
	
	public static void print(String f, Object... args) {
		global.print(f, args);
	}
	
	public static void event(String f, Object ... args) {
		global.event(f, args);
	}
	
	public static void warn(String f, Object ... args) {
		global.warn(f, args);
	}
	
	public static void info(String f, Object ... args) {
		global.info(f, args);
	}
	
	public static void error(String f, Object ... args) {
		global.error(f, args);
	}
	
	public static void lbreak() {
		System.out.println("");
	}
	
	public static void dbreak(String suffix) {
		char[] chars = new char[50];
		Arrays.fill(chars, '-');
		System.out.println(new String(chars) + suffix);
	}
}
