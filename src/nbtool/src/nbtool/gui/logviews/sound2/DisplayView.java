package nbtool.gui.logviews.sound2;

import java.awt.BorderLayout;

import javax.swing.JPanel;
import javax.swing.JSlider;

import nbtool.data.log._Log;
import nbtool.gui.logviews.misc.ViewParent;

public class DisplayView extends ViewParent {
	
	private JSlider ampSlider;
	private JSlider offsetSlider;
	private _Internal internal;

	public DisplayView() {
		ampSlider = new JSlider();
		offsetSlider = new JSlider();
		internal = new _Internal();
		
		this.setLayout(new BorderLayout());
		this.add(ampSlider, BorderLayout.NORTH);
		this.add(offsetSlider, BorderLayout.SOUTH);
		this.add(internal, BorderLayout.CENTER);
	}
	
	@Override
	public void setLog(_Log newlog) {
		// TODO Auto-generated method stub

	}
	
	private class _Internal extends JPanel {
		
	}

}
