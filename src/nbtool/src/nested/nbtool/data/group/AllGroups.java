package nbtool.data.group;

import java.util.ArrayList;

import nbtool.data.log.Log;
import nbtool.util.Center;
import nbtool.util.Events;

public class AllGroups {
	
	public static final ArrayList<Group> allGroups = new ArrayList<>();
	
	protected static void add(Group g) {
		synchronized(allGroups) {
			allGroups.add(g);
		}
	}
	
	public static void remove(Group g) {
		synchronized(allGroups) {
			allGroups.remove(g);
		}
	}
	
	public static int getGroupCount() {
		return allGroups.size();
	}
	
	public static Group latestGroup() {
		int size = allGroups.size();
		return size > 0 ? allGroups.get(size - 1) : null;
	}
	
	public static Group get(int i) {
		return allGroups.get(i);
	}
	
	public static int getLogCount() {
		return log_count;
	}
	
	private static int log_count;
	
	private static Counter counter = null;
	
	public static void _NBL_REQUIRED_START_ () {
		if (counter == null) {
			counter = new Counter();
		}
	}
	
	private static class Counter implements Events.LogsFound {
		protected Counter() {
			Center.listen(Events.LogsFound.class, this, false);
		}

		@Override
		public void logsFound(Object source, Log... found) {
			log_count += found.length;
		}
	}
	
	private AllGroups(){}
}
