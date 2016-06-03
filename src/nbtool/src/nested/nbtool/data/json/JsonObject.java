package nbtool.data.json;

import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map.Entry;

import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.Json.JsonValueType;
import nbtool.util.Debug;
import nbtool.util.Debug.DebugSettings;

public class JsonObject extends LinkedHashMap<JsonString, JsonValue> implements JsonValue {
	
	private static final DebugSettings debug = Debug.createSettings(true, true, true, Debug.INFO, null);

	@Override
	public JsonValueType type() {
		return JsonValueType.OBJECT;
	}
	
	public JsonValue put(String key, JsonValue value) {
		return this.put(new JsonString(key), value);
	}
	
	public JsonValue put(String key, String value) {
		return this.put(new JsonString(key), new JsonString(value));
	}
	
	public JsonValue put(String key, int value) {
		return this.put(new JsonString(key), new JsonNumber(value));
	}
	
	public JsonValue put(String key, boolean value) {
		return this.put(new JsonString(key), JsonBoolean.from(value));
	}
	
	public JsonValue put(String key, long value) {
//		debug.info("{%s, %s}", key, new JsonNumber(value).toString());
		return this.put(new JsonString(key), new JsonNumber(value));
	}
	
	public JsonValue put(String key, double value) {
		return this.put(new JsonString(key), new JsonNumber(value));
	}
	
	public JsonValue get(String key) {
		return super.get(new JsonString(key));
	}
	
	public JsonValue remove(String key) {
		return super.remove(new JsonString(key));
	}
	
	public boolean containsKey(String key) {
		return super.containsKey(new JsonString(key));
	}
	
	@Override
	public JsonValue get(Object key) {
		if (key instanceof String) {
			return super.get(new JsonString( (String) key));
		} else if (key instanceof JsonString) {
			return super.get(key);
		} else {
			throw new IllegalArgumentException("JsonObject keys must be Strings or JsonStrings!");
		}
	}

	@Override
	public String serialize() {
		Iterator<Entry<JsonString, JsonValue>> it = this.entrySet().iterator();
		StringBuilder builder = new StringBuilder();
		builder.append(JsonParser.TokenType.OBJECT_START.CHARACTER);
		
		while(it.hasNext()) {
			Entry<JsonString, JsonValue> entry = it.next();
			debug.event("Entry: %s, %s", entry.getKey().toString(), entry.getValue().toString());
			builder.append(entry.getKey().serialize());
			builder.append(JsonParser.TokenType.OBJECT_DIVIDER.CHARACTER);
			builder.append(entry.getValue().serialize());
			
			if (it.hasNext())
				builder.append(JsonParser.TokenType.SEPARATOR.CHARACTER);
		}
		
		builder.append(JsonParser.TokenType.OBJECT_END.CHARACTER);
		return builder.toString();
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
		return this.print(0);
	}

	@Override
	public String print(int indent) {
		Iterator<Entry<JsonString, JsonValue>> it = this.entrySet().iterator();
		StringBuilder builder = new StringBuilder();
		builder.append(Json.prefix(indent));
		builder.append(JsonParser.TokenType.OBJECT_START.CHARACTER);
		builder.append("\n");
		
		while(it.hasNext()) {
			Entry<JsonString, JsonValue> entry = it.next();
			builder.append(Json.prefix(indent + 1));
			builder.append(entry.getKey().serialize() + " ");
			builder.append(JsonParser.TokenType.OBJECT_DIVIDER.CHARACTER);
			builder.append("\n");
			builder.append(entry.getValue().print(indent + 2));
			
			if (it.hasNext())
				builder.append(JsonParser.TokenType.SEPARATOR.CHARACTER);
			builder.append("\n");
		}
		
		builder.append(Json.prefix(indent));
		builder.append(JsonParser.TokenType.OBJECT_END.CHARACTER);
		return builder.toString();
	}

	@Override
	public JsonValue copy() {
		JsonObject copy = new JsonObject();
		for (Entry<JsonString, JsonValue > v : this.entrySet()) {
			copy.put(v.getKey().asString(), v.getValue().copy());
		}
		
		return copy;
	}

	@Override
	public boolean congruent(JsonValue other) {
		if (other == null || other.type() != this.type()) {
			return false;
		}
		
		JsonObject obj = other.asObject();
		if (obj.size() != this.size())
			return false;
		
		for (Entry<JsonString, JsonValue> entry : this.entrySet()) {
			if (obj.containsKey(entry.getKey())) {
				if (!obj.get(entry.getKey()).congruent(entry.getValue()))
					return false;
			} else {
				return false;
			}
		}
		
		return true;
	}
}
