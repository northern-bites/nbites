package nbtool.data.log;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.Vector;

import nbtool.data.json.JsonObject;
import nbtool.util.SharedConstants;
import nbtool.util.ToolSettings;

public class LogInternal extends Log {

	@Override
	public byte[] serialize() {
		
		return null;
	}

	@Override
	public void writeTo(OutputStream os) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public int version() {
		// TODO Auto-generated method stub
		return 0;
	}
	
	private static long static_id = 0;
	private static final Object static_id_lock = new Object();
	private static long getID() {
		long ret;
		synchronized(static_id_lock) {
			ret = static_id++;
		}
		
		return ret;
	}

	@Override
	protected long getUniqueID() {
		return getID();
	}
	
	private LogInternal() {}
	
	public static Log emptyLog() {
		LogInternal ret = new LogInternal();
		ret.createdWhen = System.currentTimeMillis();
		ret.blocks = new Vector<>();
		ret.topLevelDictionary = new JsonObject();
		ret.logClass = SharedConstants.LogClass_Null();
		
		ret.host_type = String.format("nbtool-v%d",
				ToolSettings.VERSION);
		ret.host_name = "computer-" + System.getenv("user.name");
		
		return ret;
	}
	
	public static Log explicitLog(Vector<Block> blocks, JsonObject topLevel, String logClass,
			long created) {
		
	}
	
	public static Log parseFrom(byte[] bytes) {
		
	}
	
	public static Log parseFromParts(byte[] json, byte[] data) {
		
	}
	
	public static Log parseFromStream(InputStream is) {
		
	}

}
