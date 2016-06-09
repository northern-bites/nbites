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
	public boolean bool() {return value;}

	/*
	 * Constructor for statics TRUE and FALSE only; no other object
	 * necessary and this improves memory performance. 
	 * */
	private JsonBoolean(boolean v) {
		this.value = v;
	}
	
	public static final JsonBoolean TRUE = new JsonBoolean(true);
	public static final JsonBoolean FALSE = new JsonBoolean(false);
	
	public static JsonBoolean from(boolean val) {
		return val ? TRUE : FALSE;
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
	public String print() {
		return this.serialize();
	}

	@Override
	public String print(int indent) {
		return Json.prefix(indent) + this.serialize();
	}
	
	@Override
	public JsonValue copy() {
		return new JsonBoolean(value);
	}

	@Override
	public boolean congruent(JsonValue other) {
		if (other == null || other.type() != this.type())
			return false;
		
		return other.asBoolean().value == this.value;
	}
}
