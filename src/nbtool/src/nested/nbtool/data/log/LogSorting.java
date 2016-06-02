package nbtool.data.log;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class LogSorting {
	
	public static enum Sort {
		
		BY_TIME("by creation time", new Comparator<LogReference>(){

			public int compare(LogReference o1, LogReference o2) {
				Long s1 = o1.createdWhen;
				Long s2 = o2.createdWhen;
				return s1.compareTo(s2);
			}
			
		}),
		
		BY_CLASS("by log class", new Comparator<LogReference>(){

			public int compare(LogReference o1, LogReference o2) {
				String s1 = o1.logClass;
				String s2 = o2.logClass;
				return s1.compareTo(s2);
			}
			
		}),
		
		BY_ARRIVAL("by arrival", new Comparator<LogReference>(){

			public int compare(LogReference o1, LogReference o2) {
				Long s1 = o1.thisID;
				Long s2 = o2.thisID;
				return s1.compareTo(s2);
			}
			
		})
		
		
		;
		
		public Comparator<LogReference> comparator;
		public String guiName;
		private Sort(String n, Comparator<LogReference> cmp) {
			this.comparator = cmp; this.guiName = n;
		}
		
		@Override
		public String toString() {
			return guiName;
		}
	}

	public static void sort(Sort s,
			List<LogReference> lr) {
		Collections.sort(lr, s.comparator);
	}
}
