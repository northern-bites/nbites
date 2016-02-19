package nbtool.data;

import nbtool.util.Logger;
import nbtool.util.NBConstants;
import nbtool.util.Utility;

public class Log {
	
	//Top-level keys
	public static final String LOG_FIRST_ATOM_S = "nblog";
	public static final String LOG_CONTENTS_S = "contents";
	public static final String LOG_CREATED_S = "created";
	public static final String LOG_VERSION_S = "version";
	public static final String LOG_CHECKSUM_S = "checksum";
	public static final String LOG_HOST_TYPE_S = "host_type";
	public static final String LOG_HOST_NAME_S = "host_name";
	public static final String LOG_FROM_ADDR_S = "from_address";
	
	//content item keys
	public static final String CONTENT_TYPE_S = "type";
	public static final String CONTENT_FROM_S = "from";
	public static final String CONTENT_WHEN_S = "when";
	public static final String CONTENT_IINDEX_S = "iindex";
	public static final String CONTENT_NBYTES_S = "nbytes";
	public static final String CONTENT_IMAGE_WIDTH_S = "width";
	public static final String CONTENT_IMAGE_HEIGHT_S = "height";
	public static final String CONTENT_IMAGE_ENCODING_S = "encoding";
	
	//command key
	public static final String COMMAND_FIRST_ATOM_S = "command";
	
	//nbcross wildcard
	public static final String NBCROSS_WILDCARD_TYPE = "__WILDCARD__";
	
	public Log() {}
	
	public Log(String d, byte[] b) {
		this();
		
		this.name = null;
		this.tree = SExpr.deserializeFrom(d);
		this.bytes = b;
	}
	
	public Log(SExpr t, byte[] d) {
		this();
		
		this.name = null;
		this.tree = t;
		this.bytes = d;
	}
	
	public static Log logWithType(String type) {
		return Log.logWithType(type, null);
	}
	
	public static Log logWithType(String type, byte[] b) {
		SExpr typeField = SExpr.newKeyValue(CONTENT_TYPE_S, type);
		SExpr fieldList = SExpr.newList(typeField);
		
		SExpr topLevel = SExpr.newList(SExpr.newAtom(LOG_FIRST_ATOM_S), SExpr.newKeyValue(LOG_CONTENTS_S, fieldList));
		return new Log(topLevel, b);
	}
	
	public static Log logWithTypePlus(String type, byte[] b, SExpr... fields) {
		SExpr typeField = SExpr.newKeyValue(CONTENT_TYPE_S, type);
		SExpr fieldList = SExpr.newList(typeField);
		fieldList.append(fields);
		
		SExpr topLevel = SExpr.newList(SExpr.newAtom(LOG_FIRST_ATOM_S), SExpr.newKeyValue(LOG_CONTENTS_S, fieldList));
		return new Log(topLevel, b);
	}
	
	public static Log simpleCommandLog(String cmndName, byte[] bytes) {
		SExpr commandTree = SExpr.newList(SExpr.newAtom(COMMAND_FIRST_ATOM_S), SExpr.newAtom(cmndName));
		Log cmnd = new Log(commandTree, bytes);
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
	public final long unique_id = getID();
	
	//Core opaque log fields:
	public byte[] bytes;
	private SExpr tree;
	
	//used in some rare legacy situations.  ALMOST ALWAYS NULL.
	public String _olddesc_;
	
	//user supplied name, often null.  Used as file name.
	public String name; 
	
	public static enum SOURCE {
		DERIVED,	//created programmatically from other logs
		FILE,		//loaded from filesystem
		NETWORK,	//streamed from network during this process's lifetime
		GENERATED	//totally synthetic
	}
	
	public SOURCE source;
	public Session parent = null;	//enclosing Session instance.
	
	public byte[] data() {
		return bytes;
	}
	
	public SExpr tree() {
		return tree;
	}
	
	public String description() {
		return tree.serialize();
	}
	
	public String description(int nchars) {
		String ser = tree.serialize();
		if (ser.length() > nchars) {
			return ser.substring(0, nchars - 3) + "...";
		} else return ser;
	}
	
	public void setTree(SExpr nt) {
		if (nt.isAtom()) {
			Logger.warnf("log tree being set atom: %s", nt.serialize());
		}
		
		this.tree = nt;
	}
	
	/* this is typically used as a programmatically generated file name
	 * so the most important quality of the created String is to be a valid and
	 * unique file name on all systems.
	 * */
	public void setNameFromDesc() {
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
	
	public String madeWhere() {
		SExpr where = tree().find(LOG_CREATED_S).get(1);
		if (where.exists() && where.isAtom())
			return where.value();
		else return null;
	}
	
	public String madeWhen() {
		SExpr when = tree().find(LOG_CREATED_S).get(2);
		if (when.exists() && when.isAtom())
			return when.value();
		else return null;
	}
	
	public Integer checksum() {
		SExpr cs = tree().find(LOG_CHECKSUM_S).get(1);
		if (cs.exists() && cs.isAtom())
			return cs.valueAsInt();
		else return null;
	}
	//what is this attribute going to return?
	//public String protoRobotLocation() {
		//return getAttributes().get("proto-RobotLocation");
	//}
	
	public Integer version() {
		SExpr v = tree().find(LOG_VERSION_S).get(1);
		if (v.exists() && v.isAtom())
			return v.valueAsInt();
		else return null;
	}
	
	//Does not include the "contents" key.
	public Integer contentCount() {
		SExpr v = tree().find(LOG_CONTENTS_S);
		if (v.exists())
			return (v.count() - 1);
		return -1;
	}
	
	/*
	 * Most logs have content count 1.  This returns attributes of the first content.
	 * (the "primary" content)
	 * */
	
	public Integer primaryBytes() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find("bytes").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	public String primaryType() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find(CONTENT_TYPE_S).get(1);
		return c.exists() && c.isAtom() ? c.value() : null;
	}
	
	public String primaryFrom() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find(CONTENT_FROM_S).get(1);
		return c.exists() && c.isAtom() ? c.value() : null;
	}
	
	public Integer primaryImgIndex() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find(CONTENT_IINDEX_S).get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	public Long primaryTime() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find("time").get(1);
		return c.exists() && c.isAtom() ? c.valueAsLong() : null;
	}
	
	public Boolean primaryIsProtobuf() {
		String t = primaryType();
		if (t == null)
			return false;
		return t.startsWith(NBConstants.PROTOBUF_TYPE_PREFIX);
	}
	
	/*
	 * Attributes relating to possible image content.
	 */
	
	public String primaryEncoding() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find(CONTENT_IMAGE_ENCODING_S).get(1);
		return c.exists() && c.isAtom() ? c.value() : null;
	}
	
	public Integer primaryWidth() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find(CONTENT_IMAGE_WIDTH_S).get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	public Integer primaryHeight() {
		SExpr c = tree().find(LOG_CONTENTS_S).get(1).find(CONTENT_IMAGE_HEIGHT_S).get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	/*
	 * Helpers for non-primary content items.
	 * */
	
	public Integer contentNumBytes(int index) {
		SExpr cont = tree().find(LOG_CONTENTS_S);
		if (!cont.exists() || index >= (cont.count() - 1) )
			return null;
		
		SExpr item = cont.get(index + 1);
		if (item.isAtom())
			return null;
		

		SExpr bytes = item.find(CONTENT_NBYTES_S).get(1);
		return bytes.exists() && bytes.isAtom() ? bytes.valueAsInt() : null;
	}
	
	public Integer contentOffset(int index) {
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
	
	public byte[] bytesForContentItem(int index) {
		Integer offset = contentOffset(index);
		Integer total = contentNumBytes(index);

		if (offset == null || total == null)
			return null;
		
		return Utility.subArray(bytes, offset, total);
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
		Log log = new Log(top, null);
		
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
