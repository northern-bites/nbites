package nbtool.gui.logviews.misc;

import java.awt.Dimension;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.util.ArrayList;

import javax.swing.JPanel;
import javax.swing.tree.TreePath;

import nbtool.data.Log;

/**
 * Construction and setLog() are sometimes called in a separate thread,
 *  depending on the value returned by shoudLoadInParallel().
 * */
public abstract class ViewParent extends JPanel {
	private static final long serialVersionUID = 1L;
	protected Log log;
	
	public final void _setLog(Log newLog) {
		this.log = newLog;
		setLog(log);
	}
	
	//Use newlog to populate view.
	public abstract void setLog(Log newlog);
	//Override if necessary.
	public void alsoSelected(ArrayList<Log> also){}
	
	//Override if you want it to load in parallel.
	public static Boolean shouldLoadInParallel(){return false;}
	
	//Set up log independent objects here.
	//Please call the super (i.e., ViewParent() ) in case this ever becomes non-empty...
	public ViewParent() {
		super();
	}
}
