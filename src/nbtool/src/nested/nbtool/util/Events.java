package nbtool.util;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

import javax.swing.SwingUtilities;

import nbtool.data.group.Group;
import nbtool.data.log.Log;
import nbtool.data.log.LogReference;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.nio.RobotConnection;
import nbtool.util.Center.ToolEvent;

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
	public static interface RobotConnectionStatus extends EventListener {
		public void robotStatus(RobotConnection inst, boolean up);
	}
	
	public static final class GRobotConnectionStatus {
		public static void generate(final RobotConnection inst, final boolean up) {
			Center.addEvent(new SimpleForEach(RobotConnectionStatus.class) {

				@Override
				protected void preface() {
					Debug.event( "StreamIOStatus: %s %b",
							inst.name(), up);
				}

				@Override
				protected void inform(EventListener l) {
					((RobotConnectionStatus) l).robotStatus(inst, up);
				}

			});
		}
	}
	
	public static interface LogRefsFound extends EventListener {
		public void logRefsFound(Object source, LogReference ... found);
	}
	
	public static final class GLogRefsFound {
		public static void generate(final Object source, final LogReference ... found) {
			Center.addEvent(new SimpleCombine(LogRefsFound.class, source, found){

				@Override
				protected void combine(LinkedList<ToolEvent> others) {
					assert(this.payload.length == 2);
					if (others.size() == 0)
						return;
					
					LogReference[] ours = (LogReference[]) payload[1];
					List<LogReference> alsoFound = new ArrayList<>();
					alsoFound.addAll(Arrays.asList(ours));
					
					for (ToolEvent te : others) {
						assert(te instanceof SimpleCombine);
						
						LogReference[] theirs = (LogReference[]) ((SimpleCombine) te).payload[1];
						alsoFound.addAll(Arrays.asList(theirs));
					}
					
					Debug.info( "LogReferenceFound combined %d events with %d logs.", others.size(), alsoFound.size());
					
					this.payload[0] = null;
					this.payload[1] = alsoFound.toArray(new LogReference[0]);
				}

				@Override
				protected void preface() {
					Debug.event( "LogReferenceFound...");
				}

				@Override
				protected void inform(EventListener l) {
					LogReference[] logs = (LogReference[]) payload[1];
					((LogRefsFound) l).logRefsFound(payload[0], logs);
				}
				
			});
		}
	}
	
	public static interface LogsFound extends EventListener {
		public void logsFound(Object source, Log ... found);
	}
	
	public static final class GLogsFound {
		public static void generate(final Object source, final Log ... found) {
			Center.addEvent(new SimpleCombine(LogsFound.class, source, found){

				@Override
				protected void combine(LinkedList<ToolEvent> others) {
					assert(this.payload.length == 2);
					if (others.size() == 0)
						return;
					
					Log[] ours = (Log[]) payload[1];
					List<Log> alsoFound = new ArrayList<>();
					alsoFound.addAll(Arrays.asList(ours));
					
					for (ToolEvent te : others) {
						assert(te instanceof SimpleCombine);
						
						Log[] theirs = (Log[]) ((SimpleCombine) te).payload[1];
						alsoFound.addAll(Arrays.asList(theirs));
					}
					
					Debug.info( "LogFound combined %d events with %d logs.", others.size(), alsoFound.size());
					
					this.payload[0] = null;
					this.payload[1] = alsoFound.toArray(new Log[0]);
				}

				@Override
				protected void preface() {
					Debug.event( "LogFound...");
				}

				@Override
				protected void inform(EventListener l) {
					Log[] logs = (Log[]) payload[1];
					((LogsFound) l).logsFound(payload[0], logs);
				}
			});
		}
	}
	
	public static interface GroupAdded extends EventListener {
		public void groupAdded(Object source, Group group);
	}
	
	public static final class GGroupAdded {
		public static void generate(final Object source, final Group group) {
			Center.addEvent(new SimpleForEach(GroupAdded.class) {
				@Override
				protected void preface() {
					Debug.event( "GroupAdded from %s (%s)", source, group);
				}
				
				@Override
				protected void inform(EventListener l) {
					((GroupAdded) l).groupAdded(source, group);
				}
			});
		}
	}
	
	public static interface LogSelected extends EventListener {
		public void logSelected(Object source, Log first, List<Log> alsoSelected);
	}
	
	public static final class GLogSelected {
		public static void generate(Object source, Log first, List<Log> alsoSelected) {
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
					((LogSelected) l).logSelected(payload[0], (Log) payload[1], (List<Log>) payload[2]);
				}
				
			});
		}
	}
	
	public static interface GroupSelected extends EventListener {
		public void groupSelected(Object source, Group s);
	}
	
	public static final class GGroupSelected{
		public static void generate(final Object source, final Group s) {
			Center.addEvent(new SimpleCombine(GroupSelected.class, source, s){

				@Override
				protected void combine(LinkedList<ToolEvent> others) {
					if (others.isEmpty())
						return;
					
					/* other events can only have been posted later, so drop them. */
					Debug.event( "GroupSelected dropping %d others.", others.size());
					
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
					Debug.event( "GroupSelected{%s}", payload[0]);
				}

				@Override
				protected void inform(EventListener l) {
					((GroupSelected) l).groupSelected(payload[0], (Group) payload[1]);
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
