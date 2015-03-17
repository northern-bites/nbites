package nbtool.gui.logviews.images;

import nbtool.data.Log;

public class Detector {
	Log log;
	Testable detection;
	
	Detector(Log log_) {
		log = log_;
	}
	
	public Testable getDetection() {
		return detection;
	}
}