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

import nbtool.data.Log;
import nbtool.data.Session;
import nbtool.gui.logviews.misc.ErrorView;
import nbtool.gui.logviews.misc.SessionView;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.gui.utilitypanes.UtilityManager;
import nbtool.util.Center;
import nbtool.util.Events;
import nbtool.util.Logger;


public class LogDisplayPanel extends JPanel implements Events.LogSelected, Events.SessionSelected {
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
		
		Center.listen(Events.LogSelected.class, this, true);
		Center.listen(Events.SessionSelected.class, this, true);
		
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
	
	@Override
	public void sessionSelected(Object source, Session s) {
		if (!this.isEnabled())
			return;
		
		views.removeAll();
		SessionView sv = new SessionView(s);
		views.addTab(s.toString(), sv);
	}

	@Override
	public void logSelected(Object source, Log first,
			ArrayList<Log> alsoSelected) {
		if (!this.isEnabled())
			return;
		
		setContents(first, alsoSelected);
	}
	
	protected void setContents(Log l, ArrayList<Log> also) {
		Logger.log(Logger.INFO, "LDP.setContents() type: " + l.primaryType());
		//Class<? extends ViewParent>[] list = UtilityManager.instanceOfLTV().viewsForLog(l);
		Class<? extends ViewParent>[] list = UtilityManager.LogToViewUtility.viewsForLog(l);
		views.removeAll();
		
		this.current = l;
		
		if (this.current.bytes == null) {
			Logger.logf(Logger.ERROR, "Could not load log data: %s", l.toString());
			ErrorView ev = new ErrorView();
			ev.setLog(l);
			views.addTab("error loading", ev);
			return;
		}
		
		for (int i = 0; i < list.length; ++i) {
			Class<? extends ViewParent> ttype = list[i];

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
				CreateViewRunnable cvr = new CreateViewRunnable(i, ttype, l, also);
				Thread thr = new Thread(cvr);
				thr.start();
			} else {
				ViewParent view = null;
				try {
					view = ttype.getDeclaredConstructor().newInstance();
					view._setLog(l);
					
					if (also != null)
						view.alsoSelected(also);
					
					views.addTab(ttype.getSimpleName(), view);
				} catch (InstantiationException e) {
					e.printStackTrace();
				} catch (IllegalAccessException e) {
					e.printStackTrace();
				} catch (IllegalArgumentException e) {
					e.printStackTrace();
				} catch (InvocationTargetException e) {
					e.printStackTrace();
				} catch (NoSuchMethodException e) {
					e.printStackTrace();
				} catch (SecurityException e) {
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
		ArrayList<Log> also;
		boolean done;
		
		protected CreateViewRunnable(int tabIndex, Class<? extends ViewParent> cls, Log lg,  ArrayList<Log> p) {
			index = tabIndex;
			view = null;
			nlClass = cls;
			log = lg;
			done = false;
			
			also = p;
		}
		
		public void run() {
			if (done) {
				if (current == this.log)
					finishedLoading(index, view, nlClass);
				else
					Logger.log(Logger.WARN, "WARNING: LDP created new view thread before last finished!");
			} else {
				try {
					view = nlClass.getDeclaredConstructor().newInstance();
					view._setLog(log);
					
					if (also != null)
						view.alsoSelected(also);
					
				} catch (IllegalArgumentException e) {
					Logger.log(Logger.ERROR, e.getMessage());
					e.printStackTrace();
				} catch (SecurityException e) {
					Logger.log(Logger.ERROR, e.getMessage());
					e.printStackTrace();
				} catch (InstantiationException e) {
					Logger.log(Logger.ERROR, e.getMessage());
					e.printStackTrace();
				} catch (IllegalAccessException e) {
					Logger.log(Logger.ERROR, e.getMessage());
					e.printStackTrace();
				} catch (InvocationTargetException e) {
					Logger.log(Logger.ERROR, e.getMessage());
					e.printStackTrace();
				} catch (NoSuchMethodException e) {
					Logger.log(Logger.ERROR, e.getMessage());
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
