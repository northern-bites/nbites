package nbtool.util;

import java.util.ArrayList;

import javax.swing.SwingUtilities;

import nbtool.data.RobotStats;
import nbtool.io.CppIO.CppFunc;

//Notification center
public class N {
	
	public static enum EVENT {
		
		CPP_CONNECTION, /*(true/false)*/ 	//From CPP thread
		CPP_FUNCS_FOUND, /*(ArrayList<CppFunc>)*/	//From CPP thread
		
		CNC_CONNECTION, /*(true/false)*/	//From CommandIO thread
		SIO_THREAD, /*(true/false)*/	//From NetIO thread
		FIO_THREAD, /*(true/false)*/	//From Fileio thread
		
		LOG_LOAD, /*(loaded log)*/	//From logchooser
		LOG_FOUND, /*(found logs, as array)*/	//From SessionHandler
		LOG_DROP, /*(dropped log) (long dropped bytes)*/	//From SessionMaster
		
		REL_BOTSTAT, /*(new relevant botstat object)*/	//From SessionMaster
		STATS,											//From Stats object
		
		LOG_SELECTION, /*(selected log)(array of tree pathes)*/				//From chooser gui
		SES_SELECTION, /*(selected session)*/			//From chooser gui
		
		MAX_MEM_USAGE_CHANGED, /*(new value)*/			//From prefs
		
		STATUS; //(status) (mode)						//From SessionHandler
				
		protected int index;
		private EVENT() {
			index = -1;
		}
		
		static {
			int i = 0;
			for (EVENT e : EVENT.values() ) {
				e.index = i++;
			}
		}
	};

	public interface NListener {
		public void notified(EVENT e, Object src, Object ... args);
	}
	
	private static ArrayList<NListener>[] listeners = setup();
	
	@SuppressWarnings("unchecked")
	private static ArrayList<NListener>[] setup() {
		int len = EVENT.values().length;
		ArrayList<NListener>[] ret = new ArrayList[len];
		for (int i = 0; i < len; ++i) 
			ret[i] = new ArrayList<NListener>();
		
		return ret;
	}
	
	public static void listen(EVENT e, NListener l) {
		ArrayList<NListener> list = listeners[e.index];
		synchronized(list) {
			list.add(l);
		}
	}
	
	private static int notify(final EVENT e, final Object src, final Object ... args) {
		ArrayList<NListener> list = listeners[e.index];
		
		synchronized (list) {
			if (P.VERBOSE)
				U.w(">>notify: " + e.toString() + " from " + src.getClass().getCanonicalName() + " args: " + represent(args));
			
			for (NListener nl : list)
				nl.notified(e, src, args);
		}
		
		return list.size();
	}
	
	//Not synchronized, we just end up calling synchd notify on EDT
	public static int notifyEDT(final EVENT e, final Object src, final Object... args){
		SwingUtilities.invokeLater(new Runnable(){
			public void run() {
				N.notify(e, src, args);
			}
		});
		
		return listeners[e.index].size();
	}
	
	
	private static String represent(Object ... objs) {
		
		if (objs.length == 0)
			return "()";
		
		String ret = "(" + represent(objs[0]);
		
		for (int i = 1; i < objs.length; ++i) {
			ret += ", " + represent(objs[i]);
		}
		
		if (ret.length() > 200)
			return ret.substring(0, 200) + "... TRUNC )";
		
		return ret + ")";
	}
	
	private static String represent(Object o) {
		
		if (o instanceof Boolean)
			return String.format("%B", o);
		
		if (o instanceof RobotStats)
			return String.format("[BotStats from ut %d]", ((RobotStats) o).log_uptime);
		
		if (o instanceof ArrayList)
			return String.format("[ArrayList of size %d]", ((ArrayList) o).size());
		
		return "[" + o.toString() + "]";
	}

}
