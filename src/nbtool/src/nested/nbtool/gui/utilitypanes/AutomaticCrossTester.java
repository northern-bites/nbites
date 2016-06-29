package nbtool.gui.utilitypanes;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import nbtool.data.group.AllGroups;
import nbtool.data.group.Group;
import nbtool.data.log.Log;
import nbtool.data.log.LogReference;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CommonIO.IOState;
import nbtool.nio.CrossServer;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.util.Center;
import nbtool.util.Debug;

public class AutomaticCrossTester <K, R> {

	private static final Debug.DebugSettings debug = Debug.createSettings(Debug.INFO);

	public static interface TestInterface <K,R> {

		public void notifyTestStart();
		public String testName();

		/* should this reference be used, and if so under what category. */
		/* if return == null, should not be used */
		public K qualifiesForTesting(LogReference logr);

		public String crossFunctionName();

		//true if the test should continue with this log
		public boolean finishAnnotations(K key, Log log);

		public R getResult(LogReference from, K key, Log ... out );

		public boolean createGroupForResult(R resultType);
		public String groupNameFor(R resultType);

		public void notifyTestFinished();
	}

	private final AutomaticCrossTester<K,R> outerThis = this;
	private final TestInterface<K,R> theInterface;

	public AutomaticCrossTester(TestInterface<K,R> theInterface) {
		this.theInterface = theInterface;
	}

	private IOState state = IOState.STARTING;

	public boolean running() { return state == IOState.RUNNING; }
	public boolean finished() { return state == IOState.FINISHED; }

	private final synchronized void finish() {
		state = IOState.FINISHED;
		theInterface.notifyTestFinished();
	}

	private final Map<K, LinkedList<LogReference>> qualified = new HashMap<>();

	private static <K2, V> void insert(Map<K2, LinkedList<V>> map, K2 key, V value)  {
		synchronized(map) {
			if ( map.containsKey(key) ) {
				map.get(key).add(value);
			} else {
				LinkedList<V> list = new LinkedList<>();
				list.add(value);

				map.put(key, list);
			}
		}
	}

	private CrossInstance theInstance = null;

	public final synchronized void runTests() {

		if (state != IOState.STARTING) {
			throw new RuntimeException("cannot start from state: " + state);
		}

		debug.info("starting test of: %s", theInterface.testName());
		state = IOState.RUNNING;

		for (Group group : AllGroups.allGroups) {
			for (LogReference ref : group.logs) {

				K key = theInterface.qualifiesForTesting(ref);

				if (key != null) {
					insert(qualified, key, ref);
				}
			}
		}

		debug.info("found %d cases:", qualified.size());
		for (K key : qualified.keySet()) {
			debug.info("\t%s: %d", key.toString(), qualified.get(key).size());
		}

		theInstance = CrossServer.instanceByIndex(0);

		if (theInstance == null) {
			debug.error("cannot run test '%s' without NBCross instance!", theInterface.testName());
			this.finish();
			return;
		}

		Center.addEvent(new Center.EventRunnable() {
			@Override
			protected void run() {
				add_tests();
			}
		});
	}

	private int outstanding_tests = 0;
	private final Map<R, LinkedList<LogReference>> results = new HashMap<>();

	private class TestResponder implements IOFirstResponder {

		TestResponder(LogReference r, K k) {
			this.from = r; this.key = k;
		}

		LogReference from;
		K key;

		@Override
		public void ioFinished(IOInstance instance) {
			if (state == IOState.RUNNING) debug.error("test-%s failed when nbcross died!",
				theInterface.testName() );
		}

		@Override
		public void ioReceived(IOInstance inst, int ret, Log... out) {
			R result = theInterface.getResult(from, key, out);

			if (result != null) {
				insert(results, result, from);
			} else {
				debug.warn("null result {input had key %s}", key.toString());
			}

			synchronized(outerThis) {
				--outstanding_tests;
				if (outstanding_tests == 0) {
					debug.info("TestResponder sees testing finished!");

					Center.addEvent(new Center.EventRunnable() {
						@Override
						protected void run() {
							ship_it();
						}
					});
				}
			}
		}

		@Override
		public boolean ioMayRespondOnCenterThread(IOInstance inst) {
			return true;
		}
	}

	public void add_tests() {
//		debug.warn("%s starting!", Thread.currentThread().getName());
		debug.warn("adding tests for %s", theInterface.testName());

		synchronized(this) {
			for (K key : qualified.keySet()) {
				List<LogReference> list = qualified.get(key);

				for (LogReference ref : list) {
					Log arg = ref.get();
					if (theInterface.finishAnnotations(key, arg)) {
						theInstance.tryAddCall(new TestResponder(ref, key), theInterface.crossFunctionName(), arg);
						++outstanding_tests;
					}
				}
			}
		}
	}

	public void ship_it() {
		debug.warn("wrapping up %s", theInterface.testName());

		for (R resultType : results.keySet()) {
			if (theInterface.createGroupForResult(resultType)) {

				String name = theInterface.groupNameFor(resultType);
				Group group = AllGroups.request(name);

				group.logs.addAll(results.get(resultType));


			} else {
				debug.warn("ignoring results of type %s", resultType);
			}
		}

		this.finish();
	}
}













