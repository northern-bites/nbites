package nbtool.data.calibration;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import nbtool.data.SExpr;
import nbtool.data.json.Json;
import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.JsonArray;
import nbtool.data.json.JsonObject;
import nbtool.data.json.JsonString;
import nbtool.util.test.TestBase;
import nbtool.util.test.Tests;

public class CameraOffset {	
	public double d_roll;
	public double d_tilt;
	
	public JsonObject toObject() {
		JsonObject ret = Json.object();
		ret.put("d_roll", d_roll);
		ret.put("d_tilt", d_tilt);
		return ret;
	}
	
	private CameraOffset(double r, double t) {
		this.d_roll = r; this.d_tilt = t;
	}
	
	public static CameraOffset fromObject(JsonObject o) {
		return new CameraOffset(
				o.get("d_roll").asNumber().asDouble(),
				o.get("d_tilt").asNumber().asDouble()
				);
	}
	
	public static CameraOffset fromLisp(SExpr lisp) {
		return new CameraOffset(lisp.get(1).valueAsDouble(), lisp.get(2).valueAsDouble());
	}
	
	public static class Pair {
		CameraOffset top;
		CameraOffset bot;
		
		public Pair(CameraOffset t, CameraOffset b) {
			this.top = t; this.bot = b;
		}
	}
	
	public static class Set extends HashMap<String, Pair> {
		
		public static Set parseFrom(JsonObject obj) {
			Set ret = new Set();
			
			for (Entry<JsonString, JsonValue> entry : obj.entrySet()) {
				String robotName = entry.getKey().value;
				JsonObject topObj = entry.getValue().asObject().get("camera_TOP").asObject();
				JsonObject botObj = entry.getValue().asObject().get("camera_BOT").asObject();
				
				Pair offs = new Pair(
						CameraOffset.fromObject(topObj),
						CameraOffset.fromObject(botObj)
						);
				ret.put(robotName, offs);
			}
			
			return ret;
		}
		
		public JsonObject serialize() {
			JsonObject ret = Json.object();
			
			for (Entry<String, Pair> entry : this.entrySet()) {
				JsonObject pair = Json.object();
				
				pair.put("camera_TOP", entry.getValue().top.toObject());
				pair.put("camera_BOT", entry.getValue().bot.toObject());
				
				ret.put(entry.getKey(), pair);
			}
			
			return ret;
		}
	}
	
	public static void _NBL_ADD_TESTS_() {
		Tests.add("calibration", new TestBase("CameraOffset") {
			@Override
			public boolean testBody() throws Exception {
				JsonObject top = Json.object();
				JsonObject vera = Json.object();
				vera.put("camera_TOP", new CameraOffset(1,2).toObject());
				vera.put("camera_BOT", new CameraOffset(3,4).toObject());
				
				top.put("vera", vera);
				
				JsonObject mal = Json.object();
				mal.put("camera_TOP", new CameraOffset(5,8).toObject());
				mal.put("camera_BOT", new CameraOffset(9,7).toObject());
				
				top.put("mal", mal);
				
				JsonObject backwards = Set.parseFrom(top).serialize();
				assert(backwards.congruent(top));
				
				return true;
			}
		});
	}
}
