package nbclient.gui.logviews.parent;

import java.awt.Dimension;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;

import javax.swing.JPanel;

import nbclient.data.Log;

/**
 * Construction and setLog() are sometimes called in a separate thread.
 * */
public abstract class ViewParent extends JPanel {
	private static final long serialVersionUID = 1L;
	protected Log log;
	
	//Use newlog to populate view.
	public abstract void setLog(Log newlog);
	
	//(ONLY CALLED IF CHILDREN CALL SUPER() )
	//Determine proper size of all internal components here.  Note, feel free to simply add a JPanel as the only child of this view,
	//use a java layout therein, and simply yourjpanel.setBounds(0,0,s.width,s.height);
	protected abstract void useSize(Dimension s);
	
	//Override if you want it to load in parallel.
	public static Boolean shouldLoadInParallel(){return false;}
	
	//Set up log independent objects here.
	public ViewParent() {
		super();
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		setLayout(null);
	}
	
}
