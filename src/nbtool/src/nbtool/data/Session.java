package nbtool.data;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;

import nbtool.util.Events;
import nbtool.util.Utility;

public class Session {
	
	private static final Object indexLock = new Object();
	private static long class_index = 0;
	private static long getID() {
		long ret;
		synchronized(indexLock) {
			ret = class_index++;
		}
		
		return ret;
	}
	public final long unique_id = getID();
	
	public String directoryFrom;
	public String name;
	
	public RobotStats most_relevant;
	
	public ArrayList<Log> logs_ALL; //Logs in the order they arrived.  Only grows, does not change order.
	public ArrayList<Log> logs_DO; //All the same logs in logs_TO, but in sorted order.
	
	protected Session(String dir, String addr) {
		logs_ALL = new ArrayList<Log>();
		logs_DO = new ArrayList<Log>();
		most_relevant = null;
		
		assert(dir != null || addr != null);
		assert(dir == null || addr == null);
		
		if (dir != null) {
			this.directoryFrom = dir;
			this.name = String.format("s%d loaded from %s", this.unique_id, dir);
		} else {
			this.directoryFrom = null;
			this.name = String.format("s%d streamed from %s", this.unique_id, addr);
		}
	}
	
	public void addLog(Log ... l) {
		logs_ALL.addAll(Arrays.asList(l));
		
		boolean newBS = false;
		for (Log log : l) {
			//System.out.println(log.description());
			if (log.primaryType().equals("STATS")) {
				this.most_relevant = new RobotStats(log);
				newBS = true;
			}
		}
		
		if (newBS) {
			Events.GRelevantRobotStats.generate(this, most_relevant);
		}
	}
	
	public String toString() {
		return String.format("(%d/%d) ", logs_DO.size(), logs_ALL.size()) + name;
	}
}
