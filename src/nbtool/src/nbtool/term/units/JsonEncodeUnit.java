package nbtool.term.units;

import java.util.LinkedList;
import java.util.List;

import nbtool.data.json.Json;
import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.JsonArray;
import nbtool.data.json.JsonBoolean;
import nbtool.data.json.JsonNumber;
import nbtool.data.json.JsonObject;
import nbtool.data.json.JsonParser.JsonParseException;
import nbtool.data.json.JsonString;
import nbtool.util.Logger;

public class JsonEncodeUnit extends UnitParent {

	@Override
	public boolean test(String absPathToResources) throws UnitFailedException {
		
		JsonArray witho = new JsonArray();
		JsonArray with = new JsonArray();
		witho.add(5.0);
		witho.add(100);
		witho.add("string");
		
		with.add(new JsonNumber(5.0));
		with.add(new JsonNumber(100));
		with.add(new JsonString("string"));
		
		requireEqual(witho.serialize(), with.serialize(), "JsonArray add overloads failed");
		
		JsonArray array1 = new JsonArray();
		array1.add(new JsonString("abc"));
		array1.add(new JsonNumber(123));
		array1.add(new JsonNumber(222.123));
		array1.add(JsonBoolean.TRUE);
		requireEqual(array1.serialize(), "[\"abc\",123,222.123,true]", 
				"JsonArray unexpected serialization");
		
		JsonObject obj1 = new JsonObject();
		obj1.put("array1", array1);
		requireEqual("{\"array1\":[\"abc\",123,222.123,true]}", obj1.serialize(),
				"JsonObject unexpected serializaiton");
		
		obj1.remove("array1");
		require(obj1.size() == 0, "JsonObject did not remove value");
		array1.add(obj1);
		requireEqual("[\"abc\",123,222.123,true,{}]", array1.serialize(),
				"JsonArray unexpected serialization with object entry");
		
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
		requireEqual(correct, first, "unexpected compound serialization");
		
		try {
			requireEqual(Json.parse(correct).serialize(), first, "unexpected compound parse");
		} catch (JsonParseException e) {
			e.printStackTrace();
			throw new UnitFailedException("could not parse valid string");
		}
		
		return true;
	}

	@Override
	public String toString() {
		return "Json Encoding Unit";
	}

}
