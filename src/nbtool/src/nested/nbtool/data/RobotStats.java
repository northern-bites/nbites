package nbtool.data;

import java.io.DataInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import nbtool.data._log._Log;
import nbtool.util.Utility;

public class RobotStats {
	
	private SExpr tree = null;
	
	public RobotStats(_Log log) {
		assert(log != null);
		tree = log.tree();
		assert(tree != null);
		
		assert(tree.get(0).exists() && tree.get(0).value().equals("nblog"));
		
		SExpr c1 = tree.find("contents").get(1);
		assert(c1.find("type").get(1).value().equals("STATS"));
		SExpr flagsExpr = c1.find("flags");
		
		flags = new ArrayList<Flag>();
		
		for (int i = 1; i < flagsExpr.count(); ++i) {
			SExpr aFlag = flagsExpr.get(i);
			assert(aFlag.exists() && aFlag.count() == 3);
			
			String name = aFlag.get(0).value();
			int index = aFlag.get(1).valueAsInt();
			boolean value = aFlag.get(2).valueAsBoolean();
			
			flags.add(new Flag(name, index, value));
		}
	}
	
	@Override
	public String toString() {
		return tree.print();
	}
	
	public ArrayList<Flag> flags = null;
	
	public class Flag {
		public String name;
		public int index;
		public boolean value;
		
		public Flag(String n, int i, boolean v) {
			name = n;
			index = i;
			value = v;
		}
	}
}
