package nbtool.data.group;

import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import nbtool.data.log.LogReference;
import nbtool.util.Utility;

public class Group {
	
	private final long unique_id = Utility.getNextIndex(this);
	
	public final ArrayList<LogReference> logs = new ArrayList<>();
	
	public void add(LogReference ...logReferences ) {
		this.add(Arrays.asList(logReferences));
	}
	
	/* this method takes ownership of the logrefs.  if you do not wish this, access logs directly */
	public void add(List<LogReference> logReferences) {
		synchronized(logs) {
			for (LogReference lr : logReferences) {
				logs.add(lr);
				lr.container = this;
			}
		}
	}
	
	public void remove(LogReference ...logReferences) {
		this.remove(Arrays.asList(logReferences));
	}
	
	public void remove(List<LogReference> logReferences) {
		synchronized(logs) {
			for (LogReference lr : logReferences) {
				if (logs.remove(lr)) {
					lr.container = null;
				}
			}
		}
	}
	
	public enum GroupSource {
		UNDEFINED,
		ROBOT_STREAM,
		FILESYSTEM
	};
	
	public static final GroupSource UNDEFINED = GroupSource.UNDEFINED;
	public static final GroupSource ROBOT_STREAM = GroupSource.ROBOT_STREAM;
	public static final GroupSource FILESYSTEM = GroupSource.FILESYSTEM;
	
	public final GroupSource source;
	private final String info;
	
	public String getGroupSourceAddress() {
		return source == ROBOT_STREAM ? info : null;
	}
	
	public Path getGroupSourcePath() {
		return source == FILESYSTEM ? Paths.get(info) : null;
	}
	
	public String getGroupInfo() {
		return info;
	}
	
	private Group(GroupSource src, String info) {
		this.source = src; this.info = info;
	}
	
	public static Group groupForStream(String robotAddress) {
		Group ret = new Group(ROBOT_STREAM, robotAddress);
		AllGroups.add(ret);
		return ret;
	}
	
	public static Group groupFromPath(Path path) {
		Group ret = new Group(FILESYSTEM, path.toString());
		AllGroups.add(ret);
		return ret;
	}
	
	public static Group groupNamed(String name) {
		Group ret = new Group(UNDEFINED, name);
		AllGroups.add(ret);
		return ret;
	}
	
	@Override
	public String toString() {
		return String.format("Group{i%d,#%d}(source:%s, info:%s)", unique_id, logs.size(), source.toString(), info);
	}
	
	public String guiString() {
		return String.format("group %d: (%d logs) source: %s: %s", 
				unique_id, logs.size(), source, info);
	}
}
