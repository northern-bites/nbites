package nbtool.gui.logviews.misc;

import java.awt.Dimension;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;

import javax.swing.JPanel;

import nbtool.data.Log;

/**
 * Construction and setLog() are sometimes called in a separate thread.
 * */
public abstract class ViewParent extends JPanel {
	private static final long serialVersionUID = 1L;
	protected Log log;
	
	//Use newlog to populate view.
	public abstract void setLog(Log newlog);
	
	//Override if you want it to load in parallel.
	public static Boolean shouldLoadInParallel(){return false;}
	
	//Set up log independent objects here.
	//Please call the super (i.e., ViewParent() ) in case this ever becomes non-empty...
	public ViewParent() {
		super();
	}
	
}
