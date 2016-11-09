package nbtool.gui.logviews.misc;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;

import javax.swing.JPanel;

import nbtool.data.calibration.ColorParam;
import nbtool.data.log.Block;
import nbtool.data.log.Log;
import nbtool.gui.utilitypanes.UtilityManager;
import nbtool.images.YUYV8888Image;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.CrossServer;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.util.Debug;
import nbtool.util.SharedConstants;

public abstract class VisionView extends ViewParent implements IOFirstResponder {

	public static final String DEFAULT_VISION_FUNCTION_NAME = "Vision";

	/* you must implement this! */
	protected abstract void setupVisionDisplay();

	/* you may override these! */

	//determines what function 'callVision()' executes
	protected String functionName() {
		return DEFAULT_VISION_FUNCTION_NAME;
	}

	/* if returns true, callVision() will check ColorCalibrationUtility
	 * and use the most recent ColorParam if such exists (and is global) */
	protected boolean shouldUseGlobalColorParams() {
		return true;
	}

	//Should VisionView call setupVisionDisplay() if no NBCross instance is found?
	protected boolean continueIfNoNBCross() {
		return false;
	}

	/******
	 * These variables and methods are provided as helpers.
	 */

	/* latest from nbcross */
	protected Log latestVisionLog = null;

	protected final boolean callVision() {
		return callVision(displayedLog);
	}

	protected final boolean callVision(Log with) {
		CrossInstance ci = CrossServer.instanceByIndex(0);
		if (ci == null) return false;

		final boolean useGlobal = this.shouldUseGlobalColorParams();

		Log sentToNbcross = null;

		if (useGlobal &&
				UtilityManager.ColorCalibrationUtility.getLatest() != null &&
				UtilityManager.ColorCalibrationUtility.appliedGlobally() ) {
			sentToNbcross = this.displayedLog.deepCopy();

			ColorParam.Set latest = UtilityManager.ColorCalibrationUtility.getLatest();

			sentToNbcross.topLevelDictionary.put("ModifiedColorParams",
					latest.get(this.isTop()).serialize());
		} else {
			sentToNbcross = this.displayedLog;
		}

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

		}, functionName(), sentToNbcross));

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

	protected final Block getWhiteSpotBlock() {
		if (latestVisionLog != null) {
			for (Block b : latestVisionLog.blocks) {
				if (b.type.equals(SharedConstants.SexprType_DEFAULT()) &&
						b.whereFrom.equals("nbcross-Vision-spot-white"))
					return b;
			}
		}

		return null;
	}

	protected final Block getBlackSpotBlock() {
		if (latestVisionLog != null) {
			for (Block b : latestVisionLog.blocks) {
				if (b.type.equals(SharedConstants.SexprType_DEFAULT()) &&
						b.whereFrom.equals("nbcross-Vision-spot-black"))
					return b;
			}
		}

		return null;
	}

	protected final Block getCCDBlock() {
		System.out.println(latestVisionLog == null);
		return latestVisionLog == null ? null : latestVisionLog.find("ccdBuffer");
	}

	protected final Block getDebugImageBlock() {
		return latestVisionLog == null ? null : latestVisionLog.find("debugImage");
	}

	/******
	 * Begin internal implementation.
	 */
	final VisionView outer = this;

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
		if (callVision() || continueIfNoNBCross()) {
			setupVisionDisplay();
		} else {
			Debug.error("%s view not loading because it could not call Vision()",
					this.getClass().getName());
			this.setLayout(null);
			final NBCrossErrorPane errorPanel = new NBCrossErrorPane();
			this.add(errorPanel);

			this.addComponentListener(new ComponentListener(){
				@Override
				public void componentResized(ComponentEvent e) {reset(errorPanel);}
				@Override
				public void componentMoved(ComponentEvent e) {reset(errorPanel);}
				@Override
				public void componentShown(ComponentEvent e) {reset(errorPanel);}
				@Override
				public void componentHidden(ComponentEvent e) {reset(errorPanel);}
			});
		}
	}

	private final void reset(NBCrossErrorPane panel) {
		panel.setBounds(10,10,this.getSize().width - 20, this.getSize().height - 20);
	}

	private class NBCrossErrorPane extends JPanel {
		@Override
		public void paintComponent(Graphics g) {
			g.setColor(Color.DARK_GRAY);
			g.fillRect(0, 0, this.getSize().width, this.getSize().height);

			g.setColor(Color.RED);
			Font use = this.getFont().deriveFont(24.0f);
			g.setFont(use);
			g.drawString("could not connect to NBCross!", 50, 50);
		}
	}
}
