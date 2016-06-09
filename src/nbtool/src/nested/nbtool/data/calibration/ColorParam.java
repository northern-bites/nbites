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
import nbtool.data.json.JsonObject;
import nbtool.data.json.JsonString;
import nbtool.data.json.Json.JsonValue;
import nbtool.util.Debug;
import nbtool.util.SharedConstants;
import nbtool.util.ToolSettings;
import nbtool.util.test.TestBase;
import nbtool.util.test.Tests;

public class ColorParam {
	public String name;
	
	public double uAtY0, uAtY255;
	public double vAtY0, vAtY255;
	public double u_fuzzy_range, v_fuzzy_range;
	
	public JsonObject serialize() {
		JsonObject ret = Json.object();
		ret.put("uAtY0", uAtY0);
		ret.put("uAtY255", uAtY255);
		ret.put("vAtY0", vAtY0);
		ret.put("vAtY255", vAtY255);
		ret.put("u_fuzzy_range", u_fuzzy_range);
		ret.put("v_fuzzy_range", v_fuzzy_range);
		return ret;
	}

	public static ColorParam parse(JsonObject co, String name) {
		ColorParam ret = new ColorParam();
		ret.name = name;
		ret.uAtY0 = co.get("uAtY0").asNumber().asDouble();
		ret.uAtY255 = co.get("uAtY255").asNumber().asDouble();
		ret.vAtY0 = co.get("vAtY0").asNumber().asDouble();
		ret.vAtY255 = co.get("vAtY255").asNumber().asDouble();
		ret.u_fuzzy_range = co.get("u_fuzzy_range").asNumber().asDouble();
		ret.v_fuzzy_range = co.get("v_fuzzy_range").asNumber().asDouble();
		
		return ret;
	}
	
	public static class Camera {
		public ColorParam green;
		public ColorParam white;
		public ColorParam black;
		
		public static Camera parse(JsonObject params) {
			Camera camera = new Camera();
			camera.green = ColorParam.parse(params.get("green").asObject(), "green");
			camera.white = ColorParam.parse(params.get("white").asObject(), "white");
			camera.black = ColorParam.parse(params.get("black").asObject(), "black");
			return camera;
		}
		
		public JsonObject serialize() {
			JsonObject ret = Json.object();
			ret.put("green", green.serialize());
			ret.put("white", white.serialize());
			ret.put("black", black.serialize());
			return ret;
		}
	}
	
	public static class Set {
		public Camera[] cameras = new Camera[2];
		
		public Camera getTop() {return cameras[0];}
		public Camera getBot() {return cameras[1];}
		
		public static Set parse(JsonObject params) {
			Set ret = new Set();
			ret.cameras[0] = Camera.parse( params.get("camera_TOP").asObject() );
			ret.cameras[1] = Camera.parse( params.get("camera_BOT").asObject() );
			return ret;
		}
		
		public JsonObject serialize() {
			JsonObject ret = Json.object();
			
			ret.put("camera_TOP", cameras[0].serialize());
			ret.put("camera_BOT", cameras[1].serialize());
			return ret;
		}
	}
	
	public static Path getPath() {
		return Paths.get(ToolSettings.NBITES_DIR, SharedConstants.OFFLINE_COLOR_PARAMS_SUFFIX());
	}
	
	public static void _NBL_ADD_TESTS_() {
		Tests.add("calibration", new TestBase("ColorParam"){

			@Override
			public boolean testBody() throws Exception {
				Set set = new Set();
				
				ColorParam green = new ColorParam();
				green.uAtY0 = 1; green.uAtY255 = 2;
				green.vAtY0 = 3; green.vAtY255 = 4;
				green.u_fuzzy_range = 5; green.v_fuzzy_range = 6;
				
				ColorParam white = new ColorParam();
				white.uAtY0 = 7; white.uAtY255 = 9;
				white.vAtY0 = 8; white.vAtY255 = 10;
				white.u_fuzzy_range = 11; white.v_fuzzy_range = 12;
				
				ColorParam black = new ColorParam();
				black.uAtY0 = 13; black.uAtY255 = 15;
				black.vAtY0 = 14; black.vAtY255 = 16;
				black.u_fuzzy_range = 18; black.v_fuzzy_range = 17;
				
				set.cameras[0] = new Camera();
				set.cameras[0].green = green;
				set.cameras[0].black = black;
				set.cameras[0].white = white;
				
				set.cameras[1] = new Camera();
				set.cameras[1].green = green;
				set.cameras[1].black = black;
				set.cameras[1].white = white;
				
				assert(set.serialize().congruent(
						Set.parse( set.serialize().asObject()).serialize() )
						);
				
				return true;
			}
			
		}, new TestBase("ColorParamsExists") {

			@Override
			public boolean testBody() throws Exception {
				
				String cpStr = new String(Files.readAllBytes(
						Paths.get(ToolSettings.NBITES_DIR, SharedConstants.OFFLINE_COLOR_PARAMS_SUFFIX())
						));
				
				Set.parse(Json.parse(cpStr).asObject());
				
				return true;
			}
			
		});
	}
	
	private static ColorParam makeC(SExpr se) {
		Debug.plain(se.print());
		ColorParam param = new ColorParam();
		int i = 0;
		param.uAtY0 = se.get(i++).get(1).valueAsDouble();
		param.vAtY0 = se.get(i++).get(1).valueAsDouble();
		param.uAtY255 = se.get(i++).get(1).valueAsDouble();
		param.vAtY255 = se.get(i++).get(1).valueAsDouble();
		param.u_fuzzy_range = se.get(i++).get(1).valueAsDouble();
		param.v_fuzzy_range = se.get(i++).get(1).valueAsDouble();
		return param;
	}
	
	private static Camera make(SExpr se) {
		Camera camera = new Camera();
		camera.green = makeC(se.find("Green").get(1));
		camera.black = makeC(se.find("Orange").get(1));
		camera.white = makeC(se.find("White").get(1));
		return camera;
	}
	
	public static void main(String[] args) throws IOException {
		String lisp = new String(Files.readAllBytes(ToolSettings.NBITES_DIR_PATH.resolve("src/man/config/colorParams.txt")));
		SExpr se = SExpr.deserializeFrom(lisp);
		
		SExpr list = se.get(1);
		SExpr top = list.get(0).get(1);
		SExpr bot = list.get(1).get(1);
		
		Set set = new Set();
		set.cameras[0] = make(top);
		set.cameras[1] = make(bot);
		
		Debug.plain("%s", set.serialize().print());
	}
}
