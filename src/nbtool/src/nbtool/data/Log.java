package nbtool.data;

import java.io.Serializable;
import java.util.Map;

import javax.swing.tree.TreePath;

import nbtool.util.NBConstants;
import nbtool.util.U;

/*TODO:  if description changes, attributes is out of date.
 * */
public class Log implements Serializable {
	private static final long serialVersionUID = 5000703421741282261L;
	
	/*
	 * Unique number for every log found during this execution.
	 * */
	private static Long class_index = new Long(0);
	private static long getID() {
		long ret;
		synchronized(class_index) {
			ret = class_index++;
		}
		
		return ret;
	}
	public final long unique_id = getID();
	
	public String name; //File name, might be null
	
	//Core opaque log fields:
	public String description;
	public byte[] bytes;
	
	public SOURCE source;
	
	//Set by GUI when selected.
	public transient TreePath lastSeen = null;

	private SExpr tree = null;
	public SExpr tree() {
		if (tree == null)
			tree = SExpr.deserializeFrom(description);
		return tree;
	}
	
	public void setTree(SExpr nt) {
		this.tree = nt;
		this.description = nt.serialize();
	}
	
	public Log() {}
	
	public Log(String d, byte[] b) {
		this();
		
		this.name = null;
		this.tree = null;
		this.description = d;
		this.bytes = b;		
	}
	
	public Log(SExpr t, byte[] d) {
		this();
		
		this.name = null;
		this.tree = t;
		this.description = t.serialize();
		this.bytes = d;
	}
	
	public void setNameFromDesc() {
		this.name = String.format("i%d_type-%s_from-%s_v-%d", unique_id, pType(), pFrom(), version());
		this.name = this.name
				.substring(Math.max(0, this.name.length() - 240))
				.replace('/', '_').replace(' ', '_').replace(':', '-').replace('.', '-') + ".nblog";
	}
	
	public String toString() {
		if (name != null) return name;
		else return description;
	}
	
	public static enum SOURCE {
		DERIVED, FILE, NETWORK
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
	//what is this attribute going to return?
	//public String protoRobotLocation() {
		//return getAttributes().get("proto-RobotLocation");
	//}
	
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
	
	public int pBytes() {
		SExpr c = tree().find("contents").get(1).find("bytes").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	public String pType() {
		SExpr c = tree().find("contents").get(1).find("type").get(1);
		return c.exists() && c.isAtom() ? c.value() : null;
	}
	
	public String pFrom() {
		SExpr c = tree().find("contents").get(1).find("from").get(1);
		return c.exists() && c.isAtom() ? c.value() : null;
	}
	
	public int pI_Index() {
		SExpr c = tree().find("contents").get(1).find("i_index").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	public Long pTime() {
		SExpr c = tree().find("contents").get(1).find("time").get(1);
		return c.exists() && c.isAtom() ? c.valueAsLong() : null;
	}
	
	public boolean pIsProtobuf() {
		String t = pType();
		if (t == null)
			return false;
		return t.startsWith(NBConstants.PROTOBUF_TYPE_PREFIX);
	}
	
	/*
	 * Attributes relating to possible image content.
	 */
	
	public String pEncoding() {
		SExpr c = tree().find("contents").get(1).find("encoding").get(1);
		return c.exists() && c.isAtom() ? c.value() : null;
	}
	
	public int pWidth() {
		SExpr c = tree().find("contents").get(1).find("width").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
	
	public int pHeight() {
		SExpr c = tree().find("contents").get(1).find("height").get(1);
		return c.exists() && c.isAtom() ? c.valueAsInt() : null;
	}
}
