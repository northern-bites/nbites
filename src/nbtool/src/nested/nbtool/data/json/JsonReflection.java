package nbtool.data.json;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import nbtool.data.json.Json.JsonValue;
import nbtool.util.Debug;

public class JsonReflection {
	
	public static JsonObject serialize(Object obj, String ... excludeNames) {
		
		Set<String> exclude = new HashSet<>(Arrays.asList(excludeNames));
		
		if (obj instanceof JsonValue) {
			throw new RuntimeException(
					String.format("you use JsonReflection.serialize() on non-Json objects, not %s",
					obj));
		}
		
		JsonObject ret = Json.object();
		for (Field field : obj.getClass().getDeclaredFields()) {
			int mod = field.getModifiers();
			String name = field.getName();
			if (Modifier.isPublic(mod) && !exclude.contains(name)) {
				SerHandler handler = serializeHandlers.get(field.getType());
				if (handler != null) {
					ret.put(name, handler.handle(field));
				} else {
					Debug.warn("cannot serialize field of type %s in object %s!", 
							field.getType(), obj);
				}
			}
		}
		
		return ret;
	}
	
	private static interface SerHandler {
		JsonValue handle(Field field);
	}
	
	private static interface DesHandler {
		void handle(String name, Object to, JsonValue val);
	}

	private static Map<Class<?>, SerHandler> serializeHandlers;
	private static Map<Class<? extends JsonValue>, DesHandler> deserialzeHandlers;
	
	static {
		serializeHandlers = new HashMap<>();
		
	}
	
	static class Test {
		static int field = 5;
	}
	
	public static void main(String[] args) throws IllegalArgumentException, IllegalAccessException {
		for (Field f : Test.class.getDeclaredFields()) {
			Debug.print("%s %s", f.getType(), f.get(null));
		}		
	}
}
