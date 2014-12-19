package nbclient.util;

import java.util.ArrayList;
import java.util.HashMap;

import javax.swing.SwingUtilities;

//Notification center
public class N {
	
	public static enum EVENT {
		STATS(0), STREAM_UPDATE(1), SELECTION(2), LOGS_ADDED(3), LOG_LOADED(4), STATUS(5),
		CPP_FUNCS(6), CPP_CONNECTED(7);
		public int index;
		private EVENT(int i) {
			index = i;
		}
	};

	public interface NListener {
		public void notified(EVENT e, Object src, Object ... args);
	}
	
	//private static HashMap<EVENT, ArrayList<NListener>> listeners;
	private static ArrayList<NListener>[] listeners; 
	static{setup();}
	
	@SuppressWarnings("unchecked") 
	public static synchronized void setup() {
		listeners = new ArrayList[EVENT.values().length];
		for (int i = 0; i < EVENT.values().length; ++i)
			listeners[i] = new ArrayList<NListener>();
	}
	public static synchronized void listen(EVENT e, NListener l) {
		listeners[e.index].add(l);
	}
	public static synchronized int notify(EVENT e, Object src, Object ... args) {
		for (NListener nl : listeners[e.index])
			nl.notified(e, src, args);
		return listeners[e.index].size();
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
}
