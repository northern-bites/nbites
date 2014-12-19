package nbclient.util;

import java.awt.Rectangle;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.CopyOption;
import java.nio.file.FileSystem;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;
import java.util.Properties;
import java.util.prefs.Preferences;

import nbclient.gui.logviews.parent.ViewParent;


/*
 * 
 * */

public class P {
	public static Preferences p = java.util.prefs.Preferences.userRoot().node(P.class.getName());
	public static Rectangle getBounds() {
		int x = p.getInt("NB_Client_W_x", 0);
		int y = p.getInt("NB_Client_W_y", 0);
		int width = p.getInt("NB_Client_W_width", 900);
		int height = p.getInt("NB_Client_W_height", 600);
		
		return new Rectangle(x, y, width, height);
	}
	
	public static void putBounds(Rectangle r) {
		p.putInt("NB_Client_W_x", r.x);
		p.putInt("NB_Client_W_y", r.y);
		p.putInt("NB_Client_W_width", r.width);
		p.putInt("NB_Client_W_height", r.height);
	}
	
	public static final String PL_PREFIX = "NB_Client_PRIMARY";
	private static ArrayList<String> pcached = null;
	public static String[] getPrimaryValues() {
		if (pcached != null) return pcached.toArray(new String[pcached.size()]);
		ArrayList<String> opts = new ArrayList<String>();
		for (int i = 0; i < 5; ++i) {
			String topt = p.get(PL_PREFIX + i, null);
			if (topt == null) break;
			opts.add(topt);
		}
		
		pcached = opts;
		return opts.toArray(new String[opts.size()]);
	}
	
	public static String[] putPrimaryValue(String newest) {
		ArrayList<String> ret;
		if (pcached != null) ret = pcached;
		else {
			getPrimaryValues();
			assert(pcached != null);
			ret = pcached;
		}
		
		ret.add(0, newest);
		if (ret.size() > 5)
			ret.remove(ret.size() - 1);
		
		for (int i = 0; i < ret.size(); ++i) {
			p.put(PL_PREFIX + i, ret.get(i));
		}
		
		return ret.toArray(new String[ret.size()]);
	}
	
	public static final String SL_PREFIX = "NB_Client_SECOND";
	private static ArrayList<String> scached = null;
	public static String[] getSecondaryValues() {
		if (scached != null) return scached.toArray(new String[scached.size()]);
		ArrayList<String> opts = new ArrayList<String>();
		for (int i = 0; i < 5; ++i) {
			String topt = p.get(SL_PREFIX + i, null);
			if (topt == null) break;
			opts.add(topt);
		}
		
		scached = opts;
		return opts.toArray(new String[opts.size()]);
	}
	
	public static String[] putSecondValue(String newest) {
		ArrayList<String> ret;
		if (scached != null) ret = scached;
		else {
			getSecondaryValues();
			assert(scached != null);
			ret = scached;
		}
		
		ret.add(0, newest);
		if (ret.size() > 5)
			ret.remove(ret.size() - 1);
		
		for (int i = 0; i < ret.size(); ++i) {
			p.put(SL_PREFIX + i, ret.get(i));
		}
		
		return ret.toArray(new String[ret.size()]);
	}
	
	/*****
	 * .nbclient-views
	 * .nbclient-exceptions
	 */
	
	public static final Path VIEW_PATH = FileSystems.getDefault().getPath(U.localizePath("~/.nbclient-views.properties"));
	public static final Path EXCEPTIONS_PATH = FileSystems.getDefault().getPath(U.localizePath("~/.nbclient-exceptions.properties"));
	
	public static final HashMap<String, ArrayList<Class<? extends ViewParent>>> LTVIEW_MAP = load_LTVIEW_MAP();
	public static final HashMap<String, String> CLASS_EXCEPTIONS_MAP = load_CLASS_EXCEPTIONS_MAP();
	
	public static HashMap<String, ArrayList<Class<? extends ViewParent>>> load_LTVIEW_MAP()  {
		try {
			File f = VIEW_PATH.toFile();
			if (!f.exists()) {
				U.w("Copying log-to-view mapping to home from bundle.");
				//Need to copy from package.
				Path inpckg = FileSystems.getDefault().getPath(P.class.getResource("logtoview.properties").getPath());
				Files.copy(inpckg, VIEW_PATH);
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
				//Need to copy from package.
				U.w("Copying exception mapping to home from bundle.");
				Path inpckg = FileSystems.getDefault().getPath(P.class.getResource("class_exceptions.properties").getPath());
				Files.copy(inpckg, EXCEPTIONS_PATH);
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
}