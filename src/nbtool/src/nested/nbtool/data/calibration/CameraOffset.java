package nbtool.data.calibration;

import nbtool.data.SExpr;
import nbtool.data.json.Json;
import nbtool.data.json.JsonArray;
import nbtool.data.json.JsonObject;

public class CameraOffset {
	public boolean camera_top;
	String getCamera() {return camera_top ? "camera_TOP" : "camera_BOT";}
	
	public double d_roll;
	public double d_tilt;
	
	public JsonObject toObject() {
		JsonObject ret = Json.object();
		ret.put("camera_top", camera_top);
		ret.put("d_roll", d_roll);
		ret.put("d_tilt", d_tilt);
		return ret;
	}
	
	private CameraOffset(boolean top, double r, double t) {
		this.camera_top = top; this.d_roll = r; this.d_tilt = t;
	}
	
	public static CameraOffset fromObject(JsonObject o) {
		return new CameraOffset(
				o.get("camera_top").asBoolean().bool(),
				o.get("d_roll").asNumber().asDouble(),
				o.get("d_tilt").asNumber().asDouble()
				);
	}
	
	public static CameraOffset[] fromArray(JsonArray a) {
		CameraOffset[] offset = new CameraOffset[a.size()];
		for (int i = 0; i < a.size(); ++i) {
			offset[i] = fromObject(a.get(i).asObject());
		}
		
		return offset;
	}
	
	public static CameraOffset fromLisp(SExpr lisp) {
		return null;
	}
	
	
}
