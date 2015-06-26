package nbtool.gui;

import java.awt.Dimension;
import java.awt.Rectangle;

import javax.swing.JFrame;

import nbtool.data.ViewProfile;
import nbtool.gui.utilitypanes.UtilityParent;
import nbtool.util.Center;
import nbtool.util.Logger;
import nbtool.util.Prefs;
import nbtool.util.Center.NBToolShutdownListener;
import nbtool.util.Prefs.ExtBounds;

public class ExternalLogDisplay extends JFrame {
	//Somewhat arbitrary limit, the tool currently links one display with the number keys [1-9]
	public static final int MAX_EXTERNAL = 9;
	public static ExternalLogDisplay[] displays = new ExternalLogDisplay[9];
	
	public static ExternalLogDisplay requestAny() {
		Logger.infof("ExternalLogDisplay requestAny()");
		
		for (int i = 0; i < MAX_EXTERNAL; ++i) {
			if (displays[i] != null) {
				displays[i].request(true);
				return displays[i];
			}
			
		}
		
		displays[0] = new ExternalLogDisplay(0);
		displays[0].request(true);
		return displays[0];
	}
	
	public static void toggle(int i) {
		Logger.infof("ExternalLogDisplay toggle(%d)", i);
		
		if (displays[i] == null) {
			displays[i] = new ExternalLogDisplay(i);
			displays[i].request(true);
		} else {
			displays[i].request(!displays[i].isVisible());
		}
	}
	
	public static String preferenceKey(int index) {
		return String.format("%s-%d", ExternalLogDisplay.class.getSimpleName(), index);
	}
	
	private void request(boolean show) {
		if (show) {
			this.setVisible(true);
			this.toFront();
		} else {
			this.setVisible(false);
		}		
	}
	
	private ExternalLogDisplay(int index) {
		super("nbtool – external log display");
		this.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
		this.setSize(DEFAULT_SIZE);
		this.setMinimumSize(DEFAULT_SIZE);
		ldp = new LogDisplayPanel(false);
		LogDisplayPanel.extern[index] = ldp;

		ExtBounds eb = Prefs.BOUNDS_MAP.get(preferenceKey(index));
		if (eb != null) {
			Logger.infof("using %s for ExternalLogDisplay[%d]", eb, index);
			if (eb.bounds != null) {
				this.setBounds(eb.bounds);
			}
			
			if (eb.profile != null) {
				this.ldp.setProfile(eb.profile);
			}
		}
		
		this.setContentPane(ldp);
	}

	private LogDisplayPanel ldp;
	
	private final Dimension DEFAULT_SIZE = new Dimension(600, 600);
	
	static {
		Center.listen(new NBToolShutdownListener() {
			@Override
			public void nbtoolShutdownCallback() {
				for (int i = 0; i < MAX_EXTERNAL; ++i) {
					if (displays[i] != null) {
						Rectangle bnds = displays[i].getBounds();
						ViewProfile prof = displays[i].ldp.currentProfile();
						
						Prefs.BOUNDS_MAP.put(preferenceKey(i), new ExtBounds(bnds, prof));
					}
				}
			}
		});
	}
}
