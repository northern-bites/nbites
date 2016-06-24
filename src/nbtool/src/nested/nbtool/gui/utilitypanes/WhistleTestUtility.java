package nbtool.gui.utilitypanes;

import nbtool.data.log.Log;
import nbtool.data.log.LogReference;
import nbtool.gui.utilitypanes.AutomaticCrossTester.TestInterface;
import nbtool.util.Debug;

public class WhistleTestUtility extends AutomaticTestUtility<String, String> {

	private static final String HEARD = "contains whistle", NHEARD = "no whistle", ERROR_FP = "false positive", ERROR_FN = "false negative";

	private static boolean heard(LogReference ref) {
		return heard(ref.get());
	}

	private static boolean heard(Log log) {
		return log.topLevelDictionary.get("WhistleHeard").asBoolean().bool();
	}

	@Override
	public String purpose() {
		return "finds errors in whistle detection";
	}

	@Override
	public char preferredMemnonic() {
		return 'q';
	}

	@Override
	public TestInterface<String, String> getInterface() {

		return new TestInterface<String,String>(){

			@Override
			public void notifyTestStart() {}

			@Override
			public String testName() {
				return "WhistleDetection";
			}

			@Override
			public String qualifiesForTesting(LogReference logr) {

				if (logr.get().topLevelDictionary.containsKey("WhistleHeard")) {

					if (heard(logr)) {
						return HEARD;
					} else {
						return NHEARD;
					}

				} else {
					return null;
				}
			}

			@Override
			public String crossFunctionName() {
				return "whistle_detect";
			}

			@Override
			public void finishAnnotations(String key, Log log) { }

			@Override
			public String getResult(LogReference from, String key, Log... out) {
				assert(key == HEARD || key == NHEARD);

				boolean found = heard(out[0]);
				boolean should = key == HEARD;

				if (!found && !should) {
					Debug.print("++ whistle not heard!");
					return null;
				}

				if (found && should) {
					Debug.print("++ whistle heard!");
					return null;
				}

				if (!found && should) {
					Debug.print("-- whistle missed!");
					return ERROR_FN;
				}

				if (found && !should) {
					Debug.print("-- whistle mis-heard!");
					return ERROR_FP;
				}

				assert(false);
				return null;
			}

			@Override
			public boolean createGroupForResult(String resultType) {
				return true;
			}

			@Override
			public String groupNameFor(String resultType) {
				return resultType;
			}

			@Override
			public void notifyTestFinished() { }

		};
	}

}
