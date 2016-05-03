package nbtool.util.test;

import java.io.InputStream;

import nbtool.util.Utility;

public abstract class TestBase {
	
	protected String testingFor = null;
	protected String whereFrom = null;
	
	public abstract boolean testBody() throws Exception;
	
	public TestBase(String tf) {
		this.testingFor = tf;
		StackTraceElement trace = Utility.codeLocation(2);
		whereFrom = String.format("%s@%s:%d", trace.getClassName(), trace.getFileName(), trace.getLineNumber());	
	}
	
	public static InputStream resourceAtClass(Object classInst, String resourceName) {
		return classInst.getClass().getResourceAsStream(resourceName);
	}
	
	public static void requireEqual(Object a, Object b) {
		assert(a.equals(b));
	}
	
	public static void failed(String msg) throws TestFailedException {
		throw new TestFailedException(msg);
	}
	
	protected static class TestFailedException extends Exception {
		private static final long serialVersionUID = 1L;
		public TestFailedException(String msg) {
			super(msg);
		}
	}
}
