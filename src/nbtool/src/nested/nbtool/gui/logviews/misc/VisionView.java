package nbtool.gui.logviews.misc;

import nbtool.data.log.Block;
import nbtool.data.log.Log;
import nbtool.images.YUYV8888Image;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.CrossServer;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.util.Debug;
import nbtool.util.SharedConstants;

public abstract class VisionView extends ViewParent implements IOFirstResponder {
	
	final VisionView outer = this;
	
	/* latest from nbcross */
	protected Log latestVisionLog = null;

	@Override
	public final String[] displayableTypes() {
		return new String[]{SharedConstants.LogClass_Tripoint()};
	}
	
	@Override
	public final boolean ioMayRespondOnCenterThread(IOInstance inst) {
		return false;
	}

	@Override
	public final void setupDisplay() {
		if (callVision()) {
			setupVisionDisplay();
		} else {
			Debug.error("%s view not loading because it could not call Vision()", 
					this.getClass().getName());
		}
	}
	
	protected final boolean callVision() {
		return callVision(displayedLog);
	}
	
	protected final boolean callVision(Log with) {
		CrossInstance ci = CrossServer.instanceByIndex(0);
		if (ci == null) return false;
		
		assert(ci.tryAddCall(new IOFirstResponder(){

			@Override
			public void ioFinished(IOInstance instance) { outer.ioFinished(instance); }

			@Override
			public void ioReceived(IOInstance inst, int ret, Log... out) {
				assert(out.length == 1);
				assert(out[0].logClass.equals("VisionReturn"));
				
				latestVisionLog = out[0];
				outer.ioReceived(inst, ret, out);
			}

			@Override
			public boolean ioMayRespondOnCenterThread(IOInstance inst) {
				return false;
			}
			
		}, "Vision", displayedLog));
		
		return true;
	}
	
	protected final YUYV8888Image getOriginal() {
		return displayedLog.blocks.get(0).parseAsYUVImage();
	}
	
	protected final byte[] originalImageBytes() {
		return displayedLog.blocks.get(0).data;
	}
	
	protected final boolean isTop() {
		String wf0 = displayedLog.blocks.get(0).whereFrom;
		if (wf0.equals("camera_TOP")) return true;
		if (wf0.equals("camera_BOT")) return false;
		throw new RuntimeException("tripoint image block had invalid type: " + wf0);
	}
	
	protected final int originalWidth() {
		return displayedLog.blocks.get(0).dict.
			get(SharedConstants.LOG_BLOCK_IMAGE_WIDTH_PIXELS()).asNumber().asInt();
	}
	
	protected final int originalHeight() {
		return displayedLog.blocks.get(0).dict.
			get(SharedConstants.LOG_BLOCK_IMAGE_HEIGHT_PIXELS()).asNumber().asInt();
	}
	
	protected final Block getYBlock() {
		return latestVisionLog == null ? null : latestVisionLog.find("yBuffer");
	}
	
	protected final Block getWhiteBlock() {
		return latestVisionLog == null ? null : latestVisionLog.find("whiteRet");
	}
	
	protected final Block getGreenBlock() {
		return latestVisionLog == null ? null : latestVisionLog.find("greenRet");
	}
	
	protected final Block getOrangeBlock() {
		return latestVisionLog == null ? null : latestVisionLog.find("orangeRet");
	}
	
	protected final Block getSegmentedBlock() {
		return latestVisionLog == null ? null : latestVisionLog.find("segBuffer");
	}
	
	protected final Block getEdgeBlock() {
		return latestVisionLog == null ? null : latestVisionLog.find("edgeBuffer");
	}
	
	protected final Block getLineBlock() {
		return latestVisionLog == null ? null : latestVisionLog.find("lineBuffer");
	}
	
	protected final Block getBallBlock() {
		if (latestVisionLog != null) {
			for (Block b : latestVisionLog.blocks) {
				if (b.type.equals(SharedConstants.SexprType_DEFAULT()) &&
						b.whereFrom.equals("nbcross-Vision-ball"))
					return b;
			}
		}
		
		return null;
	}
	
	protected final Block getCCDBlock() {
		return latestVisionLog == null ? null : latestVisionLog.find("ccdBuffer");
	}
	
	protected final Block getDebugImageBlock() {
		return latestVisionLog == null ? null : latestVisionLog.find("debugImage");
	}
	
	protected abstract void setupVisionDisplay();
	
}
