package nbtool.data.log;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Vector;

import nbtool.data.json.JsonObject;
import nbtool.data.session.Session;

public abstract class Log {
	
	protected File logInFilesystem;
	public abstract boolean temporary();
	
	protected Vector<Block> blocks = null;
	public abstract Vector<Block> getBlocks();
	public abstract void setBlocks(Vector<Block> blocks);
	
	protected JsonObject topLevelDictionary = null;
	public abstract JsonObject getDictionary();
	public abstract void setDictionary(JsonObject obj);
	
	public long createdWhen = 0;
	public String logClass = "";
	
	public String host_type = null;
	public String host_name = null;
	public String host_addr = null;
	
	public Session container;
	
	//Used for ordering.  Comparing two Log objects can be done by comparing pointers, i.e. ==
	public final long jvm_unique_id = getUniqueID();
	
	public abstract byte[] serialize();
	
	public abstract void writeTo(OutputStream os);
	
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
	
	public static Log parseFromStream(InputStream is) {
		return LogInternal.parseFromStream(is);
	}

}
