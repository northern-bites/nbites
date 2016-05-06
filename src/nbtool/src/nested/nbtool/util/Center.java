package nbtool.util;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;

import nbtool.util.Events.EventListener;

//public class Boss implements Executor {

public class Center {

	/*
	 * EventListener framework.
	 * */

	private static final HashMap<Class<? extends EventListener>, ArrayList<EventListener>> guiListeners =
			new HashMap<Class<? extends EventListener>, ArrayList<EventListener>>();
	private static final HashMap<Class<? extends EventListener>, ArrayList<EventListener>> centerListeners =
			new HashMap<Class<? extends EventListener>, ArrayList<EventListener>>();


	/*
	 * eventClass determines what list the listener is added to, and should match <I>
	 * */
	public static <I extends EventListener> void listen(Class<I> eventClass, I listener, boolean guiThread) {
		assert(eventClass.isInstance(listener));
		
		HashMap<Class<? extends EventListener>, ArrayList<EventListener>> relevant
			= guiThread ? guiListeners : centerListeners;
		synchronized(relevant) {
			if (relevant.containsKey(eventClass)) {
				relevant.get(eventClass).add(listener);
			} else {
				ArrayList<EventListener> listeners = new ArrayList<EventListener>();
				listeners.add(listener);

				relevant.put(eventClass, listeners);
			}
		}
	}

	/*
	 * Event running framework.
	 */
	private static Thread center_thread = null;

	public static void startCenter() {
		if (center_thread == null) {
			center_thread = new Thread(new CenterRunnable());
			center_thread.setName("nbtool-Center");
			center_thread.setDaemon(true);

			Debug.info( "Starting Center thread...");
			center_thread.start();
		} else {
			Debug.error( "Could not start Center, center_thread != null.");
		}
	}

	private static class CenterRunnable implements Runnable {
		
		@Override
		public void run() {
			Debug.info( "Center thread active.");
			
			try {
				while(true) {
//					System.out.println("Center loop...");
					ToolEvent head = null;
					Class<? extends EventListener> eventClass = null;
					LinkedList<ToolEvent> similar = null;
					synchronized(events) {
						if (events.size() == 0) {
							events.wait();
						}

						assert(events.size() > 0);
						head = events.pop();
						eventClass = head.listenerClass();
						
						if (head.canCombine() && eventClass != null) {
							similar = new LinkedList<ToolEvent>();
							
							for (ToolEvent e : events) {
								//if (e.listenerClass().equals(eventClass))
								if (eventClass.equals(e.listenerClass()))
									similar.add(e);
							}

							for (ToolEvent s : similar) {
								events.remove(s);
							}
						}
					}
					
					Debug.event( "Center choosing event %s", head.getClass().getName());

					assert(head != null);
					if (similar != null && head.canCombine()) {
						Debug.event( "Center combining event %s, %d instances", head.getClass().getName(),
								similar.size());
						head.combine(similar);
					}

					ArrayList<EventListener> glist = null;
					ArrayList<EventListener> clist = null;
					if (eventClass != null) {
						glist = guiListeners.get(eventClass);
						clist = centerListeners.get(eventClass);
						
						if (glist == null)
							glist = new ArrayList<EventListener>();
						if (clist == null)
							clist = new ArrayList<EventListener>();
						
						Debug.event( "Center executing event %s with %d cListeners, %d guiListeners.", head.getClass().getName(),
								clist.size(), glist.size());
					} else {
						Debug.event( "Center executing event %s with null event class.", head.getClass().getName());
					}

					head.execute(glist, clist);	//Wow, that sounds macabre.
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
				System.exit(1);
			}
		}
	}

	private static final LinkedList<ToolEvent> events = new LinkedList<ToolEvent>();
	public static void addEvent(ToolEvent e) {
		synchronized(events) {
			events.add(e);
			events.notify();
		}
	}

	public static boolean isCenterThread() {
		if (center_thread == null)
			return false;
		return Thread.currentThread() == center_thread;
	}

	public static abstract class ToolEvent {

		protected abstract boolean canCombine();
		protected abstract void combine(LinkedList<ToolEvent> others);

		protected abstract Class<? extends EventListener> listenerClass();
		protected abstract void execute(ArrayList<EventListener> guiList, ArrayList<EventListener> centerList);
		
	}
	
	/*
	 * tool shutdown handling
	 */
	public static interface NBToolShutdownListener {
		//Guaranteed to be called before user settings are saved, *if* user settings are saved.
		public void nbtoolShutdownCallback();
	}
	
	private static final LinkedList<NBToolShutdownListener> shutdownListeners = new LinkedList<>();
	
	public static void listen(NBToolShutdownListener li) {
		synchronized(shutdownListeners) {
			shutdownListeners.add(li);
		}
	}
	
	static {
		Debug.plain("Center adding preference shutdown hook.");
		Runtime.getRuntime().addShutdownHook(new Thread(new Runnable(){
			@Override
			public void run() {
				
				System.out.println("\n----------------------------------\nCenter hook saving preferences...");
				for (NBToolShutdownListener l : shutdownListeners) {
					Debug.print("\tinforming %s", l.toString());
					l.nbtoolShutdownCallback();
				}
				
				try {
					UserSettings.savePreferences();
				} catch (IOException e) {
					e.printStackTrace();
				}
				
				Debug.print("Center hook done.");
			}
		}));
	}
}
