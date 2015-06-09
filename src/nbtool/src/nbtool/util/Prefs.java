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
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.Logger.LogLevel;

public class Prefs {

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

	public static Rectangle bounds = NBConstants.DEFAULT_BOUNDS;
	public static LinkedList<String> addresses = new LinkedList<String>();
	public static LinkedList<String> filepaths = new LinkedList<String>();
	public static LogLevel logLevel = LogLevel.levelINFO;
	public static int lastMode = 0;

	public static Map<String, Class<? extends ViewParent>[]> last_shown = NBConstants.POSSIBLE_VIEWS;

	public static void savePreferences() throws IOException {
		File fp = new File(Utility.localizePath(NBConstants.USER_PREFERENCES));
		Logger.logf(Logger.INFO, "saving preferences to %s", fp.getAbsolutePath());

		SExpr top = SExpr.newList();
		top.append(SExpr.newAtom("nbtool-prefs"));
		
		
		//
		top.append(SExpr.newList(
				SExpr.newAtom("bounds"),
				SExpr.newAtom(bounds.x),
				SExpr.newAtom(bounds.y),
				SExpr.newAtom(bounds.width),
				SExpr.newAtom(bounds.height)
				));
		
		{
			SExpr list = SExpr.newList(SExpr.newAtom("addresses"));
			for (String a : addresses)
				list.append(SExpr.newAtom(a));
			top.append(list);
		}

		{
			SExpr list = SExpr.newList(SExpr.newAtom("filepaths"));
			for (String f : filepaths)
				list.append(SExpr.newAtom(f));
			top.append(list);
		}

		top.append(SExpr.newKeyValue("logLevel", logLevel.name()));

		top.append(SExpr.newKeyValue("lastMode",""+ lastMode));

		{
			SExpr list = SExpr.newList(SExpr.newAtom("last_shown"));
			
			for (Entry<String,Class<? extends ViewParent>[]> e : last_shown.entrySet()) {
				SExpr sublist = SExpr.newList();
				
				sublist.append(SExpr.newAtom(e.getKey()));
				
				for (Class<? extends ViewParent> cls : e.getValue()) {
					sublist.append(SExpr.newAtom(cls.getName()));
				}
				
				list.append(sublist);
			}
			
			top.append(list);
		}
		
		String writeable = top.print();
		byte[] bytes = writeable.getBytes(StandardCharsets.UTF_8);
		Files.write(fp.toPath(), bytes);
	}

	public static void loadPreferences() throws IOException, ClassNotFoundException {
		Path prefPath = Paths.get(Utility.localizePath(NBConstants.USER_PREFERENCES));
		if (!prefPath.toFile().exists()) {
			Logger.log(Logger.WARN, "preferences file not found.");
			return;
		}
		String prefText = new String(Files.readAllBytes(prefPath),
				StandardCharsets.UTF_8);

		SExpr prefs = SExpr.deserializeFrom(prefText);
		if (prefs == null || prefs.count() < 1) {
			Logger.log(Logger.WARN, "preferences file in invalid format.");
			return;
		}

		if (prefs.find("bounds").exists()) {
			SExpr pnode = prefs.find("bounds");
			Rectangle nr = new Rectangle(pnode.get(1).valueAsInt(),
					pnode.get(2).valueAsInt(),
					pnode.get(3).valueAsInt(),
					pnode.get(4).valueAsInt());
			bounds = nr;
		}

		if (prefs.find("addresses").exists()) {
			SExpr pnode = prefs.find("addresses");
			LinkedList<String> nr = new LinkedList<String>();
			for (int i = 1; i < pnode.count(); ++i) {
				nr.add(pnode.get(i).value());
			}

			addresses = nr;
		}

		if (prefs.find("filepaths").exists()) {
			SExpr pnode = prefs.find("filepaths");
			LinkedList<String> nr = new LinkedList<String>();
			for (int i = 1; i < pnode.count(); ++i) {
				nr.add(pnode.get(i).value());
			}

			filepaths = nr;
		}

		if (prefs.find("logLevel").exists()) {
			SExpr pnode = prefs.find("logLevel");
			logLevel = Logger.LogLevel.valueOf(pnode.get(1).value());
		}

		if (prefs.find("lastMode").exists()) {
			SExpr pnode = prefs.find("lastMode");
			lastMode = pnode.get(1).valueAsInt();
		}

		if (prefs.find("last_shown").exists()) {
			SExpr pnode = prefs.find("last_shown");
			Map<String, Class<? extends ViewParent>[]> loaded = new HashMap<String, Class<? extends ViewParent>[]>();
			
			for (int i = 1; i < pnode.count(); ++i) {
				SExpr cmap = pnode.get(i);
				
				String type = cmap.get(0).value();
				ArrayList<Class> classes = new ArrayList<Class>();
				
				for (int j = 1; j < cmap.count(); ++j) {
					Class c = null;
					try {
						c = Class.forName(cmap.get(j).value());
					} catch (Exception e) {
						Logger.log(Logger.ERROR, "_____ PREVIOUSLY LOADED CLASS COULD NOT BE FOUND! _____");
						e.printStackTrace();
					}
					
					if (c != null) {
						classes.add(c);
					}
				}
				
				loaded.put(type, classes.toArray(new Class[0]));
			}
			
			last_shown = loaded;
		}

	}
}
