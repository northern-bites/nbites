package nbtool.util.test;

import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.Map.Entry;
import java.util.Vector;

import nbtool.data.json.Json;
import nbtool.util.Debug;
import nbtool.util.Debug.DebugSettings;

public class Tests {
	
	private static DebugSettings debug = Debug.createSettings(true,true,true,Debug.EVENT, "TESTS");
	
	public static final String default_section = "default_section";
	public static final LinkedHashMap<String, Vector<TestBase>> tests = new LinkedHashMap<>();
	
	public static TestBase add(TestBase test) {
		return add(default_section, test);
	}
	
	public static TestBase add(String section, TestBase test) {
		debug.info("adding test [%s] to section [%s]", test.testingFor, section);
		if (!tests.containsKey(section)) {
			tests.put(section, new Vector<TestBase>());
		}
		
		tests.get(section).add(test);
		return test;
	}
	
	/* need to reference classes where we add tests to ensure they're static-initialized */
	static {
		Json.addTests();
	}
	
	public static boolean run(String section) {
		if (!tests.containsKey(section)) {
			debug.error("no tests registered for section: %s", section);
			return false;
		}
		
		debug.printf("running tests in section: %s", section);
		
		for (TestBase t : tests.get(section)) {
			try {
				debug.warn("running [%s]{'%s' from %s}", section, t.testingFor, t.whereFrom);
				assert(t.testBody());
			} catch (Exception e) {
				debug.error("failed [%s]{'%s' from %s}", section, t.testingFor, t.whereFrom);
				e.printStackTrace();
				return false;
			}
		}
		
		return true;
	}
	
	public static boolean runAll() {
		debug.printf("running all tests");
		
		for (String section : tests.keySet()) {
			assert(run(section));
		}
		
		return true;
	}
}
