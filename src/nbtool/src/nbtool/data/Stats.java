package nbtool.data;

import nbtool.util.N;
import nbtool.util.N.EVENT;
import nbtool.util.N.NListener;
import nbtool.util.U;

public class Stats implements NListener{
	public static final Stats INST = new Stats();
	private Stats() {
		l_found = 0;
		db_found = db_cur = db_dropped = 0;
		
		N.listen(EVENT.LOG_LOAD, this);
		N.listen(EVENT.LOG_FOUND, this);
		N.listen(EVENT.LOG_DROP, this);
	}	
	
	public int l_found;
	public long db_found, db_cur, db_dropped;
	
	public void notified(EVENT e, Object src, Object... args) {
				
		switch (e) {
		case LOG_LOAD:{
			Log[] logs = (Log[]) args;
			for (Log l : logs) {
				db_found += l.bytes.length;
				db_cur += l.bytes.length;
			}
		}
			break;
		case LOG_FOUND: {
			Log[] logs = (Log[]) args;
			for (Log l : logs) {
				++l_found;
				
				if (l.bytes != null) {
					db_found += l.bytes.length;
					db_cur += l.bytes.length;
				}
			}
		}
			break;
		case LOG_DROP: {
			assert(args.length == 2);
			Long b = (Long) args[1];
			
			db_dropped += b;
			db_cur -= b;
		}
			break;
			
			default:
				U.w("STATS notified of NON-LISTENED EVENT!");
				return;
		}
		
		N.notifyEDT(EVENT.STATS, this);
	}
}
