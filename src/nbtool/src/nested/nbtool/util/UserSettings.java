package nbtool.util;

import java.awt.Rectangle;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.ListIterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Properties;
import java.util.Set;
import java.util.prefs.BackingStoreException;
import java.util.prefs.Preferences;

import nbtool.data.OrderedSet;
import nbtool.data.SExpr;
import nbtool.data.ViewProfile;
import nbtool.data.json.Json;
import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.JsonArray;
import nbtool.data.json.JsonObject;
import nbtool.data.json.JsonParser.JsonParseException;
import nbtool.data.json.JsonString;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.Debug.LogLevel;

public class UserSettings {

	public static final Map<String, DisplaySettings> BOUNDS_MAP = new HashMap<>();
	public static final Map<String, JsonValue> PREFERENCES = new HashMap<>();
	
	public static int leftSplitLoc = -1;

	//saved user inputs..
	public static OrderedSet<Path> loadPathes = new OrderedSet<>();
	/* For list with robot names with and without .local
	public static OrderedSet<String> addresses = new OrderedSet<> 
		(Robots.sortedHostNames());
	*/
	//List of robot names without .local
	public static OrderedSet<String> addresses = new OrderedSet<>
		(Robots.sortedWifiNames());

	public static LogLevel logLevel = LogLevel.levelINFO;
	
	public static String venue = "edwards16";
	
	private static final String INTERNAL_KEY = "__INTERNAL__";
	private static final String BOUNDS_KEY = "__BOUNDS__";
	private static final String ADDR_KEY = "__ADDRESSES__";
	private static final String LOAD_KEY = "__LOADPATHS__";
	private static final String LL_KEY = "__LOGLEVEL__";
	
	public static final String PROFILES_KEY = "__PROFILES__";
	
	public static final String VENUE_KEY = "__VENUE__";
	
	public static class DisplaySettings {
		public Rectangle bounds = null;
		public ViewProfile profile = null;
		public int splitLocation = -1;
		
		public DisplaySettings(Rectangle r, ViewProfile p, int split) {
			bounds = r; profile = p; splitLocation = split;
		}
		
		public JsonObject serialize() {
			JsonObject obj = Json.object();
			if (bounds == null) {
				obj.put("bounds", Json.NULL_VALUE);
			} else {
				JsonArray array = Json.array();
				array.add(bounds.x);
				array.add(bounds.y);
				array.add(bounds.width);
				array.add(bounds.height);
				obj.put("bounds", array);
			}
			
			obj.put("vp", profile == null ?
					Json.NULL_VALUE : new JsonString(profile.name));
			
			obj.put("split", splitLocation);
			
			return obj;
		}
		
		public DisplaySettings(JsonValue _obj) {
			JsonObject obj = _obj.asObject();
			if (obj.containsKey("bounds") && !Json.isNull(obj.get("bounds"))) {
				JsonArray array = obj.get("bounds").asArray();
				bounds = new Rectangle(
						array.get(0).asNumber().intValue(),
						array.get(1).asNumber().intValue(),
						array.get(2).asNumber().intValue(),
						array.get(3).asNumber().intValue()
						);
			} else {
				bounds = ToolSettings.DEFAULT_BOUNDS;
			}
			
			if (obj.containsKey("vp") && !Json.isNull(obj.get("vp"))) { 
				JsonString vp = obj.get("vp").asString();
				profile = ViewProfile.PROFILES.get(vp.value);
			}
			
			if (profile == null)
				profile = ViewProfile.DEFAULT_PROFILE;
			
			if (obj.containsKey("split")) {
				splitLocation = obj.get("split").asNumber().asInt();
			}
		}
		
		@Override
		public String toString() {
			return String.format("DisplaySettings{%s}{%s}", bounds, profile);
		}
	}

	/* SAVE PREFERENCES TO FILESYSTEM */
	public static void savePreferences() throws IOException {
		Debug.info("saving preferences to %s", ToolSettings.USER_PREFERENCES_PATH);
		
		JsonObject object = Json.object();
		object.put("#NBTOOL_USER_PREFS", Json.NULL_VALUE);
		object.put(SharedConstants.LOG_TOPLEVEL_MAGIC_KEY(), ToolSettings.VERSION);
		
		JsonObject internal = Json.object();
		
		JsonObject bounds = Json.object();
		JsonArray addr = Json.array();
		JsonArray load = Json.array();
		
		internal.put(BOUNDS_KEY, bounds);
		
		for (Entry<String, DisplaySettings> entry : BOUNDS_MAP.entrySet()) {
			bounds.put(entry.getKey(), entry.getValue().serialize());
		}
		
		internal.put(ADDR_KEY, addr);
		
		for (String a : addresses.vector()) {
			addr.add(a);
		}
		
		internal.put(LOAD_KEY, load);
		
		for (Path l : loadPathes.vector()) {
			load.add(l.toString());
		}
		
		internal.put(LL_KEY, logLevel.name());
		internal.put(VENUE_KEY, venue);
		
		internal.put(PROFILES_KEY, ViewProfile.serializeProfiles());
		
		object.put(INTERNAL_KEY, internal);
		
		for (Entry<String, JsonValue> entry : PREFERENCES.entrySet()) {
			object.put(entry.getKey(), entry.getValue());
		}
		
		byte[] bytes = object.print().getBytes(StandardCharsets.UTF_8);
		Files.write(ToolSettings.USER_PREFERENCES_PATH, bytes);
	}

	/* LOAD PREFERENCES FROM FILESYSTEM */
	public static void loadPreferences() throws IOException, ClassNotFoundException, JsonParseException {
		Debug.print("fetching user preferences from: " + ToolSettings.USER_PREFERENCES_PATH);
		
		if (!Files.exists(ToolSettings.USER_PREFERENCES_PATH)) {
			Debug.error("preferences file not found!");
			ViewProfile.setupProfiles(null);
			
			return;
		}
		
		String prefText = new String(Files.readAllBytes(ToolSettings.USER_PREFERENCES_PATH),
				StandardCharsets.UTF_8);
		prefText = prefText.trim();
		if (prefText.startsWith("(")) {
			Debug.error("not using old SExpr user preferences file (it will be overwritten on exit!)");
			ViewProfile.setupProfiles(null);
			
			return;
		}
		
		JsonObject all = Json.parse(prefText).asObject();
		if (!all.containsKey("#NBTOOL_USER_PREFS") ||
				!(all.get(SharedConstants.LOG_TOPLEVEL_MAGIC_KEY()).asNumber().intValue() ==
				ToolSettings.VERSION)) {
			Debug.error("cannot use saved preferences at: %s", ToolSettings.USER_PREFERENCES_PATH);
			Debug.error("using default values.");
			
			ViewProfile.setupProfiles(null);
			return;
		}
		
		Debug.print("preferences format accepted.");
		
		JsonObject internal = all.remove(INTERNAL_KEY).asObject();
		JsonObject bounds = internal.remove(BOUNDS_KEY).asObject();
		JsonArray addr = internal.remove(ADDR_KEY).asArray();
		JsonArray load = internal.remove(LOAD_KEY).asArray();
		
		if (internal.containsKey(VENUE_KEY)) {
			JsonString _venue = internal.remove(VENUE_KEY).asString();
			venue = _venue.value;
		}
		
		for (Entry<JsonString, JsonValue> entry : bounds.entrySet()) {
			BOUNDS_MAP.put(entry.getKey().value, new DisplaySettings(entry.getValue()));
		}
		
		for (ListIterator<JsonValue> i = addr.listIterator(addr.size());
				i.hasPrevious(); ) {
			JsonValue val = i.previous();
			addresses.update(val.asString().value);
		}
		
		for (ListIterator<JsonValue> i = load.listIterator(load.size());
				i.hasPrevious(); ) {
			JsonValue val = i.previous();
			loadPathes.update(Paths.get(val.asString().value));
		}
		
		logLevel = Debug.LogLevel.valueOf(internal.get(LL_KEY).asString().value);
		
		if (internal.get(PROFILES_KEY).type() == Json.JsonValueType.STRING) {
			Debug.warn("ha! you used tool8 before it was stable... sorry, your profiles do not transfer.");
			ViewProfile.setupProfiles(null);
		} else {
			ViewProfile.setupProfiles(internal.get(PROFILES_KEY).asObject());
		}
		
		for (Entry<JsonString, JsonValue> entry : all.entrySet()) {
			PREFERENCES.put(entry.getKey().value, entry.getValue());
		}
	}
}
