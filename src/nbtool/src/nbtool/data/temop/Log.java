package nbtool.data.temop;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.Date;

import nbtool.data.Block;
import nbtool.data.Session;
import nbtool.data.json.JsonNumber;
import nbtool.data.json.JsonObject;

public abstract class Log {
	
	//general map of log attributes.
	public abstract JsonObject attributes();
	
	//individual items contained in log (and their attributes – see Block)
	public abstract Block[] blocks();
	
	//information about where this log came from (see LogSource)
	public abstract LogSource source();
	public abstract void setSource(LogSource s);
	
	//Session containing this log
	public abstract Session parent();
	public abstract void setParent(Session s);
	
	//unique for a given process, i.e. nbtool instance.
	public final long processLogID = LogConstants.getJvmLogID();
	
	public abstract String generatedFilename();
	public abstract String displayName();
	
	/*
	 * IO operations
	 * */
	
	public static Log parseFromStream(InputStream is) {
		return _Log.parseFromStream(is);
	}
	
	public static Log parseFromBytes(byte[] bytes) {
		return _Log.parseFromBytes(bytes);
	}
	
	public abstract void serializeToStream(OutputStream os);
	
	public abstract byte[] serializeToByteArray();
	
	/*
	 * methods to retrieve specific attributes of Log
	 * 
	 * all return null if attribute not found
	 * 
	 * methods prefixed with 'primary' look for the key in
	 * 	blocks()[0].attributes
	 * */
	
	/*
	 //need to use LogConstants Key
	public Integer primaryWidth() {
		if (blocks().length > 0 &&
				blocks()[0].attributes.containsKey("width")) {
			return blocks()[0].attributes.
				get("width").<JsonNumber>cast().intValue();
		} else return null;
	} */
	
	public abstract String madeWhere();
	
	public abstract Long madeRobotTime();
	
	public abstract Date madeDate();
	
	public abstract Long checksum();
	
	public abstract Integer version();
	
	public abstract String primaryType();
	
	public abstract String primaryFrom();
	
	public abstract Integer primaryImgIndex();
	
	public abstract Boolean primaryIsProtobuf();
	
	//image attributes
	public abstract Integer primaryWidth();
	public abstract Integer primaryHeight();
}
