package nbtool.data;

import nbtool.util.NBConstants;

public class Log {	
	
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
		SExpr typeField = SExpr.newKeyValue("type", type);
		SExpr fieldList = SExpr.newList(typeField);
		
		SExpr topLevel = SExpr.newList(SExpr.newAtom("nblog"), SExpr.newKeyValue("contents", fieldList));
		return new Log(topLevel, b);
	}
	
	
	
	public static Log logWithTypePlus(String type, byte[] b, SExpr... fields) {
		SExpr typeField = SExpr.newKeyValue("type", type);
		SExpr fieldList = SExpr.newList(typeField);
		fieldList.append(fields);
		
		SExpr topLevel = SExpr.newList(SExpr.newAtom("nblog"), SExpr.newKeyValue("contents", fieldList));
		return new Log(topLevel, b);
	}
	
	public static Log simpleCommandLog(String cmndName, byte[] bytes) {
		SExpr commandTree = SExpr.newList(SExpr.newAtom("command"), SExpr.newAtom(cmndName));
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
		DERIVED, FILE, NETWORK, GENERATED
	}
	
	public SOURCE source;
	public Session parent;
	
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
			return ser.substring(0, nchars);
		} else return ser;
	}
	
	public void setTree(SExpr nt) {
		this.tree = nt;
	}
	
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
		SExpr where = tree().find("created").get(1);
		if (where.exists() && where.isAtom())
			return where.value();
		else return null;
	}
	
	public String madeWhen() {
		SExpr when = tree().find("created").get(2);
		if (when.exists() && when.isAtom())
			return when.value();
		else return null;
	}
	
	public Integer checksum() {
		SExpr cs = tree().find("checksum").get(1);
		if (cs.exists() && cs.isAtom())
			return cs.valueAsInt();
		else return null;
	}
	
	public Integer version() {
		SExpr v = tree().find("version").get(1);
		if (v.exists() && v.isAtom())
			return v.valueAsInt();
		else return null;
	}
	
	//Does not include the "contents" key.
	public Integer contentCount() {
		SExpr v = tree().find("contents");
		if (v.exists())
			return (v.count() - 1);
		return -1;
	}
	
	/*
	 * Most logs have content count 1.  This returns attributes of the first content.
	 * (the "primary" content)
	 * */
	
	public Integer primaryBytes() {
		SExpr c = tree().find("contents").get(1).find("bytes").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	public String primaryType() {
		SExpr c = tree().find("contents").get(1).find("type").get(1);
		return c.exists() && c.isAtom() ? c.value() : null;
	}
	
	public String primaryFrom() {
		SExpr c = tree().find("contents").get(1).find("from").get(1);
		return c.exists() && c.isAtom() ? c.value() : null;
	}
	
	public Integer primaryImgIndex() {
		SExpr c = tree().find("contents").get(1).find("iindex").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	public Long primaryTime() {
		SExpr c = tree().find("contents").get(1).find("time").get(1);
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
		SExpr c = tree().find("contents").get(1).find("encoding").get(1);
		return c.exists() && c.isAtom() ? c.value() : null;
	}
	
	public Integer primaryWidth() {
		SExpr c = tree().find("contents").get(1).find("width").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	public Integer primaryHeight() {
		SExpr c = tree().find("contents").get(1).find("height").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	/*
	 * Helpers for non-primary content items.
	 * */
	
	public Integer contentNumBytes(int index) {
		SExpr cont = tree().find("contents");
		if (!cont.exists() || index >= (cont.count() - 1) )
			return null;
		
		SExpr item = cont.get(index + 1);
		if (item.isAtom())
			return null;
		
		SExpr bytes = item.find("bytes").get(1);
		return bytes.exists() && bytes.isAtom() ? bytes.valueAsInt() : null;
	}
	
	public Integer contentOffset(int index) {
		SExpr cont = tree().find("contents");
		if (!cont.exists() || index >= (cont.count() - 1) )
			return null;
		
		int offset = 0;
		
		for (int i = 0; i < index; ++i) {
			SExpr bytes = cont.get(i + 1).find("bytes").get(1);
			if (!bytes.exists() || !bytes.isAtom())
				return null;
			offset += bytes.valueAsInt();
		}
		
		return offset;
	}
	
	//TESTING
	public static void main(String[] args) {
		SExpr clist = SExpr.newList(
				SExpr.newAtom("contents"),
				SExpr.newList(SExpr.newKeyValue("bytes", 10)),
				SExpr.newList(SExpr.newKeyValue("bytes", 50)),
				SExpr.newList(SExpr.newKeyValue("bytes", 100))
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
