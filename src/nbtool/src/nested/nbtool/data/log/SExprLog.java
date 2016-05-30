package nbtool.data.log;

import nbtool.data.SExpr;
import nbtool.util.Debug;
import nbtool.util.ToolSettings;
import nbtool.util.Utility;

public class SExprLog {
	
	//Top-level keys
	protected static final String LOG_FIRST_ATOM_S = "nblog";
	protected static final String LOG_CONTENTS_S = "contents";
	protected static final String LOG_CREATED_S = "created";
	protected static final String LOG_VERSION_S = "version";
	protected static final String LOG_CHECKSUM_S = "checksum";
	protected static final String LOG_HOST_TYPE_S = "host_type";
	protected static final String LOG_HOST_NAME_S = "host_name";
	protected static final String LOG_FROM_ADDR_S = "from_address";
	
	//content item keys
	protected static final String CONTENT_TYPE_S = "type";
	protected static final String CONTENT_FROM_S = "from";
	protected static final String CONTENT_WHEN_S = "when";
	protected static final String CONTENT_IINDEX_S = "iindex";
	protected static final String CONTENT_NBYTES_S = "nbytes";
	protected static final String CONTENT_IMAGE_WIDTH_S = "width";
	protected static final String CONTENT_IMAGE_HEIGHT_S = "height";
	protected static final String CONTENT_IMAGE_ENCODING_S = "encoding";
	
	//command key
	protected static final String COMMAND_FIRST_ATOM_S = "command";
	
	//nbcross wildcard
	protected static final String NBCROSS_WILDCARD_TYPE = "__WILDCARD__";
	
	protected SExprLog() {}
	
	protected SExprLog(String d, byte[] b) {
		this();
		
		this.name = null;
		this.tree = SExpr.deserializeFrom(d);
		this.bytes = b;
	}
	
	protected SExprLog(SExpr t, byte[] d) {
		this();
		
		this.name = null;
		this.tree = t;
		this.bytes = d;
	}
	
	protected static SExprLog logWithType(String type) {
		return SExprLog.logWithType(type, null);
	}
	
	protected static SExprLog logWithType(String type, byte[] b) {
		SExpr typeField = SExpr.newKeyValue(CONTENT_TYPE_S, type);
		SExpr fieldList = SExpr.newList(typeField);
		
		SExpr topLevel = SExpr.newList(SExpr.newAtom(LOG_FIRST_ATOM_S), SExpr.newKeyValue(LOG_CONTENTS_S, fieldList));
		return new SExprLog(topLevel, b);
	}
	
	protected static SExprLog logWithTypePlus(String type, byte[] b, SExpr... fields) {
		SExpr typeField = SExpr.newKeyValue(CONTENT_TYPE_S, type);
		SExpr fieldList = SExpr.newList(typeField);
		fieldList.append(fields);
		
		SExpr topLevel = SExpr.newList(SExpr.newAtom(LOG_FIRST_ATOM_S), SExpr.newKeyValue(LOG_CONTENTS_S, fieldList));
		return new SExprLog(topLevel, b);
	}
	
	protected static SExprLog simpleCommandLog(String cmndName, byte[] bytes) {
		SExpr commandTree = SExpr.newList(SExpr.newAtom(COMMAND_FIRST_ATOM_S), SExpr.newAtom(cmndName));
		SExprLog cmnd = new SExprLog(commandTree, bytes);
		return cmnd;
	}
	
	/*
	 * Unique number for every log found in this process.
	 * */
	private static final Object indexLock = new Object();
	private static long class_index = 0;
	private static long getID() {
		long ret;
		synchronized(indexLock) {
			ret = class_index++;
		}
		
		return ret;
	}
	protected final long unique_id = getID();
	
	//Core opaque log fields:
	protected byte[] bytes;
	private SExpr tree;
	
	//used in some rare legacy situations.  ALMOST ALWAYS NULL.
	protected String _olddesc_;
	
	//user supplied name, often null.  Used as file name.
	protected String name; 
	
	protected static enum SOURCE {
		DERIVED,	//created programmatically from other logs
		FILE,		//loaded from filesystem
		NETWORK,	//streamed from network during this process's lifetime
		GENERATED	//totally synthetic
	}
	
	protected SOURCE source;
	
	protected byte[] data() {
		return bytes;
	}
	
	protected SExpr tree() {
		return tree;
	}
	
	protected String description() {
		return tree.serialize();
	}
	
	protected String description(int nchars) {
		String ser = tree.serialize();
		if (ser.length() > nchars) {
			return ser.substring(0, nchars - 3) + "...";
		} else return ser;
	}
	
	protected void setTree(SExpr nt) {
		if (nt.isAtom()) {
			Debug.warn("log tree being set atom: %s", nt.serialize());
		}
		
		this.tree = nt;
	}
	
	/* this is typically used as a programmatically generated file name
	 * so the most important quality of the created String is to be a valid and
	 * unique file name on all systems.
	 * */
	protected void setNameFromDesc() {
		this.name = String.format("type=%s_from=%s_v=%d_i%d_c%d", primaryType(), primaryFrom(), version(),
				this.unique_id, this.checksum());
		this.name = this.name
				.substring(Math.max(0, this.name.length() - 240))
				.replace('/', '_').replace(' ', '_').replace(':', '-').replace('.', '-') + ".nblog";
	}
	
	public String toString() {
		if (name != null) return name;
		else return description(100);
	}
	
	/* ALL ATTRIBUTES MUST BE OBJECTS SO THAT NULL CAN BE RETURNED IF THEY'RE NOT FOUND */
	/*
	 * Attributes relating to the log whole
	 */
	
	protected String madeWhere() {
		SExpr where = tree().find(LOG_CREATED_S).get(1);
		if (where.exists() && where.isAtom())
			return where.value();
		else return null;
	}
	
	protected String madeWhen() {
		SExpr when = tree().find(LOG_CREATED_S).get(2);
		if (when.exists() && when.isAtom())
			return when.value();
		else return null;
	}
	
	protected Integer checksum() {
		SExpr cs = tree().find(LOG_CHECKSUM_S).get(1);
		if (cs.exists() && cs.isAtom())
			return cs.valueAsInt();
		else return null;
	}
	//what is this attribute going to return?
	//protected String protoRobotLocation() {
		//return getAttributes().get("proto-RobotLocation");
	//}
	
	protected Integer version() {
		SExpr v = tree().find(LOG_VERSION_S).get(1);
		if (v.exists() && v.isAtom())
			return v.valueAsInt();
		else return null;
	}
	
	//Does not include the "contents" key.
	protected Integer contentCount() {
		SExpr v = tree().find(LOG_CONTENTS_S);
		if (v.exists())
			return (v.count() - 1);
		return -1;
	}
	
	/*
	 * Most logs have content count 1.  This returns attributes of the first content.
	 * (the "primary" content)
	 * */
	
	protected Integer primaryBytes() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find("bytes").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	protected String primaryType() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find(CONTENT_TYPE_S).get(1);
		return c.exists() && c.isAtom() ? c.value() : null;
	}
	
	protected String primaryFrom() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find(CONTENT_FROM_S).get(1);
		return c.exists() && c.isAtom() ? c.value() : null;
	}
	
	protected Integer primaryImgIndex() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find(CONTENT_IINDEX_S).get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	protected Long primaryTime() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find("time").get(1);
		return c.exists() && c.isAtom() ? c.valueAsLong() : null;
	}
	
	protected Boolean primaryIsProtobuf() {
		String t = primaryType();
		if (t == null)
			return false;
		return t.startsWith(ToolSettings.PROTOBUF_TYPE_PREFIX);
	}
	
	/*
	 * Attributes relating to possible image content.
	 */
	
	protected String primaryEncoding() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find(CONTENT_IMAGE_ENCODING_S).get(1);
		return c.exists() && c.isAtom() ? c.value() : null;
	}
	
	protected Integer primaryWidth() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find(CONTENT_IMAGE_WIDTH_S).get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	protected Integer primaryHeight() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find(CONTENT_IMAGE_HEIGHT_S).get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	/*
	 * Helpers for non-primary content items.
	 * 
	 * 0 based – index 0 refers to the content item after the 'contents' key
	 * */
	
	protected Integer contentNumBytes(int index) {
		SExpr cont = tree().find(LOG_CONTENTS_S);
		if (!cont.exists() || index >= (cont.count() - 1) )
			return null;
		
		SExpr item = cont.get(index + 1);
		if (item.isAtom())
			return null;
		

		SExpr bytes = item.find(CONTENT_NBYTES_S).get(1);
		return bytes.exists() && bytes.isAtom() ? bytes.valueAsInt() : null;
	}
	
	protected Integer contentOffset(int index) {
		SExpr cont = tree().find(LOG_CONTENTS_S);
		if (!cont.exists() || index >= (cont.count() - 1) )
			return null;
		
		int offset = 0;
		
		for (int i = 0; i < index; ++i) {
			SExpr bytes = cont.get(i + 1).find(CONTENT_NBYTES_S).get(1);
			if (!bytes.exists() || !bytes.isAtom())
				return null;
			offset += bytes.valueAsInt();
		}
		
		return offset;
	}
	
	protected byte[] bytesForContentItem(int index) {
		Integer offset = contentOffset(index);
		Integer total = contentNumBytes(index);

		if (offset == null || total == null)
			return null;
		
		return Utility.subArray(bytes, offset, total);
	}
	
	protected SExpr sexprForContentItem(int index) {
		SExpr cont = tree().find(LOG_CONTENTS_S);
		if (!cont.exists() || index >= (cont.count() - 1) )
			return null;
		
		return cont.get(index + 1);
	}
	
	//TESTING
	public static void main(String[] args) {
		SExpr clist = SExpr.newList(
				SExpr.newAtom("contents"),
				SExpr.newList(SExpr.newKeyValue("bytes", "10")),
				SExpr.newList(SExpr.newKeyValue("bytes", "50")),
				SExpr.newList(SExpr.newKeyValue("bytes", "100"))
				);
		
		SExpr top = SExpr.newList(clist);
		SExprLog log = new SExprLog(top, null);
		
		System.out.println("desc  = " + log.description());
		System.out.println("count = " + log.contentCount());
		System.out.println("b0    = " + log.contentNumBytes(0));
		System.out.println("b2    = " + log.contentNumBytes(2));
		System.out.println("b3    = " + log.contentNumBytes(3));
		System.out.println("");
		System.out.println("o0    = " + log.contentOffset(0));
		System.out.println("o2    = " + log.contentOffset(2));
	}
}
