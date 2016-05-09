package nbtool.data.log;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.Arrays;
import java.util.Vector;

import com.google.protobuf.Message;

import messages.InertialState;
import messages.JointAngles;
import nbtool.data.json.Json.JsonValue;
import nbtool.data.SExpr;
import nbtool.data.json.Json;
import nbtool.data.json.JsonArray;
import nbtool.data.json.JsonObject;
import nbtool.data.json.JsonParser.JsonParseException;
import nbtool.images.YUYV8888Image;
import nbtool.util.Debug;
import nbtool.util.Debug.DebugSettings;
import nbtool.util.SharedConstants;
import nbtool.util.ToolSettings;
import nbtool.util.Utility.Pair;
import nbtool.util.test.TestBase;
import nbtool.util.test.Tests;

public class LogInternal extends Log {
	
	private static final DebugSettings debug = Debug.createSettings(true, true, true, Debug.INFO, null);
	
	@Override
	public String toString() {
		return String.format("%s(id:%d)", this.getClass().getName(), this.jvm_unique_id);
	}
	
	private Pair<JsonObject, byte[]> getParts() {
		JsonObject top = this.topLevelDictionary.copy().asObject();
		
		top.put(SharedConstants.LOG_TOPLEVEL_MAGIC_KEY(), ToolSettings.VERSION);
		top.put(SharedConstants.LOG_TOPLEVEL_LOGCLASS(), logClass);
		top.put(SharedConstants.LOG_TOPLEVEL_CREATED_WHEN(), createdWhen);
		top.put(SharedConstants.LOG_TOPLEVEL_HOST_TYPE(), host_type);
		top.put(SharedConstants.LOG_TOPLEVEL_HOST_NAME(), host_name);
		top.put(SharedConstants.LOG_TOPLEVEL_HOST_ADDR(), host_addr);
		
		int block_bytes = 0;
		JsonArray blockArray = new JsonArray();
		top.put(SharedConstants.LOG_TOPLEVEL_BLOCKS(), blockArray);
		
		for (Block b : this.blocks) {
			if (b.data != null)
				block_bytes += b.data.length;
			blockArray.add(b.getFullDictionary());
		}
		
		byte[] data = new byte[block_bytes];
		block_bytes = 0;
		for (Block b : this.blocks) {
			if (b.data != null) {
				System.arraycopy(b.data, 0, data, block_bytes, b.data.length);
				block_bytes += b.data.length;
			}
		}
		
		debug.event("LOG PAIR: %d bytes, %s", data.length, top.serialize());
		
		assert(block_bytes == data.length);
		return new Pair<JsonObject, byte[]>(top, data);
	}
	
	@Override
	public String getFullDescription() {
		return getParts().a.serialize();
	}

	@Override
	public byte[] serialize() {
		Pair<JsonObject, byte[]> parts = getParts();
		byte[] ddata = parts.a.serialize().getBytes(StandardCharsets.UTF_8);		
		ByteArrayOutputStream baos = new ByteArrayOutputStream(ddata.length + parts.b.length + 8);
		DataOutputStream dos = new DataOutputStream(baos);
		try {
			dos.writeInt(ddata.length);
			dos.write(ddata);
			dos.writeInt(parts.b.length);
			dos.write(parts.b);
			
			assert(dos.size() == baos.size());
			assert(dos.size() == (ddata.length + parts.b.length + 8));
			
			return baos.toByteArray();
		} catch (IOException e) {
			e.printStackTrace();
			assert(false);
		}
		
		return null;
	}

	@Override
	public void writeTo(OutputStream os) throws IOException {		
		os.write(serialize());
	}

	@Override
	public int version() {
		return this.topLevelDictionary.get(SharedConstants.LOG_TOPLEVEL_MAGIC_KEY()).asNumber().asInt();
	}
	
	@Override
	public boolean temporary() {
		return logReference == null ? true : logReference.temporary();
	}

	@Override
	public void saveChangesToTempFile() throws Exception {
		if (logReference != null) {
			logReference.pushToTempFile(this);
		} else {
			throw new Exception("cannot save changes to untracked Log!");
		}
	}

	@Override
	public void saveChangesToLoadFile() throws Exception {
		if (logReference != null) {
			logReference.pushToLoadFile(this);
		} else {
			throw new Exception("cannot save changes to untracked Log!");
		}
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
		ret.host_name = "computer-" + System.getProperty("user.name");
		ret.host_addr = "n/a";
		
		return ret;
	}
		
	public static Log explicitLog(Vector<Block> blocks, JsonObject topLevel, String logClass,
			long created) {
		Log ret = emptyLog();
		ret.blocks = blocks;
		ret.topLevelDictionary = topLevel;
		ret.logClass = logClass;
		ret.createdWhen = created;
		
		if (topLevel.containsKey(SharedConstants.LOG_TOPLEVEL_HOST_ADDR())) {
			ret.host_addr = stringRemove(SharedConstants.LOG_TOPLEVEL_HOST_ADDR(), topLevel);
		}
		
		if (topLevel.containsKey(SharedConstants.LOG_TOPLEVEL_HOST_NAME())) {
			ret.host_name = stringRemove(SharedConstants.LOG_TOPLEVEL_HOST_NAME(), topLevel);
		}
		
		if (topLevel.containsKey(SharedConstants.LOG_TOPLEVEL_HOST_TYPE())) {
			ret.host_type = stringRemove(SharedConstants.LOG_TOPLEVEL_HOST_TYPE(), topLevel);
		}
		
		return ret;
	}
	
	public static Log parseFrom(byte[] bytes) {
		DataInputStream dis = new DataInputStream(
				new ByteArrayInputStream(bytes));
		
		try {
			int descLen = dis.readInt();
			byte[] desc = new byte[descLen];
			dis.readFully(desc);
			int dataLen = dis.readInt();
			byte[] data = new byte[dataLen];
			dis.readFully(data);
			
			return parseFromParts(desc, data);
			
		} catch (IOException e) {
			e.printStackTrace();
			assert(false);
		}
		
		return null;
	}
	
	public static Log parseFromParts(byte[] json, byte[] data) {
		
		String desc = new String(json);
		
		if (LogInternal.isLegacyDesc(desc)) {
			debug.warn("converting legacy log to JSON format...");
			return LogInternal.parseLegacy(desc, data);
		}
		
		if (!desc.contains(SharedConstants.LOG_TOPLEVEL_MAGIC_KEY())) {
			debug.error("parsed log does not contain magic key: %s !in $s", SharedConstants.LOG_TOPLEVEL_MAGIC_KEY(),
					desc);
			throw new RuntimeException("parsed log does not contain magic key!");
		}
		
		JsonObject object = null;
		try {
			object = Json.parseAndRequireEnd(desc).asObject();
		} catch (JsonParseException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		}
		
		int vers = object.get(SharedConstants.LOG_TOPLEVEL_MAGIC_KEY()).asNumber().asInt();
		if (vers != ToolSettings.VERSION) {
			debug.warn("parsing log of different version to compiled tool! tool:%d log:%d",
					ToolSettings.VERSION, vers);
		}
		
		String logClass = valueRemove(SharedConstants.LOG_TOPLEVEL_LOGCLASS(), object).asString().value;
		long cwhen = valueRemove(SharedConstants.LOG_TOPLEVEL_CREATED_WHEN(), object).asNumber().asLong();
		
		JsonArray blockArray = valueRemove(SharedConstants.LOG_TOPLEVEL_BLOCKS(), object).asArray();
		Vector<Block> blocks = new Vector<>();
		int offset = 0;
		
		for (int i = 0; i < blockArray.size(); ++i) {
			JsonObject bdict = blockArray.get(i).asObject();
			
			int bytes = valueRemove(SharedConstants.LOG_BLOCK_NUM_BYTES(), bdict).asNumber().asInt();
			String type = valueRemove(SharedConstants.LOG_BLOCK_TYPE(), bdict).asString().value;
			String from = valueRemove(SharedConstants.LOG_BLOCK_WHERE_FROM(), bdict).asString().value;
			long ii = valueRemove(SharedConstants.LOG_BLOCK_IMAGE_INDEX(), bdict).asNumber().asLong();
			long bwm = valueRemove(SharedConstants.LOG_BLOCK_WHEN_MADE(), bdict).asNumber().asLong();
			
			blocks.add(new Block(
					Arrays.copyOfRange(data, offset, offset + bytes),
					bdict,
					type,
					from,
					ii,
					bwm
					));
			
			offset += bytes;
		}
		
		assert(offset == data.length);
		 
		return LogInternal.explicitLog(blocks, object, logClass, cwhen);
	}
	
	public static Log parseFromStream(InputStream is) throws IOException {
		DataInputStream dis = new DataInputStream(is);
		
		int descLen = dis.readInt();
		byte[] desc = new byte[descLen];
		dis.readFully(desc);
		int dataLen = dis.readInt();
		byte[] data = new byte[dataLen];
		dis.readFully(data);
		
		return parseFromParts(desc, data);
	}
	
	private static String stringRemove(String key, JsonObject obj) {
		if (obj.containsKey(key)) {
			return obj.get(key).asString().value;
		} else return "";
	}

	private static JsonValue valueRemove(String key, JsonObject obj) {
		if (obj.containsKey(key)) {
			JsonValue val = obj.get(key);
			obj.remove(key);
			return val;
		} else {
			debug.error("required key '%s' not found in Log JsonObject", key);
			return null;
		}
	}
	
	@Override
	protected void finalize() throws Throwable {
		super.finalize();
		
		if (logReference != null) {
			logReference.pushToTempFileNow(this);
		}
		//Don't pushLoad, that action must always be explicitly done by user code.
	}
	
	@Override
	public boolean addBlockFromProtobuf(Message message, String whereFrom, long imageIndex, long createdWhen) {
		String type = message.getClass().getSimpleName();
		byte[] data = message.toByteArray();
		
		blocks.add(new Block(
				data,
				new JsonObject(),
				type, whereFrom, imageIndex, createdWhen
				));
		
		return true;
	}

	@Override
	public boolean addBlockFromImage(YUYV8888Image image, String whereFrom, long imageIndex, long createdWhen) {
		
		String type = SharedConstants.YUVImageType_DEFAULT();
		byte[] data = Arrays.copyOf(image.data, image.data.length);
		
		blocks.add(new Block(
				data,
				new JsonObject(),
				type, whereFrom, imageIndex, createdWhen
				));
		
		return true;
	}

	@Override
	public boolean addBlockFromSexpr(SExpr sexpr, String whereFrom, long imageIndex, long createdWhen) {
		
		String type = SharedConstants.SexprType_DEFAULT();
		byte[] data = sexpr.serialize().getBytes(StandardCharsets.UTF_8);
		
		blocks.add(new Block(
				data,
				new JsonObject(),
				type, whereFrom, imageIndex, createdWhen
				));
		
		return true;
	}

	@Override
	public boolean addBlockFromJson(JsonValue val, String whereFrom, long imageIndex, long createdWhen) {
		
		String type = SharedConstants.JsonType_DEFAULT();
		byte[] data = val.serialize().getBytes(StandardCharsets.UTF_8);
		
		blocks.add(new Block(
				data,
				new JsonObject(),
				type, whereFrom, imageIndex, createdWhen
				));
		
		return true;
	}

	@Override
	public boolean addBlockFromLog(Log log) {
		
		String type = SharedConstants.LogType_DEFAULT();
		byte[] data = log.serialize();
		
		blocks.add(new Block(
				data,
				new JsonObject(),
				type, "n/a", 0, 0
				));
		
		return true;
	}
	
	private static boolean isLegacyDesc(String desc) {
		char sc = desc.trim().charAt(0);
		switch(sc) {
		case '{':
			return false;
		case '(':
			return true;
		default:
			debug.error("pre-version 6 description: %s", desc);
			throw new RuntimeException("LogInternal cannot parse pre-version 6 log!");
		}
	}
	
	private static Log parseLegacy(String desc, byte[] data) {
		SExprLog slog = new SExprLog(desc, data);
		
//		debug.printf("%s", desc);
//		debug.printf("%s %s %s", slog.primaryType(), slog.primaryFrom(), slog.primaryEncoding());
		
		if (slog.contentCount() < 3 ||
				!slog.primaryType().equals("YUVImage") ||
				!slog.madeWhere().equals("tripoint") ||
				!slog.primaryEncoding().equals("[Y8(U8/V8)]") ) {
			debug.warn("cannot parse non-tripoint legacy log, returning null");
			return null;
		}
		
		JsonObject imDict = Json.object();
		imDict.put(SharedConstants.LOG_BLOCK_IMAGE_WIDTH_PIXELS(), slog.primaryWidth());
		imDict.put(SharedConstants.LOG_BLOCK_IMAGE_HEIGHT_PIXELS(), slog.primaryHeight());
		Vector<Block> blocks = new Vector<>();
		
		blocks.add(new Block(
				slog.bytesForContentItem(0), imDict,
				SharedConstants.YUVImageType_DEFAULT(),
				slog.madeWhere(),
				slog.primaryImgIndex(),
//				slog.primaryTime() 
				0L
				));
		blocks.add(new Block(
				slog.bytesForContentItem(1), new JsonObject(),
				"InertialState", "", 0, 0
				));
		blocks.add(new Block(
				slog.bytesForContentItem(2), new JsonObject(),
				"JointAngles", "", 0, 0
				));

		return Log.explicitLog(blocks, new JsonObject(), SharedConstants.LogClass_Tripoint(), 0L);		
	}
	
	public static void _NBL_ADD_TESTS_() {
		Tests.add("data.log", 
			new TestBase("basic tests"){
			
			@Override
			public boolean testBody() throws Exception {
				
				Log test1 = Log.explicitLog(new Vector<Block>(), new JsonObject(), "classy", 0L);
//				debug.info("%s", test1.getFullDescription());
				byte[] testBytes = test1.serialize();
				Log test2 = Log.parseFrom(testBytes);
				
//				debug.info("\t\tjvm_unique...");
				assert (test2.jvm_unique_id == test1.jvm_unique_id + 1);
//				debug.info("\t\tparts equal...");
				Pair<JsonObject, byte[]> t1P = ((LogInternal) test1).getParts();
				Pair<JsonObject, byte[]> t2P = ((LogInternal) test2).getParts();
				
//				debug.info("%s", t1P.a.serialize());
				assert(t1P.a.congruent(t2P.a));
				assert (Arrays.equals(t1P.b, t2P.b));
				
				return true;
			}
		
		}, new TestBase("parseFrom_bytes") {

			@Override
			public boolean testBody() throws Exception {
				
				Log outer = Log.emptyLog();
				Log a1 = Log.explicitLog(new Vector<Block>(), new JsonObject(), "none", 0);
				Log a2 = Log.explicitLog(new Vector<Block>(), new JsonObject(), "thingy", 0);
				SExpr sexpr = SExpr.deserializeFrom("(none, none, ())");

				outer.addBlockFromLog(a1);
				outer.addBlockFromLog(a2);
				outer.addBlockFromSexpr(sexpr, "testing", 0, 0);
				outer.blocks.add(new Block(null,  new JsonObject(),
						"none", "", 0, 0 ));
				
				byte[] bytes = outer.serialize();
				Log parsed = Log.parseFrom(bytes);
				assert(parsed.blocks.size() == 4);
				
				Block b1 = parsed.blocks.get(0);
				Block b2 = parsed.blocks.get(2);
				Block b3 = parsed.blocks.get(3);
				Debug.print("%d bytes", b3.data.length);
				assert(b3.data.length == 0);
				assert(b1.type.equals(SharedConstants.LogType_DEFAULT()));
				assert(b2.parseAsSExpr().serialize().equals(sexpr.serialize()));
				
				Log none = Log.emptyLog();
				Log np = Log.parseFrom(none.serialize());
				assert(np.host_type.equals(String.format("nbtool-v%d",
						ToolSettings.VERSION)));
				
				return true;
			}
			
		}, new TestBase("parseFrom_stream") {

			@Override
			public boolean testBody() throws Exception {
				
				SExpr sexpr = SExpr.deserializeFrom("(none, none, ())");
				InputStream is = TestBase.resourceAtClass(this, "testLog1");
				Log pstream = Log.parseFromStream(is);
				assert(pstream.blocks.size() == 4);
				Block b22 = pstream.blocks.get(2);
				assert(b22.parseAsSExpr().serialize().equals(sexpr.serialize()));
				Debug.info("parseFromStream OK...");
				
				return true;
			}
			
		}, new TestBase("legacy") {

			@Override
			public boolean testBody() throws Exception {
				
				Log test = Log.parseFromStream(TestBase.resourceAtClass(this, "testLog2"));
				YUYV8888Image img = test.blocks.get(0).parseAsYUVImage();
				Message m1 = test.blocks.get(1).parseAsProtobufOfClass(InertialState.class);
				Message m2 = test.blocks.get(2).parseAsProtobufOfClass(JointAngles.class);
				
				return true;
			}
			
		}, new TestBase("temporary_and_pushing") {

			@Override
			public boolean testBody() throws Exception {
				
				Log test = Log.emptyLog();
				assert(test.logReference == null);
				assert(test.temporary());
				
				
				
				return true;
			}
			
		}			
				);
	}

}
