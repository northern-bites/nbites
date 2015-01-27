package nbtool.util;

import java.awt.Rectangle;
import java.awt.datatransfer.DataFlavor;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import nbtool.data.Log;

public class NBConstants {
	
	public static final String PROTOBUF_TYPE_PREFIX = "proto-";
	
	public static final Rectangle DEFAULT_BOUNDS = new Rectangle(0,0,900,600);
	public static final long DEFAULT_MAX_MEMORY_USAGE = 300000l;
	
	public static final String USER_CLASS_EXCEPTIONS = "~/.nbtool-exceptions.properties";
	public static final String USER_LOG_TO_VIEW_MAP = "~/.nbtool-views.properties";
	
	public static final String DEFAULT_S = "PT_default";
	public static final String PROTOBUF_S = "PT_protobuf";

	public static final String IMAGE_S = "YUVImage";
	public static final String STATS_S = "stats";
	
	public static final int VERSION = 4;
	
	public static final int SERVER_PORT = 30000;
	public static final int CNC_PORT = 30001;
	public static final int CPP_PORT = 30002;
	
	public static final int SOCKET_TIMEOUT = 5000; //Milliseconds
	
	//Who names a class DataFlavor?  That's just so... Idk. Fllaavvvooorr.  Data Fllaaavoor. MMM, gimme some'o that DataFlav
	public static DataFlavor treeFlavor = new DataFlavor(Log.class, "NB-OpaqueLog");
	
	public static enum MODE {
		NETWORK_SAVING(0), NETWORK_NOSAVE(1), FILESYSTEM(2), NONE(3);
		
		public final int index;
		private MODE(final int i) {
			this.index = i;
		}
	}
	
	public static final String[] mode_strings = {
		"from net to fs", "from net", "from fs", "none"
	};
	
	public static enum STATUS {
		IDLE(0),		//No io or IO threads active.  No active SessionHandler.  Ready to start.
		STARTING(1),	//Setting up IO or threads in a SessionHandler (usually very short)
		RUNNING(2),		//IO or threads active, in a SessionHandler
		STOPPING(3);	//IO or threads still active but shutting down.  When the SessionHandler determines 
						//all activity has stopped, it will notify idle.
		
		public final int index;
		private STATUS(int i) {
			this.index = i;
		}
	}
	
	public static final String[] status_strings = {
		"idle", "starting", "running", "stopping"
	};
	
	public static final ArrayList<FlagPair> flags = _setupFlags();
	private static ArrayList<FlagPair> _setupFlags() {
		ArrayList<FlagPair> ret = new ArrayList<FlagPair>();
		ret.add(new FlagPair("fileio      ", 2));
		ret.add(new FlagPair("servio      ", 3));
		
		ret.add(new FlagPair("STATS       ", 4));
		
		ret.add(new FlagPair("SENSORS     ", 5));
		ret.add(new FlagPair("GUARDIAN    ", 6));
		ret.add(new FlagPair("COMM        ", 7));
		ret.add(new FlagPair("LOCATION    ", 8));
		ret.add(new FlagPair("ODOMETRY    ", 9));
		ret.add(new FlagPair("OBSERVATIONS", 10));
		ret.add(new FlagPair("LOCALIZATION", 11));
		ret.add(new FlagPair("BALLTRACK   ", 12));
		ret.add(new FlagPair("IMAGES      ", 13));
		ret.add(new FlagPair("VISION      ", 14));
		return ret;
	}
	
	public static class FlagPair {
		public String name;
		public int index;
		protected FlagPair(String s, int i) {
			name = s; index = i;
		}
	}
}
