package nbtool.data.log;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Vector;

import nbtool.data.json.JsonObject;
import nbtool.data.session.Session;

public abstract class Log {
	
	protected LogReference logFile;
	public abstract boolean temporary();
	
	public Vector<Block> blocks = null;
	public JsonObject topLevelDictionary = null;
	
	public abstract String getFullDescription();
	
	public long createdWhen = 0;
	public String logClass = "";
	
	public String host_type = null;
	public String host_name = null;
	public String host_addr = null;
		
	//Used for ordering.  Comparing two Log objects can be done by comparing pointers, i.e. ==
	public final long jvm_unique_id = getUniqueID();
	
	public abstract byte[] serialize();
	
	public abstract void writeTo(OutputStream os) throws IOException;
	
	public abstract void saveChangesToTempFile();
	public abstract void saveChangesToLoadFile();
		
	public abstract int version();
	
	protected abstract long getUniqueID();
		
	public static Log emptyLog() {
		return LogInternal.emptyLog();
	}
	
	public static Log explicitLog(Vector<Block> blocks, JsonObject topLevel, String logClass,
			long created) {
		return LogInternal.explicitLog(blocks, topLevel, logClass, created);
	}
	
	public static Log parseFrom(byte[] bytes) {
		return LogInternal.parseFrom(bytes);
	}
	
	public static Log parseFromParts(byte[] json, byte[] data) {
		return LogInternal.parseFromParts(json, data);
	}
	
	public static Log parseFromStream(InputStream is) throws IOException {
		return LogInternal.parseFromStream(is);
	}

}
