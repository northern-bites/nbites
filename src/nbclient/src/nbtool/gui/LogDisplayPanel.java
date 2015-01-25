package nbtool.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.SwingUtilities;

import nbtool.data.SessionHandler;
import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.N;
import nbtool.util.N.EVENT;
import nbtool.util.N.NListener;
import nbtool.util.U;


public class LogDisplayPanel extends JPanel implements NListener {
	private static final long serialVersionUID = 1L;
	protected LogDisplayPanel() {
		super();
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
				
		views = new JTabbedPane();
		views.setBackground(Color.DARK_GRAY);
		add(views);
		
		N.listen(EVENT.LOG_SELECTION, this);
		setVisible(true);
	}
	
	public void trySetFocus(int index) {
		if (index >= 0 && index < views.getTabCount()) {
			views.setSelectedIndex(index);
		}
	}
	
	public void useSize(Dimension size) {
		//views gets entire thing.
		views.setBounds(0,0, size.width, size.height);
	}
	
	public void notified(EVENT e, Object src, Object... args) {
		if (e == EVENT.LOG_SELECTION) {
			setContents((Log) args[0]);
		}
	}
	
	protected void setContents(Log l) {
		U.w("LDP.setContents() type: " + l.getAttributes().get("type"));
		ArrayList<Class<? extends ViewParent>> list = LogToViewLookup.viewsForLog(l);
		views.removeAll();
		
		this.current = l;
		for (int i = 0; i < list.size(); ++i) {
			Class<? extends ViewParent> ttype = list.get(i);

			Method m;
			boolean slip = false;
			try {
				m = ttype.getMethod("shouldLoadInParallel");
				slip = (Boolean) m.invoke(null);
			} catch (NoSuchMethodException e) {
				e.printStackTrace();
			} catch (SecurityException e) {
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			} catch (IllegalArgumentException e) {
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				e.printStackTrace();
			}
			
			if (slip) {
				JLabel waitLabel = new JLabel("Loading...");
				waitLabel.setFont(waitLabel.getFont().deriveFont(Font.BOLD));
				waitLabel.setForeground(Color.LIGHT_GRAY);
				
				views.addTab(ttype.getSimpleName(), waitLabel);
				CreateViewRunnable cvr = new CreateViewRunnable(i, ttype, l);
				Thread thr = new Thread(cvr);
				thr.start();
			} else {
				ViewParent view = null;
				try {
					view = ttype.getDeclaredConstructor().newInstance();
					view.setLog(l);
					
					views.addTab(ttype.getSimpleName(), view);
				} catch (InstantiationException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IllegalAccessException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IllegalArgumentException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (InvocationTargetException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (NoSuchMethodException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (SecurityException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}	
			}	
		}
	}
 	
	private Log current;
	private JTabbedPane views;
	
	private <T extends ViewParent> void finishedLoading(int tindex, T newView, Class<? extends ViewParent> cls) {
		views.remove(views.getTabComponentAt(tindex));
		views.setComponentAt(tindex, newView);
		newView.repaint();
	}

	private final class CreateViewRunnable implements Runnable {
		int index;
		ViewParent view;
		Class<? extends ViewParent> nlClass;
		Log log;
		boolean done;
		
		protected CreateViewRunnable(int tabIndex, Class<? extends ViewParent> cls, Log lg) {
			index = tabIndex;
			view = null;
			nlClass = cls;
			log = lg;
			done = false;
		}
		
		public void run() {
			if (done) {
				if (current == this.log)
					finishedLoading(index, view, nlClass);
				else
					U.w("WARNING: LDP created new view thread before last finished!");
			} else {
				try {
					view = nlClass.getDeclaredConstructor().newInstance();
					view.setLog(log);
					
				} catch (IllegalArgumentException e) {
					U.w(e.getMessage());
					e.printStackTrace();
				} catch (SecurityException e) {
					U.w(e.getMessage());
					e.printStackTrace();
				} catch (InstantiationException e) {
					U.w(e.getMessage());
					e.printStackTrace();
				} catch (IllegalAccessException e) {
					U.w(e.getMessage());
					e.printStackTrace();
				} catch (InvocationTargetException e) {
					U.w(e.getMessage());
					e.printStackTrace();
				} catch (NoSuchMethodException e) {
					U.w(e.getMessage());
					e.printStackTrace();
				}
				finally {
					done = true;
					SwingUtilities.invokeLater(this);
				}
			}
		}
		
	}
}
