package nbtool.gui.logviews.images;

import java.util.HashMap;
import java.util.Map;

import nbtool.data.Log;

public class Testable {
	Log log;
	
	public Testable(Log log_) {
		log = log_;
	}
	
	public Map<String, Double> calculateError() throws IllegalArgumentException, IllegalAccessException {
		HashMap<String, Double> error = new HashMap<String, Double>();
		Map<String, String> attributes = log.getAttributes();
		
		java.lang.reflect.Field[] fields = getClass().getDeclaredFields();
		for (java.lang.reflect.Field f : fields) {
			String valueInDescAsString = attributes.get(f.getName());
			if (valueInDescAsString != null) {
				int valueInDesc = Integer.parseInt(valueInDescAsString);
				Class<?> t = f.getType();
				Object v = f.get(this);
				int valueInClass = ((Number) v).intValue(); 
				double percentDiff = (double)Math.abs(valueInClass - valueInDesc) / valueInDesc;
				error.put(f.getName(), percentDiff);
			}
		}
			   
		return error;
	}
}