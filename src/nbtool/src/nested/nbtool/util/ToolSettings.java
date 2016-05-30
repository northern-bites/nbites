package nbtool.util;

import java.awt.Rectangle;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;

import nbtool.gui.logviews.fieldLocation.FieldView;

import nbtool.gui.logviews.misc.DefaultView;
import nbtool.gui.logviews.misc.ViewParent;

public class ToolSettings {
	
	public static final String staticAddTestsMethodName = "_NBL_ADD_TESTS_";
	public static final String staticRequiredStartMethodName = "_NBL_REQUIRED_START_";
	
	public static final int VERSION = 8;
	public static final int MINOR_VERSION = 0;
	
	public static final int NBITES_TEAM_NUM = 14;
		
	public static final String PROTOBUF_TYPE_PREFIX = "proto-";
	
	public static final Rectangle DEFAULT_BOUNDS = new Rectangle(0,0,900,600);
		
	public static final String USER_PREFERENCES = "~/.nbtool-preferences.txt";
	public static final Path USER_PREFERENCES_PATH = 
			Paths.get(Utility.localizePath(USER_PREFERENCES));
	public static final String NBITES_DIR = System.getenv("NBITES_DIR") + "/";
	public static final Path NBITES_DIR_PATH = Paths.get(NBITES_DIR);
	
	public static final String DEFAULT_S = "_DEFAULT_";
	public static final String PROTOBUF_S = "_PROTOBUF_";
    //public static final String IMAGE_S = "_IMAGE_"

	public static final String IMAGE_S = "YUVImage";
	
	public static final int STREAM_PORT = 30000;
	public static final int CONTROL_PORT = 30001;
	public static final int NBCROSS_PORT = 30002;
	
	public static final int SOCKET_TIMEOUT = 5000; 		//Milliseconds
	
	public static final int NBCROSS_CALL_TIMEOUT = 0;	//interpreted as infinite.  Note that, since these calls are local,
														//we worry less about dead sockets.
	
//		map.put("YUVImage", new Class[]{ImageView.class, ZoomImageView.class,
//				FrontEndView.class, LineView.class, BallView.class,
//				DebugImageView.class});
//		map.put("STATS", new Class[]{BotStatsView.class});
//		map.put("proto-RobotLocation", new Class[]{FieldView.class});
//		map.put("MULTIBALL", new Class[]{ProtoBallView.class});
//		map.put("location", new Class[]{LocSwarmView.class});
//		map.put("proto-FieldLines", new Class[]{BasicLineView.class});
//		map.put("proto-Corners", new Class[]{BasicLineView.class});
//		
//		map.put("sound", new Class[]{SndFreqView.class, SndAmpView.class,
//				CorrelationView.class, DisplayView.class});
//		map.put("frequency", new Class[]{FFTWView.class, FrequencyStream.class});
				
//		map.put(PROTOBUF_S, new Class[]{ProtoBufView.class});

}
