package nbtool.data.calibration;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import nbtool.data.SExpr;
import nbtool.data.json.Json;
import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.JsonArray;
import nbtool.data.json.JsonObject;
import nbtool.data.json.JsonString;
import nbtool.util.Debug;
import nbtool.util.SharedConstants;
import nbtool.util.ToolSettings;
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
	
	public CameraOffset(double r, double t) {
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
		public CameraOffset top;
		public CameraOffset bot;
		
		public Pair(CameraOffset t, CameraOffset b) {
			this.top = t; this.bot = b;
		}
	}
	
	public static class Set extends HashMap<String, Pair> {
		
		public static Set parse(JsonObject obj) {
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
	
	public static Path getPath() {
		return Paths.get(ToolSettings.NBITES_DIR, SharedConstants.OFFLINE_CAMERA_OFFSET_SUFFIX());
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
				
				JsonObject backwards = Set.parse(top).serialize();
				assert(backwards.congruent(top));
				
				return true;
			}
		},
			new TestBase("CameraOffsetExists") {

					@Override
					public boolean testBody() throws Exception {
						Debug.print("%s", ToolSettings.NBITES_DIR_PATH);
						String cpStr = new String(Files.readAllBytes(
								Paths.get(ToolSettings.NBITES_DIR, SharedConstants.OFFLINE_CAMERA_OFFSET_SUFFIX())
								));
						
						Set.parse(Json.parse(cpStr).asObject());
						return true;
					}
			
		});
	}
	
	public static void main(String[] args) throws IOException {
		String lisp = new String(Files.readAllBytes(ToolSettings.NBITES_DIR_PATH.resolve("src/man/config/calibrationParams.txt")));
		SExpr se = SExpr.deserializeFrom(lisp);
		
		CameraOffset.Set set = new Set();
		SExpr list = se.get(1);
		for (int i = 0; i < list.count(); ++i) {
			String robot = list.get(i).get(0).value();
			CameraOffset top = new CameraOffset(list.get(i).get(1).get(1).valueAsDouble(),
							list.get(i).get(1).get(2).valueAsDouble()
					);
			
			CameraOffset bot = new CameraOffset(list.get(i).get(2).get(1).valueAsDouble(),
					list.get(i).get(2).get(2).valueAsDouble()					
			);
			
			set.put(robot, new Pair(top,bot));
		}
		
		Debug.print("%s", set.serialize().print());
	}
}
