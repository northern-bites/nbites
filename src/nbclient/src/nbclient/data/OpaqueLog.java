package nbclient.data;

import java.io.File;
import java.io.Serializable;
import java.util.Map;

import nbclient.util.U;

/*TODO:  if description changes, attributes is out of date.
 * */
public class OpaqueLog implements Serializable {
	private static final long serialVersionUID = 5000703421741282261L;
	public String name; //File name, might be null
	public String description;
	public byte[] bytes;
	
	public SOURCE source;

	private Map<String, Object> dictionary;
	
	public OpaqueLog(String d, byte[] b) {
		this.name = null;
		this.description = d;
		this.bytes = b;
		dictionary = null;
	}
	
	//We cache the attributes which may be a bad idea.
	public Map<String, Object> getAttributes() {
		if (dictionary != null) return dictionary;
		
		dictionary = U.attributes(description);
		return dictionary;
	}
	
	public void dumpAttributes() {
		this.dictionary = null;
	}
	
	public void setNameFromDesc() {
		//Somewhat hackish, we want to get the time: field because it has the highest guarantee of uniqueness.
		//The time field is usually at the end of the string.
		this.name = this.description.substring(Math.max(0, this.description.length() - 256))
				.replace(' ', '_').replace('/', '-') + ".nblog";
	}
	
	public String getType() {
		return (String) this.getAttributes().get("type");
	}
	
	public Long getCreationTime() {
		return (Long) this.getAttributes().get("time");
	}
	
	public String toString() {
		return description;
	}
	
	public static enum SOURCE {
		DERIVED, FILE, NETWORK
	}
}
