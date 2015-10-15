package nbtool.data;

import nbtool.data.log._Log;
import nbtool.util.Center;
import nbtool.util.Events;

public class ToolStats implements Events.LogLoaded, Events.LogsFound{
	public static final ToolStats INST = new ToolStats();
	private ToolStats() {
		l_found = 0;
		db_found = db_cur = db_dropped = 0;
		
		Center.listen(Events.LogLoaded.class, this, false);
		Center.listen(Events.LogsFound.class, this, false);
	}	
	
	public int l_found;
	public long db_found, db_cur, db_dropped;

	@Override
	public void logsFound(Object source, _Log... found) {
		for (_Log l : found) {
			++l_found;
			
			if (l.bytes != null) {
				db_found += l.bytes.length;
				db_cur += l.bytes.length;
			}
		}

		Events.GToolStats.generate(this, this);
	}

	@Override
	public void logLoaded(Object source, _Log... loaded) {
		for (_Log l : loaded) {
			db_found += l.bytes.length;
			db_cur += l.bytes.length;
		}
		
		Events.GToolStats.generate(this, this);
	}
}
