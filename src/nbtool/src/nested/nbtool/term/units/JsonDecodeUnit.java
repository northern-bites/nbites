package nbtool.term.units;

import nbtool.data.json.Json;
import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.JsonArray;
import nbtool.data.json.JsonNumber;
import nbtool.data.json.JsonObject;
import nbtool.data.json.JsonParser.JsonParseException;
import nbtool.util.Debug;

public class JsonDecodeUnit extends UnitParent {

	private final String STD_DECODE_REASON = "serialized component had unexpected form";
	@Override
	public boolean test(String absPathToResources) throws UnitFailedException {
		
		//Good strings
		try {
			String s="[0,{\"1\":{\"2\":{\"3\":{\"4\":[5,{\"6\":7}]}}}}]";
			JsonArray obj = Json.parse(s).<JsonArray>cast();
			requireEqual("{\"1\":{\"2\":{\"3\":{\"4\":[5,{\"6\":7}]}}}}", obj.get(1).serialize(),
					STD_DECODE_REASON);
			
			JsonObject obj2 = obj.get(1).<JsonObject>cast();
			//System.out.println(obj2.print());
			requireEqual("{\"2\":{\"3\":{\"4\":[5,{\"6\":7}]}}}", obj2.get("1").serialize(),
					STD_DECODE_REASON);
			
			s="{}";
			JsonValue obj3 = Json.parse(s);
			requireEqual("{}",obj3.serialize(), STD_DECODE_REASON);
			
			
			s="[\"hello\\bworld\\\"abc\\tdef\\\\ghi\\rjkl\\n123\\\"\"]";
			//Logger.println(s);
			JsonValue obj4 = Json.parse(s);
			//Logger.println(obj4.serialize());

			requireEqual("\"hello\\bworld\\\"abc\\tdef\\\\ghi\\rjkl\\n123\\\"\"", obj4.<JsonArray>cast().get(0).serialize(),
					"error decoding special string characters");
			
			//Logger.println("after");
			
			s= "45.0";
			JsonValue obj5 = Json.parse(s);
			JsonNumber num = obj5.<JsonNumber>cast();			
			require(num.doubleValue() == 45.0, "error parsing value as double");
			
			s="0x43";
			JsonValue obj6 = Json.parse(s);
			JsonNumber num2 = obj6.<JsonNumber>cast();
			require(num2.intValue() == Integer.decode(s), "error parsing value as int");
			
			s="{\"first\": 123, \"second\": [4, 5, 6], \"third\": 789}";
			JsonObject obj7 = Json.parse(s).<JsonObject>cast();
			require(obj7.get("first").<JsonNumber>cast().intValue() == 123,
					"error parsing int as key value");
			require(obj7.get("second").<JsonArray>cast().size() == 3,
					"error parsing array as key value");
			require(obj7.get("third").<JsonNumber>cast().intValue() == 789,
					"error parsing int as key value");
			
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
	
	private void requireInvalid(String s) throws UnitFailedException {
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

	@Override
	public String toString() {
		return "Json Decoding Unit";
	}

}
