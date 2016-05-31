package nbtool.data.log;

import java.util.Arrays;

import nbtool.util.Utility.Pair;
import nbtool.util.SharedConstants;

public class LogSearching {
	
	public static interface Criteria {
		public boolean fits(String description);
	}
	
	public static class ContainsCriteria implements Criteria {
		String[] requires = new String[0];
		String[] excludes = new String[0];
		
		public boolean fits(String description) {
			for (String r : requires) {
				if (!description.contains(r))
					return false;
			}
			
			for (String e : excludes) {
				if (description.contains(e))
					return false;
			}
			
			return true;
		}
		
		public ContainsCriteria require(String ... reqs) {
			requires = reqs;
			return this;
		}
		
		public ContainsCriteria exclude(String ... exls) {
			excludes = exls;
			return this;
		}
		
		private ContainsCriteria() { }
		
		public static ContainsCriteria create() {
			return new ContainsCriteria();
		}
	}
	
	@SuppressWarnings({ "unchecked" })
	public static Pair<String, Criteria> retrieve(int i) {
		return ((Pair<String, Criteria>) CRITERIA[i]);
	}
	
	public static Pair<String, Criteria> checked(int i) {
		if (i < 0 || i >= CRITERIA.length)
			return null;
		else return retrieve(i);
	}
	
	public static Criteria criteriaAt(int i) {
		 Pair<String, Criteria> found = checked(i);
		 return found == null ? null : found.b;
	}
	
	@SuppressWarnings({ "rawtypes", "unchecked" })
	public static final Pair[] CRITERIA = {
			new Pair("all logs", new Criteria(){
				@Override
				public boolean fits(String description) {
					return true;
				}}),
			
			new Pair("all YUVImages", ContainsCriteria.create().require("camera_", 
					SharedConstants.LOG_BLOCK_IMAGE_WIDTH_PIXELS(), 
					SharedConstants.LOG_BLOCK_IMAGE_HEIGHT_PIXELS())),
			new Pair("all tripoint", ContainsCriteria.create().require("tripoint")),
			new Pair("top images", ContainsCriteria.create().require(
					SharedConstants.LOG_BLOCK_IMAGE_WIDTH_PIXELS(), 
					SharedConstants.LOG_BLOCK_IMAGE_HEIGHT_PIXELS(),
					"camera_TOP")),
			new Pair("bottom images", ContainsCriteria.create().require(SharedConstants.LOG_BLOCK_IMAGE_WIDTH_PIXELS(), 
					SharedConstants.LOG_BLOCK_IMAGE_HEIGHT_PIXELS(),
					"camera_BOT")),
			new Pair("locswarm logs", ContainsCriteria.create().require("locswarm", "RobotLocation", "ParticleSwarm"))
	};	
}
