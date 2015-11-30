package nbtool.data.temop;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Date;

import nbtool.data.Block;
import nbtool.data.Session;
import nbtool.data.json.JsonObject;

public final class _Log extends Log {

	public static Log parseFromStream(InputStream is) {
		// TODO Auto-generated method stub
		return null;
	}

	public static Log parseFromBytes(byte[] bytes) {
		return parseFromStream(new ByteArrayInputStream(bytes));
	}
	
	private JsonObject attributes;
	private Block[] blocks;
	
	private Session parent;
	private LogSource source;
	
	@Override
	public JsonObject attributes() {
		return attributes;
	}

	@Override
	public Block[] blocks() {
		return blocks;
	}

	@Override
	public LogSource source() {
		return source;
	}

	@Override
	public Session parent() {
		return parent;
	}

	@Override
	public void setSource(LogSource s) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void setParent(Session s) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public String generatedFilename() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String displayName() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void serializeToStream(OutputStream os) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public byte[] serializeToByteArray() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String madeWhere() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Long madeRobotTime() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Date madeDate() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Long checksum() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Integer version() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String primaryType() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String primaryFrom() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Integer primaryImgIndex() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Boolean primaryIsProtobuf() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Integer primaryWidth() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Integer primaryHeight() {
		// TODO Auto-generated method stub
		return null;
	}
	
}
