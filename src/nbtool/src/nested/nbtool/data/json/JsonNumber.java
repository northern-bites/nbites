package nbtool.data.json;

import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.Json.JsonValueType;

public class JsonNumber extends Number implements JsonValue {
	
	private String representation;
	
	public JsonNumber(int val) {
		this.representation = Integer.toString(val);
	}
	
	public JsonNumber(long val) {
		this.representation = Long.toString(val);
	}
	
	public JsonNumber(double val) {
		this.representation = Double.toString(val);
	}
	
	public JsonNumber(float val) {
		this.representation = Float.toString(val);
	}
	
	public JsonNumber(String text) {
		checkValidNumberString(text);
		this.representation = text.trim();
	}
	
	public int asInt() {return intValue();}
	
	@Override
	public int intValue() {
		if (representation.startsWith("0x")) {
			return Integer.decode(representation);
		} else {
			return Integer.parseInt(representation);
		}
	}

	public long asLong() {return longValue();}
	
	@Override
	public long longValue() {
		if (representation.startsWith("0x")) {
			return Long.decode(representation);
		} else {
			return Long.parseLong(representation);
		}
	}

	public float asFloat(){return floatValue();}
	
	@Override
	public float floatValue() {
		return Float.parseFloat(representation);
	}

	public double asDouble(){return doubleValue();}
	
	@Override
	public double doubleValue() {
		return Double.parseDouble(representation);
	}

	@Override
	public JsonValueType type() {
		return JsonValueType.NUMBER;
	}

	@Override
	public String serialize() {
		return representation;
	}

	@Override
	public String print() {
		return this.serialize();
	}
	
	@Override
	public String toString() {
		return "JsonNumber(" + representation + ")";
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
		return ( other != null && (other instanceof JsonNumber)
				&& ((JsonNumber) other).representation.equals(representation) );
	}
	
	//Check format by trying to parse the text.
	public static void checkValidNumberString(String str) throws NumberFormatException {
		//Double.parseDouble doesn't like hex strings
		if (str.trim().startsWith("0x")) {
			Integer.decode(str);
		} else {
			Double.parseDouble(str);
		}
	}
	
	@Override
	public JsonValue copy() {
		return new JsonNumber(representation);
	}

	@Override
	public boolean congruent(JsonValue other) {
		if (other == null || other.type() != this.type()) 
			return false;
		
		return other.asNumber().representation.equalsIgnoreCase(this.representation);
	}
}
