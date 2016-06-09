package nbtool.data.json;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import nbtool.data.json.JsonParser.JsonParseException;
import nbtool.util.test.TestBase;
import nbtool.util.test.Tests;

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
		
		public JsonArray asArray();
		public JsonBoolean asBoolean();
		public JsonNumber asNumber();
		public JsonObject asObject();
		
		public JsonString asString();
		
		public JsonValue copy();
		
		public boolean congruent(JsonValue other);
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
	
	public static JsonValue parseAndRequireEnd(String text) throws JsonParseException {
		JsonParser parser = new JsonParser(text, 0);
		JsonValue ret = parser.parse();
		if (parser.position() != text.length()) 
			throw new JsonParseException("could not parse entire text", parser.position(), text);
		else return ret;
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

		@Override
		public JsonValue copy() {
			return NULL_VALUE;
		}

		@Override
		public boolean congruent(JsonValue other) {
			return other != null && other.type() == this.type();
		}
		
		/* end mirrored section
		 * ******************************/
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
	
	public static JsonArray array(JsonValue ... vals) {
		return Json.array(Arrays.asList(vals));
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
	
	public static JsonNull nullv() {
		return (JsonNull) NULL_VALUE;
	}
	
	public static void _NBL_ADD_TESTS_() {
		Tests.add("json", new TestBase("encode") {
			
			@Override
			public boolean testBody() throws Exception {
				JsonArray witho = new JsonArray();
				JsonArray with = new JsonArray();
				witho.add(5.0);
				witho.add(100);
				witho.add("string");
				
				with.add(new JsonNumber(5.0));
				with.add(new JsonNumber(100));
				with.add(new JsonString("string"));
				
				requireEqual(witho.serialize(), with.serialize());
				
				JsonArray array1 = new JsonArray();
				array1.add(new JsonString("abc"));
				array1.add(new JsonNumber(123));
				array1.add(new JsonNumber(222.123));
				array1.add(JsonBoolean.TRUE);
				requireEqual(array1.serialize(), "[\"abc\",123,222.123,true]");
				
				JsonObject obj1 = new JsonObject();
				obj1.put("array1", array1);
				requireEqual("{\"array1\":[\"abc\",123,222.123,true]}", obj1.serialize());
				
				obj1.remove("array1");
				assert(obj1.size() == 0);
				array1.add(obj1);
				requireEqual("[\"abc\",123,222.123,true,{}]", array1.serialize());
				
				JsonObject map = new JsonObject();
				map.put("k31", Json.string("v3"));
				map.put("k32", Json.num(123.45));
				map.put("k33", Json.bool(false));
				map.put("k34", Json.NULL_VALUE);
				
				List<JsonValue> l = new LinkedList<>();
				l.add(Json.string("vvv"));
				l.add(Json.num(1.23456789123456789));
				l.add(Json.bool(true));
				l.add(Json.NULL_VALUE);
				map.put("list", Json.array(l));
				
				String correct = "{\"k31\":\"v3\",\"k32\":123.45,\"k33\":false,\"k34\":null,\"list\":[\"vvv\",1.234567891234568,true,null]}";
				String first = map.serialize();
				requireEqual(correct, first);
				
				requireEqual(Json.parse(correct).serialize(), first);
				
				return true;
			}
			
		});
		
		Tests.add("json", new TestBase("decode"){

			@Override
			public boolean testBody() throws Exception {
				//Good strings
				try {
					String s="[0,{\"1\":{\"2\":{\"3\":{\"4\":[5,{\"6\":7}]}}}}]";
					JsonArray obj = Json.parse(s).<JsonArray>cast();
					requireEqual("{\"1\":{\"2\":{\"3\":{\"4\":[5,{\"6\":7}]}}}}", obj.get(1).serialize());
					
					JsonObject obj2 = obj.get(1).<JsonObject>cast();
					//System.out.println(obj2.print());
					requireEqual("{\"2\":{\"3\":{\"4\":[5,{\"6\":7}]}}}", obj2.get("1").serialize());
					
					s="{}";
					JsonValue obj3 = Json.parse(s);
					requireEqual("{}",obj3.serialize());
					
					
					s="[\"hello\\bworld\\\"abc\\tdef\\\\ghi\\rjkl\\n123\\\"\"]";
					//Logger.println(s);
					JsonValue obj4 = Json.parse(s);
					//Logger.println(obj4.serialize());

					requireEqual("\"hello\\bworld\\\"abc\\tdef\\\\ghi\\rjkl\\n123\\\"\"",
							obj4.<JsonArray>cast().get(0).serialize());
					
					//Logger.println("after");
					
					s= "45.0";
					JsonValue obj5 = Json.parse(s);
					JsonNumber num = obj5.<JsonNumber>cast();			
					assert(num.doubleValue() == 45.0);
					
					s="0x43";
					JsonValue obj6 = Json.parse(s);
					JsonNumber num2 = obj6.<JsonNumber>cast();
					assert(num2.intValue() == Integer.decode(s));
					
					s="{\"first\": 123, \"second\": [4, 5, 6], \"third\": 789}";
					JsonObject obj7 = Json.parse(s).<JsonObject>cast();
					assert(obj7.get("first").<JsonNumber>cast().intValue() == 123
							);
					assert(obj7.get("second").<JsonArray>cast().size() == 3);
					assert(obj7.get("third").<JsonNumber>cast().intValue() == 789);
					
				} catch (JsonParseException e) {
					e.printStackTrace();
					return false;
				}
						
				//requireInvalid("name");	//valid string, test of requireInvalid()
				
				requireInvalid("[5,]");
				requireInvalid("[5,,2]");
				requireInvalid("{\"name\":");
				requireInvalid("{\"name\":}");
				requireInvalid("{\"name");
				requireInvalid("[[null, 123.45, \"a\\b c\"}, true]");
				
				
				return true;
			}
			
			private void requireInvalid(String s) throws Exception {
				boolean thrown = false; 
				try {
					Json.parse(s);
				} catch (JsonParseException jpe) {
					thrown = true;
				} finally {
					if (!thrown) {
						failed(String.format("parser did not throw on invalid string '%s' ", s));
					}
				}
			}	
		} , new TestBase("congruence") {

			@Override
			public boolean testBody() throws Exception {
				
				JsonNumber num = Json.num(100);
				JsonNumber num1 = Json.num(100);
				JsonNumber num2 = Json.num(200);
				JsonNumber num3 = Json.num(300);
				
				assert(num.congruent(num1));
				assert(!num.congruent(num3));
				
				JsonBoolean b1 = Json.bool(true);
				JsonBoolean b2 = Json.bool(false);
				JsonBoolean b3 = Json.bool(true);
				
				assert(b1.congruent(b3));
				assert(!b2.congruent(b3));
				
				JsonArray a1 = Json.array(num, b1, b2);
				JsonArray a2 = Json.array(Json.num(100), b1, b2);
				JsonArray a3 = Json.array(num, b1, b2);
				JsonArray a4 = Json.array();
				JsonArray a5 = Json.array(num, b2, b1);
				
				assert(a1.congruent(a2));
				assert(a1.congruent(a3));
				assert(!a1.congruent(a4));
				assert(!a1.congruent(a5));
				
				JsonString s1 = Json.string("thing");
				JsonString s2 = Json.string("thinG");
				JsonString s3 = Json.string("thing");
				
				assert(s1.congruent(s1));
				assert(s1.congruent(s3));
				assert(!s1.congruent(s2));
				
				JsonObject o1 = Json.object();
				o1.put("a", a1);
				JsonObject o2 = Json.object();
				o2.put("a", a3);
				
				assert(o1.congruent(o2));
				JsonObject o3 = Json.object();
				assert(!o1.congruent(o3));
				
				return true;
			}
			
		}
		);
		
	}	
	
}
