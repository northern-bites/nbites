package nbtool.util;

import java.awt.Rectangle;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map.Entry;
import java.util.Properties;
import java.util.prefs.BackingStoreException;
import java.util.prefs.Preferences;

import nbtool.gui.logviews.misc.ViewParent;

public class P {
	
	/*
	 * Java stored preferences:
	 * 	display x pos
	 * 	display y pos
	 * 	display width
	 * 	display height
	 * 
	 * 	control primary values (up to 5)
	 * 	control secondary values (up to 5)
	 * 
	 * control paths (up to 5)
	 * control addresses (up to 5)
	 * 
	 * 	max log data storage
	 * */
		
	public static Preferences p = java.util.prefs.Preferences.userRoot().node(P.class.getName());
	
	private static final String xName = "NBClient_W_x";
	private static final String yName = "NBClient_W_y";
	private static final String wName = "NBClient_W_w";
	private static final String hName = "NBClient_W_h";
	
	private static final String heapName = "NBClient_heap";
	
	private static final String lastModeName = "NBClient_mode";
	
	private static final String pathsName = "NBClient_paths";
	private static final String addrsName = "NBClient_addrs";
	
	private static final String mapsDirName = "NBClient_mapsDir";
	
	public static String getMapsDir() {
		return p.get(mapsDirName, "");
	}
	
	public static void setMapsDir(String path) {
		p.put(mapsDirName, path);
	}
	
	public static long getHeap() {
		return p.getLong(heapName, NBConstants.DEFAULT_MAX_MEMORY_USAGE);
	}
	
	public static void putHeap(long nval) {
		p.putLong(heapName, nval);
	}
	
	public static int getLastMode() {
		return p.getInt(lastModeName, 0);
	}
	
	public static void putLastMode(int i) {
		if (i >= NBConstants.MODE.values().length)
			throw new IllegalArgumentException("mode index " + i + " is larger than possible mode indices.");
		p.putInt(lastModeName, i);
	}
	
	private static LinkedList<String> getOrderedSet(String name) {
		LinkedList<String> set = new LinkedList<String>();
		
		for (int i = 0; i < 5; ++i) {
			String opt = p.get(name + i, null);
			if (opt == null) break;
			if (set.contains(opt)) continue;
			set.add(opt);
		}
		
		return set;
	}
	
	private static String[] putOrderedSet(String name, String newv) {
		LinkedList<String> set = getOrderedSet(name);
		if (set.contains(newv)) {
			set.remove(newv);
			set.addFirst(newv);
		} else {
			set.addFirst(newv);
		}
		
		int bound = Math.min(5, set.size());
		for (int i = 0; i < bound; ++i) {
			p.put(name + i, set.get(i));
		}
		
		return set.toArray(new String[0]);
	}
	
	private static void clearOrderedSet(String name) {
		String[] keys = new String[0];
		try {
			keys = p.keys();
		} catch (BackingStoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		for (String k : keys) {
			if (k.startsWith(name))
				p.remove(k);
		}
	}
	
	public static String[] getPaths() {
		return getOrderedSet(pathsName).toArray(new String[0]);
	}
	
	public static String[] putPaths(String pth) {
		return putOrderedSet(pathsName, pth);
	}
	
	public static String[] getAddrs() {
		return getOrderedSet(addrsName).toArray(new String[0]);
	}
	
	public static String[] putAddrs(String a) {
		return putOrderedSet(addrsName, a);
	}
	
	public static Rectangle getBounds() {
		int x = p.getInt(xName, NBConstants.DEFAULT_BOUNDS.x);
		int y = p.getInt(yName, NBConstants.DEFAULT_BOUNDS.y);
		int width = p.getInt(wName, NBConstants.DEFAULT_BOUNDS.width);
		int height = p.getInt(hName, NBConstants.DEFAULT_BOUNDS.height);
		
		return new Rectangle(x, y, width, height);
	}
	
	public static void putBounds(Rectangle r) {
		p.putInt(xName, r.x);
		p.putInt(yName, r.y);
		p.putInt(wName, r.width);
		p.putInt(hName, r.height);
	}
	
	//Doesn't change window size atm.
	public static void resetNonFilePreferences() {
		putHeap(NBConstants.DEFAULT_MAX_MEMORY_USAGE);
		clearOrderedSet(pathsName);
		clearOrderedSet(addrsName);
		setMapsDir("");
	}
	
	/*****
	 * .nbtool-views
	 * .nbtool-exceptions
	 * @throws IOException 
	 */
	
	private static final String BUNDLE_CE_NAME = "DEFAULT_CLASS_EXCEPTIONS.properties";
	private static final String BUNDLE_LTV_NAME = "DEFAULT_LTV_MAP.properties";
	
	public static void copyOrReplaceMapping() throws IOException {
		U.w("Copying log-to-view mapping to home from bundle.");
		//Need to copy from package.
		Path inpckg = FileSystems.getDefault().getPath(P.class.getResource(BUNDLE_LTV_NAME).getPath());
		Files.copy(inpckg, VIEW_PATH, StandardCopyOption.REPLACE_EXISTING);
	}
	
	public static void copyOrReplaceExceptions() throws IOException {
		U.w("Copying class exceptions to home from bundle.");
		//Need to copy from package.
		Path inpckg = FileSystems.getDefault().getPath(P.class.getResource(BUNDLE_CE_NAME).getPath());
		Files.copy(inpckg, EXCEPTIONS_PATH, StandardCopyOption.REPLACE_EXISTING);
	}
	
	public static final Path VIEW_PATH = FileSystems.getDefault().getPath(U.localizePath(NBConstants.USER_LOG_TO_VIEW_MAP));
	public static final Path EXCEPTIONS_PATH = FileSystems.getDefault().getPath(U.localizePath(NBConstants.USER_CLASS_EXCEPTIONS));
	
	public static final HashMap<String, ArrayList<Class<? extends ViewParent>>> LTVIEW_MAP = load_LTVIEW_MAP();
	public static final HashMap<String, String> CLASS_EXCEPTIONS_MAP = load_CLASS_EXCEPTIONS_MAP();
	
	public static HashMap<String, ArrayList<Class<? extends ViewParent>>> load_LTVIEW_MAP()  {
		try {
			File f = VIEW_PATH.toFile();
			if (!f.exists()) {
				copyOrReplaceMapping();
			}

			assert(f.exists());


			Properties prop = new Properties();
			InputStream in = new FileInputStream(f);
			prop.load(in);
			in.close();

			U.w("P: load_LTVIEW_MAP(): found " + prop.size() + " types.");

			HashMap<String, ArrayList<Class<? extends ViewParent>>> ret = new  HashMap<String, ArrayList<Class<? extends ViewParent>>>();

			for (Entry<Object, Object> e : prop.entrySet()) {
				String el = (String) e.getValue();
				String[] parts = el.trim().split(" ");
				ArrayList<Class<? extends ViewParent>> views = new ArrayList<Class<? extends ViewParent>>();
				for (String viewName : parts) {
					if (viewName.isEmpty()) continue;
					views.add((Class<? extends ViewParent>) Class.forName(viewName));
				}
				
				ret.put((String) e.getKey(), views); 
			}

			return ret;
		} catch (Exception e) {
			e.printStackTrace();
			U.w("Cannot run without log-to-view mapping.");
			System.exit(1);
			return null;
		}
	}
	
	public static HashMap<String, String> load_CLASS_EXCEPTIONS_MAP()  {
		try {
			File f = EXCEPTIONS_PATH.toFile();
			if (!f.exists()) {
				copyOrReplaceExceptions();
			}

			assert(f.exists());

			Properties prop = new Properties();
			InputStream in = new FileInputStream(f);
			prop.load(in);
			in.close();

			U.w("P: load_CLASS_EXCEPTIONS_MAP(): found " + prop.size() + " exceptions.");

			HashMap<String, String> ret = new  HashMap<String, String>();

			for (Entry<Object, Object> e : prop.entrySet()) {
				 ret.put((String) e.getKey(), (String) e.getValue());
			}

			return ret;
		} catch (Exception e) {
			e.printStackTrace();
			U.w("Cannot run without class exception mapping.");
			System.exit(1);
			return null;
		}
	}
	
	
	//
	//ltv2
	
}
