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
import java.util.Map;
import java.util.Map.Entry;
import java.util.Properties;
import java.util.Set;
import java.util.prefs.BackingStoreException;
import java.util.prefs.Preferences;

import nbtool.data.SExpr;
import nbtool.data.ViewProfile;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.Logger.LogLevel;

public class Prefs {

	/*
	 * CLASS EXCEPTIONS!  See wiki.
	 */
	private static final String BUNDLE_CE_NAME = "DEFAULT_CLASS_EXCEPTIONS.properties";
	public static final HashMap<String, String> CLASS_EXCEPTIONS_MAP = load_CLASS_EXCEPTIONS_MAP();
	public static HashMap<String, String> load_CLASS_EXCEPTIONS_MAP()  {
		try {
			Properties prop = new Properties();
			InputStream in = new FileInputStream(Prefs.class.getResource(BUNDLE_CE_NAME).getPath());
			prop.load(in);
			in.close();

			Logger.log(Logger.INFO, "load_CLASS_EXCEPTIONS_MAP(): found " + prop.size() + " exceptions.");

			HashMap<String, String> ret = new  HashMap<String, String>();

			for (Entry<Object, Object> e : prop.entrySet()) {
				ret.put((String) e.getKey(), (String) e.getValue());
			}

			return ret;
		} catch (Exception e) {
			e.printStackTrace();
			Logger.log(Logger.ERROR, "Cannot run without class exception mapping.");
			System.exit(1);
			return null;
		}
	}

	/* "OLD" preference values – stored at top level. */
	//bounds (and default) for main display
	public static Rectangle bounds = NBConstants.DEFAULT_BOUNDS;
	//splitpane inits for main display
	public static int leftSplitLoc = -1, rightSplitLoc = -1;
	//saved user inputs..
	public static LinkedList<String> addresses = new LinkedList<String>();
	public static LinkedList<String> filepaths = new LinkedList<String>();
	public static LogLevel logLevel = LogLevel.levelINFO;
	
	/* "NEW" preference values - stored inside appropriate key */
	private static final String EXT_BOUNDS_KEY = "__EXT_BOUNDS__";
	private static final String MISC_KEY = "__MISC__";
	private static final String STATIC_PREFS_KEY = "__STATIC__";
	
	public static class ExtBounds {
		public Rectangle bounds;
		public ViewProfile profile;
		
		public ExtBounds(Rectangle r, ViewProfile p) {
			bounds = r; profile = p;
		}
		
		public SExpr serialize() {
			SExpr top = SExpr.list();
			if (bounds == null)
				top.append(SExpr.atom("__NULL__"));
			else {
				top.append(
						SExpr.atom(bounds.x),
						SExpr.atom(bounds.y),
						SExpr.atom(bounds.width),
						SExpr.atom(bounds.height)
						);
			}
			
			if (profile == null) {
				top.append(SExpr.atom("__NULL__"));
			} else {
				top.append(SExpr.atom(profile.name));
			}
			
			return top;
		}
		
		public ExtBounds(SExpr s) {
			assert(s.isList());
			assert(s.count() >= 2);
			
			int loc = 0;
			if (s.get(0).isAtom() && s.get(0).value().equals("__NULL__")) {
				bounds = null;
				loc = 1;
			} else {
				bounds = new Rectangle(s.get(0).valueAsInt(),
						s.get(1).valueAsInt(),
						s.get(2).valueAsInt(),
						s.get(3).valueAsInt());
				loc = 4;
			}
			
			if (s.get(loc).isAtom() && s.get(loc).value().equals("__NULL__")) {
				profile = null;
			} else {
				String pname = s.get(loc).value();
				//if get() returns null, then profile should be null.
				profile = ViewProfile.PROFILES.get(pname);
			}
		}
		
		@Override
		public String toString() {
			return String.format("ExtBounds{%s}{%s}", bounds, profile);
		}
	}
	
	public static final Map<String, ExtBounds> BOUNDS_MAP = new HashMap<>();
	public static final Map<String, SExpr> MISC_MAP = new HashMap<>();

	/* SAVE PREFERENCES TO FILESYSTEM */
	public static void savePreferences() throws IOException {
		File fp = new File(Utility.localizePath(NBConstants.USER_PREFERENCES));
		Logger.logf(Logger.INFO, "saving preferences to %s", fp.getAbsolutePath());

		SExpr top = SExpr.newList();
		top.append(SExpr.newAtom("nbtool-prefs"));
		
		SExpr staticPrefs = SExpr.newList(SExpr.atom(STATIC_PREFS_KEY));
		top.append(staticPrefs);
		
		staticPrefs.append(SExpr.newList(
				SExpr.newAtom("bounds"),
				SExpr.newAtom(bounds.x),
				SExpr.newAtom(bounds.y),
				SExpr.newAtom(bounds.width),
				SExpr.newAtom(bounds.height)
				));
		
		staticPrefs.append(SExpr.list(
				SExpr.atom("split"),
				SExpr.atom(leftSplitLoc),
				SExpr.atom(rightSplitLoc)
				));
		
		{
			SExpr list = SExpr.newList(SExpr.newAtom("addresses"));
			for (String a : addresses)
				list.append(SExpr.newAtom(a));
			staticPrefs.append(list);
		}

		{
			SExpr list = SExpr.newList(SExpr.newAtom("filepaths"));
			for (String f : filepaths)
				list.append(SExpr.newAtom(f));
			staticPrefs.append(list);
		}

		staticPrefs.append(SExpr.newKeyValue("logLevel", logLevel.name()));

		top.append(ViewProfile.makeProfilesSExpr());
		
		SExpr bnds_se = SExpr.list(SExpr.atom(EXT_BOUNDS_KEY));
		for (Entry<String, ExtBounds> entry : BOUNDS_MAP.entrySet()) {
			bnds_se.append(SExpr.pair(entry.getKey(), entry.getValue().serialize()));
		}
		top.append(bnds_se);
		
		SExpr misc_se = SExpr.list(SExpr.atom(MISC_KEY));
		for (Entry<String, SExpr> entry : MISC_MAP.entrySet()) {
			misc_se.append(SExpr.pair(entry.getKey(), entry.getValue()));
		}
		top.append(misc_se);
		
		String writeable = top.print();
		byte[] bytes = writeable.getBytes(StandardCharsets.UTF_8);
		Files.write(fp.toPath(), bytes);
	}

	/* LOAD PREFERENCES FROM FILESYSTEM */
	public static void loadPreferences() throws IOException, ClassNotFoundException {
		Path prefPath = Paths.get(Utility.localizePath(NBConstants.USER_PREFERENCES));
		System.out.println("\tfetching user preferences from: " + prefPath);
		System.out.println("\tusing NBITES_DIR=" + NBConstants.NBITES_DIR);

		if (!prefPath.toFile().exists()) {
			Logger.log(Logger.WARN, "preferences file not found.");
			return;
		}
		String prefText = new String(Files.readAllBytes(prefPath),
				StandardCharsets.UTF_8);

		SExpr prefs = SExpr.deserializeFrom(prefText);
		if (prefs == null || prefs.count() < 1 || 
				!(prefs.get(0).isAtom() && prefs.get(0).value().equals("nbtool-prefs") ) 
				) {
			Logger.log(Logger.WARN, "PREFERENCES FILE HAD INVALID FORMAT");
			return;
		}
		
		//Logger.println(prefs.print());
		
		SExpr staticPrefs = prefs.find(STATIC_PREFS_KEY);
		staticPrefs = staticPrefs.exists() ? staticPrefs : prefs;
		readStaticPrefs(staticPrefs);
				
		SExpr vp_se = prefs.find(ViewProfile.PROFILES_KEY);
		if (vp_se.exists()) {
			ViewProfile.setupProfiles(vp_se);
		} else {
			ViewProfile.setupProfiles(VIEW_PROFILE_NOT_FOUND);
		}
		
		SExpr ext_se = prefs.find(EXT_BOUNDS_KEY);
		if (ext_se.exists()) {
			for (int i = 1; i < ext_se.count(); ++i) {
				SExpr item = ext_se.get(i);
				String key = item.get(0).value();
				ExtBounds eb = new ExtBounds(item.get(1));
				
				//Logger.printf("****got %s from %s (%s)", eb, item.get(1).serialize(), key);
				
				BOUNDS_MAP.put(key, eb);
			}
		}
		
		SExpr msc_se = prefs.find(MISC_KEY);
		if (msc_se.exists()) {
			for (int i = 1; i < msc_se.count(); ++i) {
				SExpr item = ext_se.get(i);
				String key = item.get(0).value();
				
				MISC_MAP.put(key, item.get(1));
			}
		}
	}
	
	private static final SExpr VIEW_PROFILE_NOT_FOUND = SExpr.list(SExpr.atom(ViewProfile.PROFILES_KEY));
	
	private static void readStaticPrefs(SExpr staticPrefs) {
		if (staticPrefs.find("bounds").exists()) {
			SExpr pnode = staticPrefs.find("bounds");
			Rectangle nr = new Rectangle(pnode.get(1).valueAsInt(),
					pnode.get(2).valueAsInt(),
					pnode.get(3).valueAsInt(),
					pnode.get(4).valueAsInt());
			bounds = nr;
		}
		
		if (staticPrefs.find("split").exists()) {
			SExpr pnode = staticPrefs.find("split");
			leftSplitLoc = pnode.get(1).valueAsInt();
			rightSplitLoc = pnode.get(2).valueAsInt();
		}

		if (staticPrefs.find("addresses").exists()) {
			SExpr pnode = staticPrefs.find("addresses");
			LinkedList<String> nr = new LinkedList<String>();
			for (int i = 1; i < pnode.count(); ++i) {
				nr.add(pnode.get(i).value());
			}

			addresses = nr;
		}

		if (staticPrefs.find("filepaths").exists()) {
			SExpr pnode = staticPrefs.find("filepaths");
			LinkedList<String> nr = new LinkedList<String>();
			for (int i = 1; i < pnode.count(); ++i) {
				nr.add(pnode.get(i).value());
			}

			filepaths = nr;
		}

		if (staticPrefs.find("logLevel").exists()) {
			SExpr pnode = staticPrefs.find("logLevel");
			logLevel = Logger.LogLevel.valueOf(pnode.get(1).value());
		}
	}
}
