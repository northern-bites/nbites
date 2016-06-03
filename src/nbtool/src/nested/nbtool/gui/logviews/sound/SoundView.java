package nbtool.gui.logviews.sound;

import java.awt.Graphics;

import nbtool.data.log.Block;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.gui.logviews.sound2.ModMath;
import nbtool.gui.logviews.sound3.Constants;
import nbtool.util.Debug;

public class SoundView extends ViewParent {
	
	int rate;
	int target;
	double magLeft;
	double magRight;
	double offLeft;
	double offRight;

	@Override
	public void setupDisplay() {
		Block block = displayedLog.blocks.get(0);
		rate = block.dict.get("rate").asNumber().asInt();
		target = block.dict.get("target").asNumber().asInt();
		
		magLeft = block.dict.get("magLeft").asNumber().asDouble();
		magRight = block.dict.get("magRight").asNumber().asDouble();
		
		offLeft = block.dict.get("offLeft").asNumber().asDouble();
		offRight = block.dict.get("offRight").asNumber().asDouble();
	}
	
	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);

		String header = String.format("rate: %d target frequency: %d", rate, target);
		g.drawString(header, 100, 100);
		int i = 1;
		g.drawString("magLeft " + magLeft, 100, 100 + (20*i++));
		g.drawString("magRight " + magRight, 100, 100 + (20*i++));
		g.drawString("offLeft " + offLeft, 100, 100 + (20*i++));
		g.drawString("offRight " + offRight, 100, 100 + (20*i++));
		
		g.drawLine(0, 500, 500, 500);
		
		double adjusted_left = offLeft;
		while (adjusted_left < 0) adjusted_left += Constants.TAU;
		double adjusted_right = offRight;
		while (adjusted_right < 0) adjusted_right += Constants.TAU;
		
		Debug.info("%f %f", adjusted_left, adjusted_right);
		double offset = ModMath.diff(adjusted_left, adjusted_right);
		double dist_off = 340 * ((offset / Constants.TAU) / target);
		double theta = Math.acos(offset / Constants.EAR_TO_EAR_SEPARATION);
		String derived = String.format("offset %f distance %f theta %f", 
				offset, dist_off, theta);
		g.drawString(derived, 100, 100 + (20*i++));
		
		int start_x = 250;
		int start_y = 500;
		g.drawLine(start_x, start_y,
				(int) (start_x + 100 * Math.cos(theta)), 
				(int) (start_y + 100 * Math.sin(theta)));
	}

	@Override
	public String[] displayableTypes() {
		return new String[]{"sound3"};
	}

}
