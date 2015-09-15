package nbtool.data.json;

import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.Json.JsonValueType;

public class JsonBoolean implements JsonValue {
	
	private boolean value;

	@Override
	public JsonValueType type() {
		return JsonValueType.BOOLEAN;
	}

	@Override
	public String serialize() {
		return Boolean.toString(value);
	}
	
	public boolean value() { return value; }

	/*
	 * Constructor for statics TRUE and FALSE only; no other object
	 * necessary and this improves memory performance. 
	 * */
	private JsonBoolean(boolean v) {
		this.value = v;
	}
	
	public static final JsonBoolean TRUE = new JsonBoolean(true);
	public static final JsonBoolean FALSE = new JsonBoolean(false);

	@SuppressWarnings("unchecked")
	@Override
	public <T extends JsonValue> T cast() {
		return (T) this;
	}

	@Override
	public String print() {
		return this.serialize();
	}

	@Override
	public String print(int indent) {
		return Json.prefix(indent) + this.serialize();
	}
}
