package nbtool.gui.logviews.images;

import nbtool.data.Log;

public class PostDetection extends Testable {
	int singlePost; // leftPost if rightPost is set
	int rightPost;
	
	public PostDetection(Log log_, int singlePost_, int rightPost_) {
		super(log_);
		
		singlePost = singlePost_;
		rightPost = rightPost_;
	}
	
	public PostDetection(Log log_, int singlePost_) {
		this(log_, singlePost_, -1);
	}
}