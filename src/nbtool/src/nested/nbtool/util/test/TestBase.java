package nbtool.util.test;

import java.io.File;
import java.io.InputStream;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;

import nbtool.util.Debug;
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
	
	public static Path resourcePathAtClass(Object classInst, String resourceName) {
		URL url = classInst.getClass().getResource(resourceName);
		try {
			return (new File(url.toURI())).toPath();
		} catch (URISyntaxException e) {
			e.printStackTrace();
			return null;
		}
	}
	
	public static void requireEqual(Object a, Object b) {
		if (a.equals(b)) {
			return;
		} else {
			Debug.error("! {%s} equals {%s} ", a, b);
			assert(a.equals(b));
		}
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
	
	public static void _NBL_ADD_TESTS_() {
		Tests.add(new TestBase("resourceAtClass"){

			@Override
			public boolean testBody() throws Exception {
				InputStream is = TestBase.resourceAtClass(this, "resourceAtClass-testFile");
				byte first = (byte) is.read();
				
				assert(first == 'T');
				
				return true;
			}
			
		});
		
		Tests.add(new TestBase("resourceAtPath"){

			@Override
			public boolean testBody() throws Exception {
				Path testPath = TestBase.resourcePathAtClass(this, "resourceAtClass-testFile");
				assert(Files.exists(testPath));
				assert(testPath.endsWith("nbtool/util/test/resourceAtClass-testFile"));
							
				return true;
			}
			
		});
	}
}
