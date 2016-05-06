package nbtool.data.log;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Vector;

import com.google.protobuf.Message;

import nbtool.data.SExpr;
import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.JsonObject;
import nbtool.data.session.Session;
import nbtool.images.YUYV8888Image;
import nbtool.util.Debug;
import nbtool.util.Debug.DebugSettings;
import nbtool.util.test.TestBase;
import nbtool.util.test.Tests;

public abstract class Log {
	
	protected LogReference logReference;
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
		
	public abstract void saveChangesToTempFile() throws Exception;
	public abstract void saveChangesToLoadFile() throws Exception;
		
	public abstract int version();
	
	protected abstract long getUniqueID();
	
	public abstract boolean addBlockFromProtobuf(Message message,
            String whereFrom, long imageIndex, long createdWhen);
	
	public abstract boolean addBlockFromImage(YUYV8888Image image,
			String whereFrom, long imageIndex, long createdWhen);
	
	public abstract boolean addBlockFromSexpr(SExpr sexpr,
			String whereFrom, long imageIndex, long createdWhen);

	public abstract boolean addBlockFromJson(JsonValue val,
			String whereFrom, long imageIndex, long createdWhen);

	public abstract boolean addBlockFromLog(Log log);
		
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
