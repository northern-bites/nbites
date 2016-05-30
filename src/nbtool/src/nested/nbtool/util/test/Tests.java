package nbtool.util.test;

import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.Map.Entry;
import java.util.Vector;

import nbtool.data.json.Json;
import nbtool.data.log.Log;
import nbtool.util.ClassFinder;
import nbtool.util.Debug;
import nbtool.util.Debug.DebugSettings;
import nbtool.util.ToolSettings;

public class Tests {
	
	private static DebugSettings debug = Debug.createSettings(true,true,true,Debug.EVENT, "TESTS");
	
	public static final String default_section = "default_section";
	public static final LinkedHashMap<String, Vector<TestBase>> allTests = new LinkedHashMap<>();
	
	public static boolean add(TestBase ... tests) {
		return add(default_section, tests);
	}
	
	public static boolean add(String section, TestBase ... tests) {
		
		for (TestBase test : tests) {
			debug.info("adding test [%s] to section [%s]", test.testingFor, section);
			if (!allTests.containsKey(section)) {
				allTests.put(section, new Vector<TestBase>());
			}
			
			allTests.get(section).add(test);
		}
		
		return true;
	}
	
	public static void findAllTests() {
		ClassFinder.callAllInstancesOfStaticMethod(ToolSettings.staticAddTestsMethodName);
		assert(ClassFinderTest.found);
	}
	
	private static void test_break() {
		Debug.set_yellow(); Debug.dbreak(""); Debug.lbreak(); Debug.set_reset();
	}
	
	private static void test_break2() {
		Debug.set_cyan(); Debug.dbreak(""); Debug.lbreak(); Debug.set_reset();
	}
	
	public static boolean run(String section) {
		if (!allTests.containsKey(section)) {
			debug.error("no tests registered for section: %s", section);
			return false;
		}
		
		debug.warn("  running section <%s>", section);
		test_break();
		
		for (TestBase t : allTests.get(section)) {
			try {
				debug.warn("    running [%s]", t.testingFor );
				debug.warn("        from [%s]", t.whereFrom);
				assert(t.testBody());
				debug.info("    [passed]");
				Debug.lbreak();
			} catch (Throwable e) {
				debug.error("   !failed! [%s]{'%s' from %s}", section, t.testingFor, t.whereFrom);
				e.printStackTrace();
				return false;
			}
		}
		
		return true;
	}
	
	public static boolean runAll() {
		Debug.lbreak();
		debug.warn("\trunning all tests");
		test_break2();
		
		for (String section : allTests.keySet()) {
			if (!run(section)) {
				debug.error("< TESTING FAILED >");
				return false;
			}
		}
		
		debug.warn("done");
		test_break2();
		
		return true;
	}
}
