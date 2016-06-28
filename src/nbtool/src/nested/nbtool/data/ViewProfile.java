package nbtool.data;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import nbtool.data.json.Json;
import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.JsonArray;
import nbtool.data.json.JsonObject;
import nbtool.data.json.JsonString;
import nbtool.data.log.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.ClassFinder;
import nbtool.util.Debug;
import nbtool.util.ToolSettings;

public class ViewProfile {

	/**********  ViewProfile instance methods & constructors*/

	public String name = null;
	public ViewState[][] states = null;

	public ViewProfile(String n) {
		assert(n != null && !n.isEmpty());

		this.name = n;
		this.states = new ViewState[TYPES.length][];
	}

	@SuppressWarnings("unchecked")
	public Class<? extends ViewParent>[] selected(String type) {
		int tindex = Arrays.asList(TYPES).indexOf(type);

		ArrayList<Class<? extends ViewParent>> sel = new ArrayList<>();
		for (ViewState vs : states[tindex]) {
			if (vs.showing)
				sel.add(vs.viewClass);
		}

		return sel.toArray(new Class[sel.size()]);
	}

	@SuppressWarnings("unchecked")
	public Class<? extends ViewParent>[] viewsForLog(Log log) {
		ArrayList<Class<? extends ViewParent>> views = new ArrayList<Class<? extends ViewParent>>();
		String ptype = log.logClass;
		assert(ptype != null);

		int tindex = Arrays.asList(TYPES).indexOf(ptype);
		if (tindex >= 0) {
			for (ViewState vs : states[tindex]) {
				if (vs.showing)
					views.add(vs.viewClass);
			}
		}
//
//		if (ptype.startsWith("proto-")) {
//			views.addAll(Arrays.asList(this.selected(ToolSettings.PROTOBUF_S)));
//		}

		views.addAll(Arrays.asList(this.selected(ToolSettings.DEFAULT_S)));

		Debug.info("LogToViewUtility found %d views for log of type %s.", views.size(), ptype);
		return views.toArray(new Class[views.size()]);
	}

	@Override
	public String toString() {
		return name;
	}

	public static class ViewState {
		public boolean showing;
		public Class<? extends ViewParent> viewClass;

		ViewState(boolean s, Class<? extends ViewParent> cls) {
			showing = s;
			viewClass = cls;
		}

		@Override
		public String toString() {
			return viewClass.getName();
		}
	}

	/**********  ViewProfile static methods & setup*/

	public static final String DEFAULT_PROFILE_NAME = "DEFAULT";
	public static ViewProfile DEFAULT_PROFILE = null;

	public static final Map<String, ViewProfile> PROFILES = new HashMap<>();

	private static ViewProfile makeDefaultProfile() {
		ViewProfile def = new ViewProfile(DEFAULT_PROFILE_NAME);

		for (int j = 0; j < TYPES.length; ++j) {
			Class<? extends ViewParent>[] possible =
					POSSIBLE_VIEWS.get(TYPES[j]);

			//Select all.
			def.states[j] = resolve(possible, null);
		}

		return def;
	}

	public static ViewProfile addWithName(String name) {
		assert(name != null && !name.isEmpty());

		if (name.equalsIgnoreCase(DEFAULT_PROFILE_NAME)) {
			Debug.error("cannot overwrite default profile with {%s}", name);
			return null;
		}

		ViewProfile nvp = new ViewProfile(name);

		for (int j = 0; j < TYPES.length; ++j) {
			Class<? extends ViewParent>[] possible =
					POSSIBLE_VIEWS.get(TYPES[j]);

			//Select all.
			nvp.states[j] = resolve(possible, null);
		}

		PROFILES.put(name, nvp);
		return nvp;
	}

	private static void _setup_(JsonObject profiles) {
		for (Entry<JsonString, JsonValue> entry : profiles.entrySet()) {
			String name = entry.getKey().value;
			JsonObject viewMap = entry.getValue().asObject();
			ViewProfile vp = new ViewProfile(name);

			for (int j = 0; j < TYPES.length; ++j) {
				Class<? extends ViewParent>[] possible =
						POSSIBLE_VIEWS.get(TYPES[j]);

				JsonValue array = viewMap.get(TYPES[j]);
				if (array != null) {
					Class<? extends ViewParent>[] lshown = classValuesFrom(array.asArray());
					vp.states[j] = resolve(possible, lshown);
				} else {
					vp.states[j] = resolve(possible, null);
				}
			}

			PROFILES.put(name, vp);
		}
	}

	public static void setupProfiles(JsonObject profiles) {

		if (profiles != null) {
			_setup_(profiles);
		}

		if (!PROFILES.containsKey(DEFAULT_PROFILE_NAME)) {
			PROFILES.put(DEFAULT_PROFILE_NAME, ViewProfile.makeDefaultProfile());
		}

		DEFAULT_PROFILE = PROFILES.get(DEFAULT_PROFILE_NAME);
	}


	@SuppressWarnings("unchecked")
	/* where carray is an array of JsonStrings encoding class names */
	public static Class<? extends ViewParent>[] classValuesFrom(JsonArray carray) {
		ArrayList<Class<? extends ViewParent>> classes =
				new ArrayList<>();

		for (JsonValue val : carray) {
			Class<? extends ViewParent> c = null;
			try {
				c = (Class<? extends ViewParent>) Class.forName(val.asString().value);
			} catch (Exception e) {
				Debug.error( "_____ PREVIOUSLY LOADED CLASS COULD NOT BE FOUND { %s }! _____",
						val.asString().value);
				e.printStackTrace();
			}

			if (c != null) {
				classes.add(c);
			}
		}

		return classes.toArray(new Class[0]);
	}

	private static ViewState[] resolve(Class<? extends ViewParent>[] possible,
			Class<? extends ViewParent>[] lastShown) {
		ViewState[] ret = new ViewState[possible.length];
		HashSet<Class<? extends ViewParent>> pset =
				new HashSet<Class<? extends ViewParent>>(Arrays.asList(possible));

		int i = 0;
		if (lastShown != null) {
			/* create true VS for every class in lastShown */
			for (Class<? extends ViewParent> cls : lastShown) {
				if (pset.contains(cls)) {
					ret[i++] = new ViewState(true, cls);
					pset.remove(cls);
				}
			}

			/* add false VS for every class left in pset */
			for (Class<? extends ViewParent> cls : pset) {
				ret[i++] = new ViewState(false, cls);
			}
		} else {
			/* no last shown for this type, add true VS for all */
			for (Class<? extends ViewParent> cls : pset) {
				ret[i++] = new ViewState(true, cls);
			}
		}

		assert(i == possible.length);
		return ret;
	}

	public static JsonObject serializeProfiles() {

		JsonObject allProfiles = Json.object();

		for (Entry<String, ViewProfile> entry : PROFILES.entrySet()) {
			assert(entry.getKey().equals(entry.getValue().name));

			ViewProfile vp = entry.getValue();
			JsonObject vpO = Json.object();

			for (String type : TYPES) {
				Class<? extends ViewParent>[] sel = vp.selected(type);

				JsonArray carray = Json.array();
				for (Class<? extends ViewParent> cls : sel) {
					carray.add(cls.getName());
				}

				vpO.put(type, carray);
			}

			allProfiles.put(vp.name, vpO);
		}

		return allProfiles;
	}

	public static final Map<String, Class<? extends ViewParent>[]> POSSIBLE_VIEWS =
			new HashMap<String, Class<? extends ViewParent>[]>();

	public static String[] TYPES;

	/* NOTE: not _NBL_REQUIRED_START_ because it is a dependency of many other components */
	/* Called directly in terminal main() */
	@SuppressWarnings("unchecked")
	public static void findAllViews() {
		Debug.warn("ViewProfile finding all subclasses of ViewParent");
		List<Class<?>> found = ClassFinder.findAllSubclasses(ViewParent.class);
		Map<String, List<Class<?>>> pv_map =
				new HashMap<>();

		for (Class<?> cls : found) {
			ViewParent inst;
			String[] types = null;
			try {
				inst = (ViewParent) cls.getDeclaredConstructor().newInstance();
				types = inst.displayableTypes();
			} catch (InstantiationException | IllegalAccessException | IllegalArgumentException
					| InvocationTargetException | NoSuchMethodException | SecurityException e) {
				e.printStackTrace();

				Debug.error("*************************\n" +
							"\tfatal error checking no-args constructor of ViewParent subclass:\n" +
							"\t%s error: %s", cls.getName(), e.getMessage()
						);
				System.exit(-1);

			}

			if (types == null) {
				Debug.error("class %s returns NULL displayableTypes !", cls.getName());
				continue;
			}

			for (String t : types) {
				if (pv_map.containsKey(t)) {
					pv_map.get(t).add(cls);
				} else {
					LinkedList<Class<?>> newList = new LinkedList<>();
					newList.add(cls);
					pv_map.put(t, newList);
				}
			}
		}

		for (String type : pv_map.keySet()) {
			POSSIBLE_VIEWS.put(type, pv_map.get(type).toArray(new Class[0]));
		}

		TYPES = POSSIBLE_VIEWS.keySet().toArray(new String[0]);
		Debug.info("%s", POSSIBLE_VIEWS.toString());
	}
}





