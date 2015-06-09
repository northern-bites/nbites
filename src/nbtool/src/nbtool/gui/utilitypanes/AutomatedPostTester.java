// package nbtool.gui.utilitypanes;

// import java.util.ArrayList;
// import java.util.HashMap;
// import java.util.Map;
// import java.util.Map.Entry;

// import javax.swing.JScrollPane;
// import javax.swing.JTextArea;

// import nbtool.data.Log;
// import nbtool.gui.logviews.images.PostDetector;

// public class AutomatedPostTester extends UtilityParent {
// 	AutomatedTester tester;
	
// 	public AutomatedPostTester() {
// 		ArrayList<String> postLabels = new ArrayList<String>();
// 		postLabels.add("from"); // TODO attributes and keys
		
// 		Map<String, Double> tolerance = new HashMap<String, Double>();
// 		tolerance.put("colInImage", 0.05);
		
// 		tester = new AutomatedTester(postLabels, tolerance, PostDetector.class);
		
// 		String formattedResults = new String("POST DETECTION TESTS:\n\n");
// 		for (Entry<Log, Map<String, Double>> testResult : tester.logsToPercentErrors.entrySet()) {
// 			formattedResults = formattedResults.concat(testResult.getKey().name);
// 			formattedResults = formattedResults.concat("->");
// 			for (Entry<String, Double> attributeResult : testResult.getValue().entrySet()) {
// 				formattedResults = formattedResults.concat(attributeResult.getKey());
// 				formattedResults = formattedResults.concat(": ");
// 				formattedResults = formattedResults.concat(attributeResult.getValue().toString());
// 			}
// 			formattedResults = formattedResults.concat("\n");
// 		}
// 		formattedResults = formattedResults.concat("\nfalsePositives->");
// 		formattedResults = formattedResults.concat(Integer.toString(tester.falsePositives));
// 		formattedResults = formattedResults.concat(", falseNegatives->");
// 		formattedResults = formattedResults.concat(Integer.toString(tester.falseNegatives));
		
// 		JTextArea display = new JTextArea (formattedResults);
// 		JScrollPane scroll = new JScrollPane (display, 
// 											  JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, 
// 											  JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
// 		add(scroll);
// 		setSize(600, 800);
// 	}

// 	@Override
// 	public Object getCurrentValue() {
// 		return null;
// 	}
// }
