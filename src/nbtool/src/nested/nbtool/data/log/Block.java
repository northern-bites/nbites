package nbtool.data.log;

import com.google.protobuf.Message;

import nbtool.data.SExpr;
import nbtool.data.json.Json;
import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.JsonObject;
import nbtool.data.json.JsonParser.JsonParseException;
import nbtool.images.YUYV8888Image;
import nbtool.util.SharedConstants;
import nbtool.util.Utility;

public class Block {
	public byte[] data = null;
	public JsonObject dict = null;
	
	public String type = "";
	public String whereFrom = "";
	public long imageIndex = 0;
	public long createdWhen = 0;
	
	public Block(byte[] data, JsonObject dict,
			String type, String where, long index, long when ) {
		this.data = data;
		this.dict = dict;
		this.type = type;
		this.whereFrom = where;
		this.imageIndex = index;
		this.createdWhen = when;
	}
	
	public Block() { }
	
	public Log parseAsLog() {
		parseTypeCheck(SharedConstants.LogType_DEFAULT());
		return Log.parseFrom(data);
	}
	
	public JsonValue parseAsJson() throws JsonParseException {
		parseTypeCheck(SharedConstants.JsonType_DEFAULT());
		return Json.parse(new String(data));
	}
	
	public SExpr parseAsSExpr() {
		parseTypeCheck(SharedConstants.SexprType_DEFAULT());
		return SExpr.deserializeFrom(new String(data));
	}
	
	public YUYV8888Image parseAsYUVImage() {
		parseTypeCheck(SharedConstants.YUVImageType_DEFAULT());
		
		int width = dict.get(SharedConstants.LOG_BLOCK_IMAGE_WIDTH_PIXELS()).
				asNumber().asInt();
		int height = dict.get(SharedConstants.LOG_BLOCK_IMAGE_HEIGHT_PIXELS()).
				asNumber().asInt();
		
		return new YUYV8888Image(width / 2, height, this.data);
	}
	
	@SuppressWarnings("unchecked")
	static <T extends Message> T parseAsProtobuf(Block block) {
		@SuppressWarnings("rawtypes")
		Class cls = Utility.protobufClassFromType(block.type);
		
		if (cls == null) {
			throw new BlockParseException(block.type, "protobuf<?>");
		}
		
		T parsed = Utility.<T>protobufInstanceForClassWithData(cls,
				block.data);
		
		if (parsed == null) {
			throw new BlockParseException(block.type, "protobuf<?>");
		}
		
		return parsed;
	}

	private void parseTypeCheck(String rType) {
		if (!rType.equals(type)) {
			throw new BlockParseException(type, rType);
		}
	}
	
	@SuppressWarnings("serial")
	public static class BlockParseException extends RuntimeException {
		public BlockParseException(String type, String rtype) {
			super(String.format("Block of type '%s' cannot be parsed as '%s'", 
					type, rtype));
		}
	}
}
