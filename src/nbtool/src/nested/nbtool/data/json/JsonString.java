package nbtool.data.json;

import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.Json.JsonValueType;
import nbtool.util.Debug;

public class JsonString implements JsonValue {
	
	/* WARNING: value DOES NOT equal the string returned by serialize() and friends. */
	public String value;
	public String value() { return value; }
	
	@Override
	public String toString() {return value;}
	
	public JsonString(String v) {
		this.value = v;
	}

	@Override
	public JsonValueType type() {
		return JsonValueType.STRING;
	}

	@Override
	public String serialize() {
		return String.format("\"%s\"", escaped());
	}

	private String escaped() {
		StringBuilder builder = new StringBuilder(this.value);
		int i = 0;
		while (i < value.length()) {
			int j = value.indexOf(JsonParser.TokenType.STRING.CHARACTER,
					i);
			if (j >= 0) {
				if (j == 0 || value.charAt(j - 1) != '\\') {
					builder.insert(j, '\\');
					i = j + 2;
				} else {
					i = j + 1;
				}
			} else break;
		}
		
		return builder.toString();
	}

	@Override
	public String print() {
		return this.serialize();
	}

	@Override
	public String print(int indent) {
		return Json.prefix(indent) + this.serialize();
	}

	/* ****************************
	 * this block defines the standard (only) way to implement these methods.
	 * Because of Java syntax, there is no great way of moving these definitions
	 * across classes.  However, this section should be mirrored across Json classes  */
	@SuppressWarnings("unchecked")
	@Override
	public <T extends JsonValue> T cast() {
		return (T) this;
	}
	
	public JsonArray asArray() {
		return this.<JsonArray>cast();
	}
	
	public JsonBoolean asBoolean() {
		return this.<JsonBoolean>cast();
	}
	
	public JsonNumber asNumber() {
		return this.<JsonNumber>cast();
	}
	
	public JsonObject asObject() {
		return this.<JsonObject>cast();
	}
	
	public JsonString asString() {
		return this.<JsonString>cast();
	}
	
	/* end mirrored section
	 * ******************************/
	
	@Override
	public boolean equals(Object other) {
		if (other == null)
			return false;
		
		if (other instanceof String) {
			return value.equals(other);
		}
		
		if (other instanceof JsonString) {
			String ov = ((JsonString) other).value;
			return value.equals(ov);
		}
		
		return false;
	}
	
	@Override
	public int hashCode() {
		return value.hashCode();
	}

	@Override
	public JsonValue copy() {
		return new JsonString(value);
	}

	@Override
	public boolean congruent(JsonValue other) {
		if (other == null || other.type() != this.type())
			return false;
		
		return other.asString().value.equals(this.value);
	}
	
}
