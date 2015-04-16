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

	public SExpr tree;
	
	public Log() {}
	
	public Log(String d, byte[] b) {
		this();
		
		this.name = null;
		this.description = d;
		this.bytes = b;
		
		this.tree = SExpr.deserializeFrom(description);
	}
	
	public void dumpAttributes() {
		
	}
	
	public void setNameFromDesc() {
		this.name = String.format("type=%s_from=%s_time=%d_v=%d", type(), from(), time(), version());
		this.name = this.name
				.substring(Math.max(0, this.name.length() - 240)).replace('/', '-') + ".nblog";
	}
	
	public String toString() {
		if (name != null) return name;
		else return description;
	}
	
	public static enum SOURCE {
		DERIVED, FILE, NETWORK
	}
	
	/*
	 * Some standard attributes parsed.
	 * */
	
	public String type() {
		SExpr type = tree.find("contents").get(1).find("type").get(1);
		if (type.exists() && type.isAtom())
			return type.value();
		else return null;
	}
	
	public String from() {
		SExpr from = tree.find("created").get(1);
		if (from.exists() && from.isAtom())
			return from.value();
		else return null;
	}
	
	public String encoding() {
		return null;
	}
	
	public Integer index() {
		return 0;
	}
	
	public Long time() {
		return 0L;
	}
	
	
	public Integer height() {
		SExpr height = tree.find("contents").get(1).find("height").get(1);
		
		if (height.exists() && height.isAtom())
			return height.valueAsInt();
		else return null;
	}
	
	public Integer width() {
		SExpr width = tree.find("contents").get(1).find("width").get(1);
		
		if (width.exists() && width.isAtom())
			return width.valueAsInt();
		else return null;
	}
	
	public Integer checksum() {
		SExpr cs = tree.find("checksum").get(1);
		if (cs.exists() && cs.isAtom())
			return cs.valueAsInt();
		else return null;
	}
	
	public Integer version() {
		SExpr v = tree.find("version").get(1);
		if (v.exists() && v.isAtom())
			return v.valueAsInt();
		else return null;
	}
	
	public boolean isProtobuf() {
		return type().startsWith(NBConstants.PROTOBUF_TYPE_PREFIX);
	}
}
