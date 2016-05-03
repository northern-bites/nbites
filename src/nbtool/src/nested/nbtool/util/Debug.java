package nbtool.util;

public class Debug {
	
	/* cannot code escape char in java string, this is a work-around.
	 * NON PORTABLE (UNIX only, only some terminals) */
	public static final String COLOR_RED = (char) 27 + "[31m";
	public static final String COLOR_GREEN = (char) 27 + "[32m";
	public static final String COLOR_YELLOW = (char) 27 + "[33m";
	public static final String COLOR_BLUE = (char) 27 + "[34m";
	public static final String COLOR_MAGENTA = (char) 27 + "[35m";
	public static final String COLOR_CYAN = (char) 27 + "[36m";
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
	
	public static enum LogLevel {
		levelEVENT(0, "EVENT"),
		levelINFO(1, "INFO"),
		levelWARN(2, "WARN"),
		levelERROR(3, "ERROR"),
		levelALWAYS(4, "    ");
		
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
	public static LogLevel ALWAYS = LogLevel.levelALWAYS;
	
	private static String[] LOG_COLORS = {
		"",
		COLOR_GREEN,
		COLOR_YELLOW,
		COLOR_RED,
		COLOR_CYAN
	};
		
	public static LogLevel level = EVENT;
	
	/*
	 * Don't synchronize output functions, we can assume the JVM does this for us.
	 * */
	
	private static final DebugSettings global =
			new DebugSettings(true, true, true, null, "");
	
	public static DebugSettings createSettings(boolean checks, boolean asserts,
				boolean colors, LogLevel lev, String dp ) {
		return new DebugSettings( checks,  asserts,
				 colors,  lev,  dp );
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
		
		public String format(LogLevel msgLev, String f, Object ... args) {
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
			
			String formatInside = String.format(f, args);
			String formatted = String.format("%s[%C][%s] %s\n%s",
					pre, msgLev.name.charAt(0),
					debugPrefix, formatInside, post);
			
			return formatted;
		}
		
		public void printf(String f, Object ... args) {
			System.out.print(format(ALWAYS, f, args));
		}
		
		public void event(String f, Object ... args) {
			System.out.print(format(ALWAYS, f, args));
		}
		
		public void info(String f, Object ... args) {
			System.out.print(format(ALWAYS, f, args));
		}
		
		public void warn(String f, Object ... args) {
			System.out.print(format(ALWAYS, f, args));
		}
		
		public void error(String f, Object ... args) {
			System.out.print(format(ALWAYS, f, args));
		}
	}
	
	//private static String outputFormat(LogLevel lev, )
	
	public static void plain(String f, Object... args) {
		System.out.printf(f + "\n", args);
	}
	
	public static void printf(String f, Object... args) {
		global.printf(f, args);
	}
	
	public static void log(LogLevel l, String m) {
		if (level.shows(l)) {
			System.out.print(global.format(l, m));
		}
	}
	
	public static void logf(LogLevel l, String f, Object ... args) {
		if (level.shows(l)) {
			System.out.print(global.format(l, f, args));
		}
	}
	
	public static void warnf(String f, Object ... args) {
		global.warn(f, args);
	}
	
	public static void warn(String f) {
		warnf(f);
	}
	
	public static void infof(String f, Object ... args) {
		global.info(f, args);
	}
	
	public static void info(String f) {
		info(f);
	}
	
	public static void errorf(String f, Object ... args) {
		global.error(f, args);
	}
	
	public static void error(String f) {
		error(f);
	}
}
