package nbtool.data.log;

import java.util.Arrays;

import com.google.protobuf.Message;

import nbtool.data.SExpr;
import nbtool.data.json.Json;
import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.JsonObject;
import nbtool.data.json.JsonParser.JsonParseException;
import nbtool.images.Y16Image;
import nbtool.images.Y8Image;
import nbtool.images.YUYV8888Image;
import nbtool.util.SharedConstants;
import nbtool.util.Utility;
import nbtool.util.Utility.Pair;

public class Block {
	public byte[] data = null;
	public JsonObject dict = null;

	public String type = "";
	public String whereFrom = "";
	public long imageIndex = 0;
	public long createdWhen = 0;

	public static Block explicit(byte[] data, JsonObject dict,
			String type, String where, long index, long when) {
		return new Block(data,dict,
				 type,  where, index, when );
	}

	public Block(byte[] data, JsonObject dict,
			String type, String where, long index, long when ) {
		this.data = data == null ? new byte[0] : data;
		this.dict = dict == null ? new JsonObject() : dict;
		this.type = type == null ? "" : type;
		this.whereFrom = type == null ? "" : where;
		this.imageIndex = index;
		this.createdWhen = when;
	}

	public static Block explicit(byte[] data,
			String type) {
		return new Block(data, null,
				 type,  null, 0, 0 );
	}

	public Block(byte[] data,
			String type) {
		this(data, null, type, null, 0, 0);
	}

	public Block() { }

	public static Block empty() {
		return new Block();
	}

	public JsonObject getFullDictionary() {
		JsonObject obj = dict.copy().asObject();
		obj.put(SharedConstants.LOG_BLOCK_TYPE(), type);
		obj.put(SharedConstants.LOG_BLOCK_WHERE_FROM(), whereFrom);
		obj.put(SharedConstants.LOG_BLOCK_IMAGE_INDEX(), imageIndex);
		obj.put(SharedConstants.LOG_BLOCK_WHEN_MADE(), createdWhen);
		obj.put(SharedConstants.LOG_BLOCK_NUM_BYTES(), data == null ? 0 : data.length);

		return obj;
	}

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

	private Pair<Integer,Integer> imagePair() {
		int width = dict.get(SharedConstants.LOG_BLOCK_IMAGE_WIDTH_PIXELS()).
				asNumber().asInt();
		int height = dict.get(SharedConstants.LOG_BLOCK_IMAGE_HEIGHT_PIXELS()).
				asNumber().asInt();
		return new Pair<Integer,Integer>(width, height);
	}

	public YUYV8888Image parseAsYUVImage() {
		parseTypeCheck(SharedConstants.YUVImageType_DEFAULT());

		Pair<Integer,Integer> pair = imagePair();
//		return new YUYV8888Image(pair.a / 2, pair.b, this.data);
		return new YUYV8888Image(pair.a * 2, pair.b, this.data);
	}

	public Y16Image parseAsY16Image() {
		parseTypeCheck(SharedConstants.YUVImageType_Y16());

		Pair<Integer,Integer> pair = imagePair();
		return new Y16Image(pair.a, pair.b, this.data);
	}

	public Y8Image parseAsY8Image() {
//		parseTypeCheck(SharedConstants.YUVImageType_Y8(), "whiteRet", "greenRet", "orangeRet");
		parseTypeCheck(SharedConstants.YUVImageType_Y8());

		Pair<Integer,Integer> pair = imagePair();
		return new Y8Image(pair.a, pair.b, this.data);
	}



	public Message parseAsProtobufOfClass(Class<? extends Message> pclass) {
		if (pclass == null) {
			throw new BlockParseException(this.type, String.format("protobuf<null>") );
		}

		parseTypeCheck(pclass.getSimpleName());

		Message parsed = Utility.protobufInstanceForClassWithData(pclass, this.data);

		if (parsed == null) {
			throw new BlockParseException(this.type, String.format("protobuf<%s>", pclass.getSimpleName()));
		}

		return parsed;
	}

	private void parseTypeCheck(String ... rtypes) {
		for (String r : rtypes) {
			if (r.equals(type)) return;
		}

		throw new BlockParseException(type, rtypes);
	}

	@SuppressWarnings("serial")
	public static class BlockParseException extends RuntimeException {
		public BlockParseException(String type, String ... rtype) {
			super(String.format("Block of type '%s' cannot be parsed as any of '%s'",
					type, Arrays.toString(rtype)));
		}
	}
}
