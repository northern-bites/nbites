package nbtool.util.test;

import java.io.IOException;

import nbtool.util.Debug;

public abstract class TestThrow<T extends Exception> extends TestBase {
	
	private Class<T> exception;
	private static final Debug.DebugSettings debug = 
			Debug.createSettings(true, true, true, Debug.INFO, null);

	public TestThrow(String tf, Class<T> exceptionClass) {
		super(tf);
		this.exception = exceptionClass;
	}

	@Override
	public boolean testBody() throws Exception {
		try {
			generateThrow();
		} catch (Exception except) {
			if (exception.isInstance(except))
				return true;
			else {
				debug.error("test '%s' generated exception %s, needed %s", this.testingFor,
						except.getClass().getName(), exception.getName());
				return false;
			}
		}
		
		debug.error("test '%s' needed exception %s but generated nothing.", 
				this.testingFor, exception.getName() );
		return false;
	}
	
	public abstract void generateThrow() throws T;
	
	public static void _NBL_ADD_TESTS_() {
		Tests.add("TestThrow", new TestBase("thrown"){

			@Override
			public boolean testBody() throws Exception {
				
				TestThrow<IOException> thrower = new TestThrow<IOException>("", IOException.class){
					@Override
					public void generateThrow() throws IOException {
						throw new IOException();
					}
				};
				
				debug.slevel = Debug.ALWAYS;
				assert(thrower.testBody());
				debug.slevel = Debug.WARN;
				
				return true;
			}
			
		}, new TestBase("not thrown"){

			@Override
			public boolean testBody() throws Exception {
				
				TestThrow<IOException> nt1 = new TestThrow<IOException>("", IOException.class){
					@Override
					public void generateThrow() throws IOException {
						throw new RuntimeException();
					}
				};
				
				debug.slevel = Debug.ALWAYS;
				assert(!nt1.testBody());
				debug.slevel = Debug.WARN;
				
				TestThrow<IOException> nt2 = new TestThrow<IOException>("", IOException.class){
					@Override
					public void generateThrow() throws IOException { }
				};
				
				debug.slevel = Debug.ALWAYS;
				assert(!nt2.testBody());
				debug.slevel = Debug.WARN;
				
				return true;
			}
			
		});
	}
}
