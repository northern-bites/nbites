package nbtool.data;

import java.io.File;
import java.io.Serializable;
import java.util.Map;

import nbtool.util.NBConstants;
import nbtool.util.U;

/*TODO:  if description changes, attributes is out of date.
 * */
public class Log implements Serializable {
	private static final long serialVersionUID = 5000703421741282261L;
	
	public String name; //File name, might be null
	
	//Core opaque log fields:
	public String description;
	public byte[] bytes;
	
	public SOURCE source;

	private Map<String, String> dictionary;
	
	public Log(String d, byte[] b) {
		this.name = null;
		this.description = d;
		this.bytes = b;
		dictionary = null;
	}
	
	//We cache the attributes which may be a bad idea.
	public Map<String, String> getAttributes() {
		if (dictionary != null) return dictionary;
		
		dictionary = U.attributes(description);
		return dictionary;
	}
	
	public void dumpAttributes() {
		dictionary = null;
		_index = null;
		_time = null;
		_height = null;
		_width = null;
		_checksum = null;
		_version = null;
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
	 * Some standard attributes parsed/cached.
	 * */
	
	public String type() {
		return getAttributes().get("type");
	}
	
	public String from() {
		return getAttributes().get("from");
	}
	
	public String encoding() {
		return getAttributes().get("encoding");
	}
	
	private Integer _index = null;
	public Integer index() {
		if (_index != null) return _index;
		
		String str = getAttributes().get("index");
		if (str == null) return null;
		else {
			_index = Integer.parseInt(str);
			return _index;
		}
	}
	
	private Long _time = null;
	public Long time() {
		if (_time != null) return _time;
		
		String str = getAttributes().get("time");
		if (str == null) return null;
		else {
			_time = Long.parseLong(str);
			return _time;
		}
	}
	
	private Integer _height = null;
	public Integer height() {
		if (_height != null) return _height;
		
		String str = getAttributes().get("height");
		if (str == null) return null;
		else {
			_height = Integer.parseInt(str);
			return _height;
		}
	}
	
	private Integer _width = null;
	public Integer width() {
		if (_width != null) return _width;
		
		String str = getAttributes().get("width");
		if (str == null) return null;
		else {
			_width = Integer.parseInt(str);
			return _width;
		}
	}
	
	private Integer _checksum = null;
	public Integer checksum() {
		if (_checksum != null) return _checksum;
		
		String str = getAttributes().get("checksum");
		if (str == null) return null;
		else {
			_checksum = Integer.parseInt(str);
			return _checksum;
		}
	}
	
	private Integer _version = null;
	public Integer version() {
		if (_version != null) return _version;
		
		String str = getAttributes().get("version");
		if (str == null) return null;
		else {
			_version = Integer.parseInt(str);
			return _version;
		}
	}
	
	public boolean isProtobuf() {
		return type().startsWith(NBConstants.PROTOBUF_TYPE_PREFIX);
	}
}
