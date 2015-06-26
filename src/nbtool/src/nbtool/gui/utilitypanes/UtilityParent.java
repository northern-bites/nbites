package nbtool.gui.utilitypanes;

import java.util.ArrayList;
import java.util.HashSet;

import javax.swing.JFrame;

import nbtool.util.Logger;
import nbtool.util.Prefs;
import nbtool.util.Prefs.ExtBounds;

public abstract class UtilityParent {
	
	protected JFrame previouslySupplied = null;
	
	protected String preferenceKey() {
		return String.format("utility-%s", this.getClass().getSimpleName());
	}
	
	public final JFrame getDisplay() {
		JFrame display = supplyDisplay();
		
		if (previouslySupplied == null) {
			//first time this utility has been requested, try to load bounds.
			
			String key = preferenceKey();
			ExtBounds bounds = Prefs.BOUNDS_MAP.get(key);
			if (bounds != null) {
				Logger.infof("using %s for %s", bounds, preferenceKey());
				
				display.setBounds(bounds.bounds);
			}
		} 
		
		previouslySupplied = display;
		/* force standard dco behavior... */
		display.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
		return display;
	}
	
	/* get display for this utility – subclass may decide whether to supply new instance on each request */
	public abstract JFrame supplyDisplay();
	
	/* return brief description of utility's purpose */
	public abstract String purpose();
	
	/* return preferred keybind – may not be honored */
	public abstract char preferredMemnonic();
}
