package nbtool.test;

import nbtool.util.P;
import nbtool.util.U;

public class Test {
	
	public abstract class Abs {
		public final int size = 0;
		
		public int retSize() {return size;}
	}
	
	public class Ex extends Abs {
		public final int size = 5;
	}

	
	public static void main(String[] args) {
		
		String a = "hello";
		
		U.wf("%20s", a);
		
	}
}
