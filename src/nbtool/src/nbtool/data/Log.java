package nbtool.data;

import java.io.Serializable;
import java.util.Map;

import javax.swing.tree.TreePath;

import nbtool.util.NBConstants;
import nbtool.util.Utility;

/*TODO:  if description changes, attributes is out of date.
 * */
public class Log implements Serializable {
	private static final long serialVersionUID = 5000703421741282261L;
	
	/*
	 * Unique number for every log found during this execution.
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
	
	//user supplied name, often null.  Used as file name.
	public String name; 
	
	//Core opaque log fields:
	public byte[] bytes;
	private SExpr tree = null;
	
	public String _olddesc_;
	
	public static enum SOURCE {
		DERIVED, FILE, NETWORK
	}
	
	public SOURCE source;
	
	//Set by GUI when selected.
	public transient TreePath lastSeen = null;

	
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
	
	public static Log logWithType(String type) {
		SExpr typeField = SExpr.newKeyValue("type", type);
		SExpr fieldList = SExpr.newList(typeField);
		
		SExpr topLevel = SExpr.newList(SExpr.newKeyValue("contents", fieldList));
		return new Log(topLevel, null);
	}
	
	public static Log logWithType(String type, byte[] b) {
		SExpr typeField = SExpr.newKeyValue("type", type);
		SExpr fieldList = SExpr.newList(typeField);
		
		SExpr topLevel = SExpr.newList(SExpr.newKeyValue("contents", fieldList));
		return new Log(topLevel, b);
	}
	
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
	
	public void setNameFromDesc() {
		this.name = String.format("i%d_type=%s_from=%s_v=%d", unique_id, primaryType(), primaryFrom(), version());
		this.name = this.name
				.substring(Math.max(0, this.name.length() - 240))
				.replace('/', '_').replace(' ', '_').replace(':', '-').replace('.', '-') + ".nblog";
	}
	
	public String toString() {
		if (name != null) return name;
		else return description();
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
	
	public int contentCount() {
		SExpr v = tree().find("contents");
		if (v.exists())
			return v.count();
		return -1;
	}
	
	/*
	 * Most logs have content count 1.  This returns attributes of the first content.
	 * (the "primary" content)
	 * */
	
	public int primaryBytes() {
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
	
	public int primaryI_Index() {
		SExpr c = tree().find("contents").get(1).find("i_index").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	public Long primaryTime() {
		SExpr c = tree().find("contents").get(1).find("time").get(1);
		return c.exists() && c.isAtom() ? c.valueAsLong() : null;
	}
	
	public boolean primaryIsProtobuf() {
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
	
	public int primaryWidth() {
		SExpr c = tree().find("contents").get(1).find("width").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	public int primaryHeight() {
		SExpr c = tree().find("contents").get(1).find("height").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
}
