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
	
	// *** MODIFIED ON CENTER THREAD ***
	//Logs in the order they arrived.  Only grows, does not change order.
	public final ArrayList<Log> logs_ALL = new ArrayList<>(); 
	
	// *** ONLY MODIFIED IN EDThread ***
	//Some subset of logs_ALL, in order specified by GUI.
	public ArrayList<Log> logs_DO; 
	
	/* Used in SessionMaster for specific purpose sessions */
	protected Session(String name) {
//		logs_ALL = new ArrayList<Log>();
		logs_DO = new ArrayList<Log>();
		most_relevant = null;
		
		directoryFrom = null;
		this.name = name;
	}
	
	/* Used in SessionMaster for stream/load sessions */
	protected Session(String dir, String addr) {
//		logs_ALL = new ArrayList<Log>();
		logs_DO = new ArrayList<Log>();
		most_relevant = null;
		
		/* assert(dir != null || addr != null); */
		assert(dir == null || addr == null);
		
		if (dir != null) {
			this.directoryFrom = dir;
			this.name = String.format("s%d loaded from %s", this.unique_id, dir);
		} else {
			this.directoryFrom = null;
			this.name = String.format("s%d streamed from %s", this.unique_id, addr);
		}
	}
	
	/*
	 * addLog for streaming – checks the log for STATS to update our information of the robot,
	 * then decides to keep the log based on <dropSTATS> and <keep>
	 * */
	public void addLog(Log l, boolean keep) {
		l.parent = this;
		
		if (l.primaryType().equals("STATS")) {
			this.most_relevant = new RobotStats(l);
			Events.GRelevantRobotStats.generate(this, most_relevant);
			
			if (SessionMaster.dropSTATS) return;
		}
		
		if (keep) {
			logs_ALL.add(l);
		}
	}
	
	/*
	 * addLog for FileIO loading.  Finds most recent STATS log in logs (if present) and
	 * sets it to most_relevant.
	 * */
	public void addLog(Log ... l) {
		logs_ALL.addAll(Arrays.asList(l));
		
		boolean newBS = false;
		for (Log log : l) {
			log.parent = this;

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
