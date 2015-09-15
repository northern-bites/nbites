package nbtool.data.json;

import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.Json.JsonValueType;

public class JsonNumber extends Number implements JsonValue {
	
	private String representation;
	
	public JsonNumber(int val) {
		this.representation = Integer.toString(val);
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
	
	@Override
	public int intValue() {
		if (representation.startsWith("0x")) {
			return Integer.decode(representation);
		} else {
			return Integer.parseInt(representation);
		}
	}

	@Override
	public long longValue() {
		if (representation.startsWith("0x")) {
			return Long.decode(representation);
		} else {
			return Long.parseLong(representation);
		}
	}

	@Override
	public float floatValue() {
		return Float.parseFloat(representation);
	}

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
}
