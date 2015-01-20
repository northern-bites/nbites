package nbclient.util;

import java.awt.datatransfer.DataFlavor;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class NBConstants {
	
	public static final String DEFAULT_S = "default";
	public static final String IMAGE_S = "YUVImage";
	public static final String PROTOBUF_S = "protobuf";
	public static final String TEST_S = "test";
	public static final String STATS_S = "stats";
	
	public static final int VERSION = 4;
	
	public static final int SERVER_PORT = 30000;
	public static final int CNC_PORT = 30001;
	public static final int CPP_PORT = 30002;
	
	//Who names a class DataFlavor?  That's just so... Idk. Fllaavvvooorr.  Data Fllaaavoor. MMM, gimme some'o that DataFlav
	//public static DataFlavor treeFlavor = new DataFlavor(Log.class, "NB-OpaqueLog");
	
	public static enum MODE {
		NETWORK_SAVING(0), NETWORK_NOSAVE(1), FILESYSTEM(2), NONE(3);
		
		public final int index;
		private MODE(int i) {
			this.index = i;
		}
	}
	
	public static final String[] mode_strings = {
		"from net to fs", "from net", "from fs", "none"
	};
	
	public static enum STATUS {
		IDLE(0),
		STARTING(1),
		RUNNING(2),
		STOPPING(3);
		
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
