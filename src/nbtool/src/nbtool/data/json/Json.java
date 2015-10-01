package nbtool.data.json;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import nbtool.data.json.JsonParser.JsonParseException;

public class Json {
	
	public static enum JsonValueType {
		OBJECT(JsonObject.class, LinkedHashMap.class),
		ARRAY(JsonArray.class, ArrayList.class),
		
		STRING(JsonString.class, String.class),
		NUMBER(JsonNumber.class, Number.class),
		
		BOOLEAN(JsonBoolean.class, Boolean.class),
		NULL(Json.NULL_VALUE.getClass(), null);
		
		public Class<?> jsonClass;
		public Class<?> javaClass;
		private JsonValueType(Class<?> json, Class<?> java) {
			this.jsonClass = json;
			this.javaClass = java;
		}
	}
	
	public static interface JsonValue {
		public JsonValueType type();
		
		//JSON text of value, somewhat compact encoding.
		public String serialize();
		
		public String print();
		/*
		 * Indenting rules:
		 * 	a JsonValue implementation is responsible for adding the proper indentation to every line
		 *  of its plain-text rep., including the first and last.
		 *  
		 * an implementation may span multiple lines but should not add a trailing newline.
		 * */
		public String print(int indent);
		
		//This allows you to daisy-chain subclass operations.  Attempts to inline cast an object
		//to an incorrect class will throw a standard java ClassCastException
		public <T extends JsonValue> T cast();
	}
	
	private static final int spacesPerIndent = 2;
	protected static String prefix(int indent) {
		char[] array = new char[indent * spacesPerIndent];
		Arrays.fill(array, ' ');
		return new String(array);
	}
	
	public static JsonValue parse(String text) throws JsonParseException {
		return parse(text, 0);
	}
	
	public static JsonValue parse(String text, int from) throws JsonParseException {
		JsonParser parser = new JsonParser(text, from);
		return parser.parse();
	}
	
	public static JsonParser parser(String text, int start) {
		return new JsonParser(text, start);
	}
	
	private static final class JsonNull implements JsonValue {

		@Override
		public JsonValueType type() {
			return JsonValueType.NULL;
		}

		@Override
		public String serialize() {
			return "null";
		}

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
	
	public static final JsonValue NULL_VALUE = new JsonNull();
	
	public static boolean isNull(JsonValue value) {
		return (value == null || (value instanceof JsonNull));
	}
	
	public static boolean asBool(JsonValue value) {
		switch (value.type()) {
			
		case BOOLEAN:
			return value.<JsonBoolean>cast().value();
		
		case NUMBER:
			return value.<JsonNumber>cast().intValue() > 0;
		
		case STRING:
			String text = value.<JsonString>cast().value();
			return Boolean.valueOf(text);	
			
		default:
		}
		
		throw new ClassCastException("Cannot derive boolean from " + value.toString());
	}
	
	public static JsonObject object() {
		return new JsonObject();
	}
	
	public static JsonObject object(Map<JsonString, JsonValue> map) {
		JsonObject obj = new JsonObject();
		obj.putAll(map);
		return obj;
	}
	
	public static JsonArray array() {
		return new JsonArray();
	}
	
	public static JsonArray array(List<JsonValue> vals) {
		JsonArray array = new JsonArray();
		array.addAll(vals);
		return array;
	}
	
	public static JsonString string() {
		return new JsonString("");
	}
	
	public static JsonString string(String s) {
		return new JsonString(s);
	}
	
	public static JsonNumber num(int v) {
		return new JsonNumber(v);
	}
	
	public static JsonNumber num(long v) {
		return new JsonNumber(v);
	}
	
	public static JsonNumber num(float v) {
		return new JsonNumber(v);
	}
	
	public static JsonNumber num(double v) {
		return new JsonNumber(v);
	}
	
	public static JsonBoolean bool(boolean val) {
		return val ? JsonBoolean.TRUE : JsonBoolean.FALSE;
	}
}
