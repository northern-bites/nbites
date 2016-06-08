package nbtool.gui.utilitypanes;

import java.util.ArrayList;
import java.util.HashSet;

import javax.swing.JFrame;

import nbtool.util.Debug;
import nbtool.util.UserSettings;
import nbtool.util.UserSettings.DisplaySettings;

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
			DisplaySettings bounds = UserSettings.BOUNDS_MAP.get(key);
			if (bounds != null) {
				Debug.info("using %s for %s", bounds, preferenceKey());
				
				display.setBounds(bounds.bounds);
			}
		} 
		
		previouslySupplied = display;
		/* force standard close behavior... */
		display.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
		return display;
	}
	
	/* get display for this utility – subclass may decide whether to supply new instance on each request */
	public abstract JFrame supplyDisplay();
	
	/* return a string describing the utility's purpose -Automatically added to gui*/
	public abstract String purpose();
	
	/* return preferred keybind (the letter you press to cause the utility to pop up) – may not be honored */
	public abstract char preferredMemnonic();
}
