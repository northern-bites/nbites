// package nbtool.gui.logviews.images;

// import java.util.HashMap;
// import java.util.Map;

// import nbtool.data.Log;

// public class Testable {
// 	Log log;
// 	boolean set;
	
// 	public boolean falsePositive;
// 	public boolean falseNegative;
// 	public Map<String, Double> percentErrors;
	
// 	public Testable(Log log_) {
// 		log = log_;
// 	}
	
// 	public void calculateError(String attributePrefix, Map<String, Double> tolerance) throws IllegalArgumentException, IllegalAccessException {
// 		percentErrors = new HashMap<String, Double>();
// 		Map<String, String> attributes = log.getAttributes();
		
// 		java.lang.reflect.Field[] fields = getClass().getDeclaredFields();
// 		for (java.lang.reflect.Field f : fields) {
// 			String name = attributePrefix.concat(f.getName());
// 			String valueInDescAsString = attributes.get(name);
// 			if (valueInDescAsString != null) {
// 				if (!set) {
// 					falsePositive = true;
// 					continue;
// 				}
					
// 				int valueInDesc = Integer.parseInt(valueInDescAsString);
// 				Object v = f.get(this);
// 				int valueInClass = ((Number) v).intValue(); 
// 				double percentDiff = (double)Math.abs(valueInClass - valueInDesc) / valueInDesc;
// 				if (percentDiff > tolerance.get(f.getName())) {
// 					falsePositive = true;
// 					falseNegative = true;
// 				}
					
// 				percentErrors.put(f.getName(), percentDiff);
// 			} else if (tolerance.get(f.getName()) != null && set) {
// 				falseNegative = true;
// 			}
// 		}
// 	}
// }