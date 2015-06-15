package nbtool.gui.utilitypanes;

import java.util.ArrayList;
import java.util.HashSet;

import javax.swing.JFrame;

public abstract class UtilityParent {
	public final JFrame getDisplay() {
		JFrame display = supplyDisplay();
		/* force standard utility display options... */
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
