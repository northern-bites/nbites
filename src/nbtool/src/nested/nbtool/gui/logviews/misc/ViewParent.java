package nbtool.gui.logviews.misc;

import java.awt.Dimension;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JPanel;
import javax.swing.tree.TreePath;

import nbtool.data.log.Log;
import nbtool.util.Debug;

/**
 * Construction and setupDisplay() are sometimes called in a separate thread,
 *  depending on the value returned by shoudLoadInParallel().
 * */
public abstract class ViewParent extends JPanel {
	private static final long serialVersionUID = 1L;
	protected Log displayedLog = null;
	protected List<Log> alsoSelected = null;
	
	public final void internal(Log log,  List<Log> also) {
		this.displayedLog = log;
		this.alsoSelected = also;
		
		setupDisplay();
	}
	
	//called by external code: use
	//	log 
	//	alsoSelected
	//to populate the view.
	public abstract void setupDisplay();
	
	public abstract String[] displayableTypes();
	
	//Override if you want it to load in parallel.
	public static Boolean shouldLoadInParallel(){ return false; }
	public static Boolean shouldShowInScrollPane(){ return false; }
	
	//Set up log independent objects here.
	//Please call the super (i.e., ViewParent() ) in case this ever becomes non-empty...
	public ViewParent() {
		super();
	}
	
	public static final ViewParent instantiate(Class<? extends ViewParent> subclass) {
		try {
			return subclass.getDeclaredConstructor().newInstance();
		} catch (Exception e) {
			e.printStackTrace();
			Debug.error("message %s\n**********************************************\n"
					+ "class %s does not declare necessary simple constructor!\n",
					e.getMessage(),
					subclass.getName());
			return null;
		}
	}
}
