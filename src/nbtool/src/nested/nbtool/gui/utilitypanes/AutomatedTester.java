// package nbtool.gui.utilitypanes;

// import java.lang.reflect.InvocationTargetException;
// import java.util.ArrayList;
// import java.util.HashMap;
// import java.util.Map;
// import java.util.Map.Entry;

// import nbtool.data.Log;
// import nbtool.data.Session;
// import nbtool.data.SessionMaster;
// import nbtool.gui.logviews.images.Detector;
// import nbtool.gui.logviews.images.Testable;

// // TODO fix non loaded log crash
// // TODO ambiguous post problems
// // TODO refactor vision, post, and testing system into packages
// public class AutomatedTester {	
// 	int falsePositives;
// 	int falseNegatives;
// 	Map<Log, Map<String, Double>> logsToPercentErrors;
	
// 	public AutomatedTester(ArrayList<String> attributesInDescString, Map<String, Double> tolerance, Class<?> detectorClass) {
// 		// TODO handle empty session
// 		Session session = SessionMaster.INST.sessions.get(SessionMaster.INST.sessions.size()-1);
		
// 		ArrayList<Log> logsToTest = new ArrayList<Log>();
// 		for (Log log : session.logs_DO) {
// 			Map<String, String> attributes = log.getAttributes();
// 			for (String attribute : attributesInDescString) {
// 				if (attributes.get(attribute) != null) {
// 					logsToTest.add(log);
// 				}
// 			}
// 		}
		
// 		logsToPercentErrors = new HashMap<Log, Map<String, Double>>();
// 		for (Log log : logsToTest) {
// 			try {
// 				Detector detector = (Detector) detectorClass.getConstructor(Log.class).newInstance(log);
// 				Map<String, Testable> detections = detector.getDetections();
// 				Map<String, Double> errors = new HashMap<String, Double>();
// 				for (Entry<String, Testable> detectionEntry : detections.entrySet()) {
// 					Testable detection = detectionEntry.getValue();
// 					detection.calculateError(detectionEntry.getKey(), tolerance);
// 					if (detection.falsePositive) falsePositives++;
// 					if (detection.falseNegative) falseNegatives++;
// 					for (Entry<String, Double> errorEntry : detection.percentErrors.entrySet()) {
// 						errors.put(detectionEntry.getKey().concat(errorEntry.getKey()), errorEntry.getValue());
// 					}
// 				}
// 				logsToPercentErrors.put(log, errors);
// 			} catch (InvocationTargetException e) {
// 				e.printStackTrace();
// 			} catch (NoSuchMethodException e) {
// 				e.printStackTrace();
// 			} catch (SecurityException e) {
// 				e.printStackTrace();
// 			} catch (InstantiationException e) {
// 				e.printStackTrace();
// 			} catch (IllegalAccessException e) {
// 				e.printStackTrace();
// 			} catch (IllegalArgumentException e) {
// 				e.printStackTrace();
// 			}
// 		}
// 	}
// }