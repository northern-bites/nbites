package nbtool.data;

import java.io.File;
import java.util.ArrayList;
import java.util.Comparator;

import nbtool.util.NBConstants.MODE;
import nbtool.util.U;

public class Session {
	public String dir;
	public String name;
	
	public BotStats most_relevant;
	
	public ArrayList<Log> logs_TO; //Logs in the order they arrived.  Only grows, does not change order.
	public ArrayList<Log> logs_DO; //All the same logs in logs_TO, but in sorted order.
	
	private String part = null;
	private Comparator<Log> cmp = null;
	
	protected Session(int i, MODE m, String p, String s) {
		logs_TO = new ArrayList<Log>();
		logs_DO = new ArrayList<Log>();
		most_relevant = null;
		dir = null;
				
		switch (m) {
		case FILESYSTEM:
			dir = U.localizePath(p) + File.separator;
			
			name = String.format("%d: FILESYSTEM(%s)", i, dir);
			break;
		case NETWORK_NOSAVE:
			name = String.format("%d: NET(%s)", i, s);
			break;
		case NETWORK_SAVING:
			name = String.format("%d: NET(%s) to FS(%s)", i, s, p);
			break;
		case NONE:
			break;
		default:
			break;
		
		}
	}
	
	public void addLog(Log l) {
		logs_TO.add(l);
		logs_DO.add(l);
	}
	
	public void sortWith(String prt, Comparator<Log> c) {
		
	}
	
	public String toString() {
		return name;
	}
}
