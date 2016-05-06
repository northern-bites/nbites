package nbtool.util;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;

import javax.swing.SwingUtilities;

import nbtool.data.RobotStats;
import nbtool.data.Session;
import nbtool.data._log._Log;
import nbtool.io.ControlIO.ControlInstance;
import nbtool.io.FileIO.FileInstance;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.StreamIO.StreamInstance;
import nbtool.util.Center.ToolEvent;
import nbtool.util.ToolSettings.STATUS;

public class Events {
	
	public static abstract interface EventListener{}
	
	private static abstract class SimpleForEach extends ToolEvent {
		
		private Class<? extends EventListener> eclass;
		
		public SimpleForEach(Class<? extends EventListener> eclass) {
			this.eclass = eclass;
		}
		
		@Override
		protected boolean canCombine() {
			return false;
		}

		@Override
		protected void combine(LinkedList<ToolEvent> others) {}

		@Override
		protected Class<? extends EventListener> listenerClass() { 
			return eclass;
		}

		@Override
		protected void execute(ArrayList<EventListener> guiList,
				ArrayList<EventListener> centerList) {
			
			assert(Center.isCenterThread());
			
			this.preface();
			
			for (EventListener ce : centerList) {
				this.inform(ce);
			}
			
			final SimpleForEach sfe = this;
			final EventListener[] listeners = guiList.toArray(new EventListener[0]);
			
			try {
				SwingUtilities.invokeAndWait(new Runnable() {

					@Override
					public void run() {
						
						for (EventListener ge : listeners) {
							sfe.inform(ge);
						}
						
					}
					
				});
			} catch (InvocationTargetException e) {
				e.printStackTrace();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			
		}
		
		protected abstract void preface();
		protected abstract void inform(EventListener l);
	}
	
	//SimpleCombine can only combine SimpleCombine events.
	private static abstract class SimpleCombine extends SimpleForEach {
		
		protected Object[] payload;

		protected SimpleCombine(Class<? extends EventListener> eclass,
				Object ... load
				) {
			super(eclass);
			this.payload = load;
		}
		
		@Override
		protected boolean canCombine() {
			return true;
		}

		@Override
		protected abstract void combine(LinkedList<ToolEvent> others);
	}
	
	/*
	 * Most of the specific EventListener sub-interfaces.
	 */

	/* NBCROSS EVENTS */
	//Get functions from instance.
	public static interface CrossStatus extends EventListener {
		public void nbCrossFound(CrossInstance inst, boolean up);
	}
	
	public static final class GCrossStatus {
		public static void generate(final CrossInstance inst, final boolean up) {
			
			Center.addEvent(new SimpleForEach(CrossStatus.class){

						@Override
						protected void preface() {
							Debug.event( "CrossStatus: %s %b",
									inst.name(), up);
						}

						@Override
						protected void inform(EventListener l) {
							((CrossStatus) l).nbCrossFound(inst, up);
						}
				
			});
		}
	}

	/* STREAM EVENTS */
	public static interface StreamIOStatus extends EventListener {
		public void streamStatus(StreamInstance inst, boolean up);
	}
	
	public static final class GStreamIOStatus {
		public static void generate(final StreamInstance inst, final boolean up) {
			Center.addEvent(new SimpleForEach(StreamIOStatus.class) {

				@Override
				protected void preface() {
					Debug.event( "StreamIOStatus: %s %b",
							inst.name(), up);
				}

				@Override
				protected void inform(EventListener l) {
					((StreamIOStatus) l).streamStatus(inst, up);
				}

			});
		}
	}
	
	/* CONTROL EVENTS */
	public static interface ControlStatus extends EventListener {
		public void controlStatus(ControlInstance inst, boolean up);
	}
	
	public static final class GControlStatus {
		public static void generate(final ControlInstance inst, final boolean up) {
			Center.addEvent(new SimpleForEach(ControlStatus.class) {

				@Override
				protected void preface() {
					Debug.event( "ControlStatus: %s %b",
							inst.name(), up);
				}

				@Override
				protected void inform(EventListener l) {
					((ControlStatus) l).controlStatus(inst, up);
				}

			});
		}
	}
	
	/* FILEIO EVENTS */
	public static interface FileIOStatus extends EventListener {
		public void fileioStatus(FileInstance fi, boolean up);
	}
	
	public static final class GFileIOStatus {
		public static void generate(final FileInstance inst, final boolean up) {
			Center.addEvent(new SimpleForEach(FileIOStatus.class) {

				@Override
				protected void preface() {
					Debug.event( "FileIOStatus: %s %b",
							inst.name(), up);
				}

				@Override
				protected void inform(EventListener l) {
					((FileIOStatus) l).fileioStatus(inst, up);
				}

			});
		}
	}
	
	public static interface LogLoaded extends EventListener {
		public void logLoaded(Object source, _Log ... loaded);//SOURCE NULL if combined.
	}
	
	public static final class GLogLoaded {
		public static void generate(final Object source, final _Log ... loaded) {
			Center.addEvent(new SimpleCombine(LogLoaded.class, source, loaded){

				@Override
				protected void combine(LinkedList<ToolEvent> others) {
					assert(this.payload.length == 2);
					if (others.size() == 0)
						return;
					
					_Log[] ours = (_Log[]) payload[1];
					ArrayList<_Log> alsoLoaded = new ArrayList<_Log>();
					alsoLoaded.addAll(Arrays.asList(ours));
					
					for (ToolEvent te : others) {
						assert(te instanceof SimpleCombine);
						
						_Log[] theirs = (_Log[]) ((SimpleCombine) te).payload[1];
						alsoLoaded.addAll(Arrays.asList(theirs));
					}
					
					Debug.info( "LogLoaded combined %d events with %d logs.", others.size(), alsoLoaded.size());
					
					this.payload[0] = null;
					this.payload[1] = alsoLoaded.toArray(new _Log[0]);
				}

				@Override
				protected void preface() {
					Debug.event( "LogLoaded...");
				}

				@Override
				protected void inform(EventListener l) {
					_Log[] logs = (_Log[]) payload[1];
					((LogLoaded) l).logLoaded(payload[0], logs);
				}
				
			});
		}
	}
	
	public static interface LogsFound extends EventListener {
		public void logsFound(Object source, _Log ... found);
	}
	
	public static final class GLogsFound {
		public static void generate(final Object source, final _Log ... found) {
			Center.addEvent(new SimpleCombine(LogsFound.class, source, found){

				@Override
				protected void combine(LinkedList<ToolEvent> others) {
					assert(this.payload.length == 2);
					if (others.size() == 0)
						return;
					
					_Log[] ours = (_Log[]) payload[1];
					ArrayList<_Log> alsoFound = new ArrayList<_Log>();
					alsoFound.addAll(Arrays.asList(ours));
					
					for (ToolEvent te : others) {
						assert(te instanceof SimpleCombine);
						
						_Log[] theirs = (_Log[]) ((SimpleCombine) te).payload[1];
						alsoFound.addAll(Arrays.asList(theirs));
					}
					
					Debug.info( "LogFound combined %d events with %d logs.", others.size(), alsoFound.size());
					
					this.payload[0] = null;
					this.payload[1] = alsoFound.toArray(new _Log[0]);
				}

				@Override
				protected void preface() {
					Debug.event( "LogFound...");
				}

				@Override
				protected void inform(EventListener l) {
					_Log[] logs = (_Log[]) payload[1];
					((LogsFound) l).logsFound(payload[0], logs);
				}
				
			});
		}
	}
	
	public static interface SessionAdded extends EventListener {
		public void sessionAdded(Object source, Session session);
	}
	
	public static final class GSessionAdded {
		public static void generate(final Object source, final Session session) {
			Center.addEvent(new SimpleForEach(SessionAdded.class) {
				@Override
				protected void preface() {
					Debug.event( "SessionAdded from %s (%s)", source, session);
				}
				
				@Override
				protected void inform(EventListener l) {
					((SessionAdded) l).sessionAdded(source, session);
				}
			});
		}
	}
	
	public static interface RelevantRobotStats extends EventListener {
		public void relRobotStats(Object source, RobotStats bs);
	}
	
	public static final class GRelevantRobotStats {
		public static void generate(final Object source, final RobotStats bs) {
			Center.addEvent(new SimpleCombine(RelevantRobotStats.class, source, bs){
				@Override
				protected void combine(LinkedList<ToolEvent> others) {
					if (others.isEmpty())
						return;
					
					/* other events can only have been posted later, so drop them. */
					Debug.event( "RelevantRobotStats dropping %d others.", others.size());
					
					ToolEvent te = others.getLast();
					assert(te instanceof SimpleCombine);
					SimpleCombine sc = (SimpleCombine) te;
					payload[0] = sc.payload[0];
					payload[1] = sc.payload[1];
				}

				@Override
				protected void preface() {
					String bst = bs.toString();
					Debug.event( "RelevantRobotStats{%s}", bst.length() > 50 ? bst.substring(0, 50) : bst);
				}

				@Override
				protected void inform(EventListener l) {
					((RelevantRobotStats) l).relRobotStats(payload[0], (RobotStats) payload[1]);
				}
				
			});
		}
	}
	
	public static interface ToolStats extends EventListener {
		public void toolStats(Object source, nbtool.data.ToolStats s);
	}
	
	public static final class GToolStats {
		public static void generate(final Object source, final nbtool.data.ToolStats ts) {
			Center.addEvent(new SimpleCombine(ToolStats.class, source, ts){
				@Override
				protected void combine(LinkedList<ToolEvent> others) {
					if (others.isEmpty())
						return;
					
					/* other events can only have been posted later, so drop them. */
					Debug.event( "ToolStats dropping %d others.", others.size());
					
					ToolEvent te = others.getLast();
					assert(te instanceof SimpleCombine);
					SimpleCombine sc = (SimpleCombine) te;
					payload[0] = sc.payload[0];
					payload[1] = sc.payload[1];
				}

				@Override
				protected void preface() {
					Debug.event( "ToolStats...");
				}

				@Override
				protected void inform(EventListener l) {
					((ToolStats) l).toolStats(payload[0], (nbtool.data.ToolStats) payload[1]);
				}
				
			});
		}
	}
	
	public static interface ToolStatus extends EventListener {
		public void toolStatus(Object source, STATUS s, String desc);	
	}
	
	public static final class GToolStatus {
		public static void generate(final Object source, final STATUS s, final String desc) {
			Center.addEvent(new SimpleForEach(ToolStatus.class) {

				@Override
				protected void preface() {
					Debug.event( "ToolStatus %s %s", s, desc);
				}

				@Override
				protected void inform(EventListener l) {
					((ToolStatus) l).toolStatus(source, s, desc);
				}
				
			});
		}
	}
	
	public static interface LogSelected extends EventListener {
		public void logSelected(Object source, _Log first, ArrayList<_Log> alsoSelected);
	}
	
	public static final class GLogSelected {
		public static void generate(Object source, _Log first, ArrayList<_Log> alsoSelected) {
			Center.addEvent(new SimpleCombine(LogSelected.class, source, first, alsoSelected){

				@Override
				protected void combine(LinkedList<ToolEvent> others) {
					if (others.isEmpty())
						return;
					
					/* other events can only have been posted later, so drop them. */
					Debug.event( "LogSelected dropping %d others.", others.size());
					
					if (others.isEmpty())
						return;
					
					ToolEvent te = others.getLast();
					assert(te instanceof SimpleCombine);
					SimpleCombine sc = (SimpleCombine) te;
					payload[0] = sc.payload[0];
					payload[1] = sc.payload[1];
					payload[2] = sc.payload[2];
				}

				@Override
				protected void preface() {
					Debug.event( "LogSelected{%s}", payload[0]);
				}

				@Override
				protected void inform(EventListener l) {
					((LogSelected) l).logSelected(payload[0], (_Log) payload[1], (ArrayList<_Log>) payload[2]);
				}
				
			});
		}
	}
	
	public static interface SessionSelected extends EventListener {
		public void sessionSelected(Object source, Session s);
	}
	
	public static final class GSessionSelected {
		public static void generate(final Object source, final Session s) {
			Center.addEvent(new SimpleCombine(SessionSelected.class, source, s){

				@Override
				protected void combine(LinkedList<ToolEvent> others) {
					if (others.isEmpty())
						return;
					
					/* other events can only have been posted later, so drop them. */
					Debug.event( "SessionSelected dropping %d others.", others.size());
					
					if (others.isEmpty())
						return;
					
					ToolEvent te = others.getLast();
					assert(te instanceof SimpleCombine);
					SimpleCombine sc = (SimpleCombine) te;
					payload[0] = sc.payload[0];
					payload[1] = sc.payload[1];
				}

				@Override
				protected void preface() {
					Debug.event( "SessionSelected{%s}", payload[0]);
				}

				@Override
				protected void inform(EventListener l) {
					((SessionSelected) l).sessionSelected(payload[0], (Session) payload[1]);
				}
				
			});
		}
	}
	
	public static interface ViewProfileSetChanged extends EventListener {
		//Get new profiles via ViewProfile.PROFILES
		public void viewProfileSetChanged(Object changer);
	}
	
	public static final class GViewProfileSetChanged {
		public static void generate(final Object source) {
			Center.addEvent(new SimpleCombine(ViewProfileSetChanged.class, source) {
				@Override
				protected void combine(LinkedList<ToolEvent> others) {
					if (others.isEmpty())
						return;
					
					SimpleCombine sc = (SimpleCombine) others.getLast();
					payload[0] = sc.payload[0];
				}

				@Override
				protected void preface() {
					Debug.event( "ViewProfileSetChanged{%s}", payload[0]);
				}

				@Override
				protected void inform(EventListener l) {
					((ViewProfileSetChanged) l).viewProfileSetChanged(payload[0]);
				}
			});
		}
	}
}
