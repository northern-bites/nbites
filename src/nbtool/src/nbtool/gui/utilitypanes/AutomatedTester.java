package nbtool.gui.utilitypanes;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import nbtool.data.Log;
import nbtool.data.Session;
import nbtool.data.SessionMaster;
import nbtool.gui.logviews.images.Detector;

// TODO fix non loaded log crash
// TODO false positive counting
// TODO ambiguous post problems
// TODO refactor vision, post, and testing system into packages
public class AutomatedTester {	
	Map<Log, Map<String, Double>> results;
	int falsePositives;
	int falseNegatives;
	
	public AutomatedTester(ArrayList<String> attributesInDescString, Map<String, Double> tolerance, Class<?> detectorClass) {
		// TODO handle empty session
		Session session = SessionMaster.INST.sessions.get(SessionMaster.INST.sessions.size()-1);
		
		ArrayList<Log> logsToTest = new ArrayList<Log>();
		for (Log log : session.logs_DO) {
			Map<String, String> attributes = log.getAttributes();
			for (String attribute : attributesInDescString) {
				if (attributes.get(attribute) != null) {
					logsToTest.add(log);
				}
			}
		}
		
		results = new HashMap<Log, Map<String, Double>>();
		Constructor<?>[] clist = detectorClass.getConstructors();
		for (Log log : logsToTest) {
			try {
				Detector detector = (Detector) clist[0].newInstance(log);
				Map<String, Double> error = detector.getDetection().calculateError();
				results.put(log, error);
				for (Entry<String, Double> attributeError : error.entrySet()) {
					if (attributeError.getValue() > tolerance.get(attributeError.getKey()))
						falseNegatives++;
				}
			} catch (InstantiationException e) {
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			} catch (IllegalArgumentException e) {
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				e.printStackTrace();
			}
		}
	}
}