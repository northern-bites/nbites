package nbclient.test;

import nbclient.util.P;
import nbclient.util.U;

public class Test {
	
	public abstract class Abs {
		public final int size = 0;
		
		public int retSize() {return size;}
	}
	
	public class Ex extends Abs {
		public final int size = 5;
	}

	
	public static void main(String[] args) {
		
		Object a = P.CLASS_EXCEPTIONS_MAP;
		Object b = P.LTVIEW_MAP;
		
	}
}
