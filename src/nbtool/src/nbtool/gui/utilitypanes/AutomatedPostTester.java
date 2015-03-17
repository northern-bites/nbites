package nbtool.gui.utilitypanes;

import java.util.ArrayList;
import java.util.Map;
import java.util.Map.Entry;

import javax.swing.JTextField;

import nbtool.data.Log;
import nbtool.gui.logviews.images.PostDetector;

// TODO expand gui capabilities
// TODO list problematic logs
public class AutomatedPostTester extends UtilityParent {
	AutomatedTester tester;
	
	public AutomatedPostTester() {
		ArrayList<String> postLabels = new ArrayList<String>();
		postLabels.add("singlePost");
		
		tester = new AutomatedTester(postLabels, PostDetector.class);
		
		String formattedResults = new String();
		for (Entry<Log, Map<String, Double>> testResult : tester.results.entrySet()) {
			formattedResults = formattedResults.concat(testResult.getKey().name);
			formattedResults = formattedResults.concat("->");
			for (Entry<String, Double> attributeResult : testResult.getValue().entrySet()) {
				formattedResults = formattedResults.concat(attributeResult.getKey());
				formattedResults = formattedResults.concat(": ");
				formattedResults = formattedResults.concat(attributeResult.getValue().toString());
			}
			formattedResults = formattedResults.concat("\n\n");
		}
		
		JTextField display = new JTextField(formattedResults);
		add(display);
		setSize(800, 800);
	}

	@Override
	public Object getCurrentValue() {
		return null;
	}
}
