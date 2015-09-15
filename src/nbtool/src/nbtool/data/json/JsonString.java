package nbtool.data.json;

import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.Json.JsonValueType;
import nbtool.util.Logger;

public class JsonString implements JsonValue {
	
	/* WARNING: value DOES NOT equal the string returned by serialize() and friends. */
	public String value;
	public String value() { return value; }
	
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

	@SuppressWarnings("unchecked")
	@Override
	public <T extends JsonValue> T cast() {
		return (T) this;
	}
	
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
}
