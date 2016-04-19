package nbtool.data.log;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.Vector;

import nbtool.data.json.JsonObject;

public abstract class Log {
	
	public Vector<Block> blocks = null;
	public JsonObject topLevelDictionary = null;
	
	public long createdWhen = 0;
	public String logClass = "";
	
	public abstract byte[] serialize();
	
	public abstract void writeTo(OutputStream os);
	
	public static Log emptyLog() {
		
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
