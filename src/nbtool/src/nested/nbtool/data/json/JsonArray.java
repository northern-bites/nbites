package nbtool.data.json;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map.Entry;

import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.Json.JsonValueType;

public class JsonArray extends ArrayList<JsonValue> implements JsonValue {
	
	public boolean add(String str) {
		super.add(new JsonString(str));
		return true;
	}
	
	public boolean add(int val) {
		super.add(new JsonNumber(val));
		return true;
	}
	
	public boolean add(double val) {
		super.add(new JsonNumber(val));
		return true;
	}
	
	public boolean add(boolean val) {
		super.add(val ? JsonBoolean.TRUE : JsonBoolean.FALSE);
		return true;
	}
	
	@Override
	public JsonValueType type() {
		return Json.JsonValueType.ARRAY;
	}

	@Override
	public String serialize() {
		Iterator<JsonValue> it = this.iterator();
		StringBuilder builder = new StringBuilder();
		builder.append(JsonParser.TokenType.ARRAY_START.CHARACTER);
		
		while(it.hasNext()) {
			JsonValue entry = it.next();
			builder.append(entry.serialize());
			
			if (it.hasNext())
				builder.append(JsonParser.TokenType.SEPARATOR.CHARACTER);
		}
		
		builder.append(JsonParser.TokenType.ARRAY_END.CHARACTER);
		return builder.toString();		
	}

	@Override
	public String print() {
		return this.print(0);
	}

	@Override
	public String print(int indent) {
		Iterator<JsonValue> it = this.iterator();
		StringBuilder builder = new StringBuilder();
		builder.append(Json.prefix(indent));
		builder.append(JsonParser.TokenType.ARRAY_START.CHARACTER);
		builder.append("\n");
		
		while(it.hasNext()) {
			JsonValue entry = it.next();
			builder.append(entry.print(indent + 1));
			
			if (it.hasNext())
				builder.append(JsonParser.TokenType.SEPARATOR.CHARACTER);
			
			builder.append("\n");
		}
		
		builder.append(Json.prefix(indent));
		builder.append(JsonParser.TokenType.ARRAY_END.CHARACTER);
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
	public JsonValue copy() {
		JsonArray copy = new JsonArray();
		for (JsonValue v : this) {
			copy.add( v == null ? null : v.copy());
		}
		
		return copy;
	}

	@Override
	public boolean congruent(JsonValue other) {
		if (other == null || other.type() != this.type())
			return false;
		
		JsonArray array = other.asArray();
		if (array.size() != this.size())
			return false;
		
		for (int i = 0; i < this.size(); ++i) {
			if (!this.get(i).congruent(array.get(i))) {
				return false;
			}
		}
		
		return true;
	}
}
