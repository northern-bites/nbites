package nbclient.util;

import java.util.ArrayList;

import javax.swing.SwingUtilities;

//Notification center
public class N {
	
	public static enum EVENT {
		
		CPP_CONNECTION, /*(true/false)*/ CPP_FUNCS_FOUNDS,
		CNC_CONNECTION, /*(true/false)*/
		SIO_THREAD, /*(true/false)*/
		FIO_THREAD, /*(true/false)*/
		
		LOG_LOAD,
		LOG_FOUND,
		LOG_DROP,
		
		REL_BOTSTAT,
		
		LOG_SELECTION,
		SES_SELECTION,
		
		STATUS; //(src) status mode
				
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
	
	public static synchronized int notify(final EVENT e, final Object src, final Object ... args) {
		ArrayList<NListener> list = listeners[e.index];
		
		synchronized (list) {
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
}
