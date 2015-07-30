package nbtool.util;

import java.awt.Rectangle;
import java.awt.datatransfer.DataFlavor;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import nbtool.data.Log;
import nbtool.gui.logviews.fieldLocation.FieldView;
import nbtool.gui.logviews.images.EdgeView;
import nbtool.gui.logviews.images.LineView;
import nbtool.gui.logviews.images.DebugImageView;
import nbtool.gui.logviews.images.BasicLineView;
import nbtool.gui.logviews.images.ImageView;
import nbtool.gui.logviews.images.ZoomImageView;
import nbtool.gui.logviews.images.BallView;
import nbtool.gui.logviews.images.FrontEndView;
import nbtool.gui.logviews.misc.BotStatsView;
import nbtool.gui.logviews.misc.CrossBright;
import nbtool.gui.logviews.misc.DefaultView;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.gui.logviews.proto.ProtoBufView;
import nbtool.gui.logviews.proto.ProtoBallView;
import nbtool.gui.logviews.sound2.SndFreqView;
import nbtool.gui.logviews.sound2.SndAmpView;
import nbtool.gui.logviews.loc.LocSwarmView;


public class NBConstants {
	
	public static final int VERSION = 7;
	public static final int MINOR_VERSION = 0;
	
	public static final int NBITES_TEAM_NUM = 14;
	
	public static final String PROTOBUF_TYPE_PREFIX = "proto-";
	
	public static final Rectangle DEFAULT_BOUNDS = new Rectangle(0,0,900,600);
		
	public static final String USER_PREFERENCES = "~/.nbtool-preferences.txt";
	public static final String NBITES_DIR = System.getenv("NBITES_DIR");
	
	public static final String DEFAULT_S = "_DEFAULT_";
	public static final String PROTOBUF_S = "_PROTOBUF_";

	public static final String IMAGE_S = "YUVImage";
	public static final String STATS_S = "stats";
	
	public static final int STREAM_PORT = 30000;
	public static final int CONTROL_PORT = 30001;
	public static final int NBCROSS_PORT = 30002;
	
	public static final int SOCKET_TIMEOUT = 5000; 		//Milliseconds
	
	public static final int NBCROSS_CALL_TIMEOUT = 0;	//interpreted as infinite.  Note that, since these calls are local,
														//we worry less about dead sockets.
		
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
	
	public static final String[] STATUS_STRINGS = {
		"idle", "starting", "running", "stopping"
	};
	
	public static final Map<String, Class<? extends ViewParent>[]> POSSIBLE_VIEWS = setupPossible();
	@SuppressWarnings("unchecked")
	private static Map<String, Class<? extends ViewParent>[]> setupPossible() {
		Map<String, Class<? extends ViewParent>[]> map = new HashMap<String, Class<? extends ViewParent>[]>();
		
		map.put("YUVImage", new Class[]{ImageView.class, ZoomImageView.class, FrontEndView.class, LineView.class, BallView.class, DebugImageView.class});
		map.put("STATS", new Class[]{BotStatsView.class});
		map.put("prot-RobotLocation", new Class[]{FieldView.class});
		map.put(DEFAULT_S, new Class[]{DefaultView.class});
		map.put(PROTOBUF_S, new Class[]{ProtoBufView.class});
		map.put("MULTIBALL", new Class[]{ProtoBallView.class});
		map.put("proto-RobotLocation", new Class[]{FieldView.class});
		map.put("location", new Class[]{LocSwarmView.class});
		map.put("proto-FieldLines", new Class[]{BasicLineView.class});
		map.put("sound", new Class[]{SndFreqView.class, SndAmpView.class});
		map.put("proto-Corners", new Class[]{BasicLineView.class});
		
		map.put(DEFAULT_S, new Class[]{DefaultView.class});
		map.put(PROTOBUF_S, new Class[]{ProtoBufView.class});

		return map;
	}
}
