package nbtool.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;

import javax.swing.DefaultComboBoxModel;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.SwingUtilities;

import nbtool.data.Log;
import nbtool.data.Session;
import nbtool.data.ViewProfile;
import nbtool.gui.logdnd.LogDND;
import nbtool.gui.logdnd.LogDND.LogDNDTarget;
import nbtool.gui.logviews.misc.ErrorView;
import nbtool.gui.logviews.misc.SessionView;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.gui.utilitypanes.UtilityManager;
import nbtool.util.Center;
import nbtool.util.Events;
import nbtool.util.Logger;
import nbtool.util.Prefs;
import nbtool.util.Events.ViewProfileSetChanged;
import nbtool.util.Prefs.ExtBounds;

public class LogDisplayPanel extends JPanel implements 
	Events.LogSelected, Events.SessionSelected, Events.LogsFound, LogDNDTarget {
	
	public static final String MAIN_LOG_DISPLAY_KEY = "MAIN_LOG_DISPLAY";
	
	public static LogDisplayPanel main = null;
	public static LogDisplayPanel[] extern = new LogDisplayPanel[9];
	
	private boolean 	mainDisplay;
	private Log			current;
	private JTabbedPane tabs;
	
	private DisplayFooter footer;
	private ViewProfile profile = null;
	private final LogDisplayPanel outerThis = this;
	
	protected LogDisplayPanel(boolean isMain) {
		super();
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentShown(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		this.mainDisplay = isMain;
		this.current = null;
		this.tabs = new JTabbedPane();
		this.tabs.setBackground(Color.DARK_GRAY);
		
		if (isMain) {
			ExtBounds eb = Prefs.BOUNDS_MAP.get(MAIN_LOG_DISPLAY_KEY);
			if (eb != null && eb.profile != null) {
				Logger.infof("main LogDisplayPanel using profile %s", eb);
				this.profile = eb.profile;
			} else {
				Logger.warnf("main LogDisplayPanel COULD NOT LOAD PROFILE");
			}
		}
		
		/* if profile isn't set by now, DisplayFooter's setupComboBox will set it */
		this.footer = new DisplayFooter();
				
		add(tabs);
		add(footer);
		LogDND.makeComponentTarget(this, this);
		Center.listen(Events.LogSelected.class, this, true);
		Center.listen(Events.SessionSelected.class, this, true);
		Center.listen(Events.LogsFound.class, this, true);
	}
	
	public void trySetFocus(int index) {
		if (index >= 0 && index < tabs.getTabCount()) {
			tabs.setSelectedIndex(index);
		}
	}
	
	public void useSize(Dimension size) {
		Dimension s = footer.getPreferredSize();
		//Logger.infof("this %s", size);
		tabs.setBounds(0,0, size.width, size.height - s.height);
		footer.setBounds(0, size.height - s.height, size.width, s.height);
		//Logger.infof("footer %s", footer.getSize());
	}
	
	@Override
	public void sessionSelected(Object source, Session s) {
		if (!this.isEnabled() || !this.isVisible() || !this.mainDisplay)
			return;
		
		tabs.removeAll();
		SessionView sv = new SessionView(s);
		tabs.addTab(s.toString(), sv);
	}

	@Override
	public void logSelected(Object source, Log first,
			ArrayList<Log> alsoSelected) {
		if (!this.isEnabled() || !this.isVisible() || !this.mainDisplay)
			return;
		
		footer.streamBox.setSelected(false);
		setContents(first, alsoSelected);
	}
	
	@Override
	public void takeLogsFromDrop(Log[] log) {
		footer.streamBox.setSelected(false);
		if (log.length < 1) return;
		
		ArrayList<Log> logs = new ArrayList<>(Arrays.asList(log));
		setContents(logs.remove(0), logs);
	}

	@Override
	public void logsFound(Object source, Log... found) {
		if (footer.streamBox.isSelected()) {
			Log streamLog = null;

			for (Log l : found) {
				if (l.description().contains(footer.streamField.getText()) &&
						l.source == Log.SOURCE.NETWORK) {
					streamLog = l;
					break;
				}
			}
			
			if (streamLog != null) {
				setContents(streamLog, new ArrayList<Log>());
			}
		}
	}
	
	protected void setContents(Log l, ArrayList<Log> also) {
		Logger.log(Logger.INFO, "LDP.setContents() type: " + l.primaryType());
		//Class<? extends ViewParent>[] list = UtilityManager.instanceOfLTV().viewsForLog(l);
		Class<? extends ViewParent>[] list = profile.viewsForLog(l);
		tabs.removeAll();
		
		this.current = l;
		
		if (this.current.bytes == null) {
			Logger.logf(Logger.ERROR, "Could not load log data: %s", l.toString());
			ErrorView ev = new ErrorView();
			ev._setLog(l);
			tabs.addTab("error loading", ev);
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
				
				tabs.addTab(ttype.getSimpleName(), waitLabel);
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
					
					tabs.addTab(ttype.getSimpleName(), wrap(view));
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
 		
	private void finishedLoading(int tindex, ViewParent newView) {
		tabs.remove(tabs.getTabComponentAt(tindex));
		
		tabs.setComponentAt(tindex, wrap(newView));
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
				if (current == this.log) {
					finishedLoading(index, view);
				} else {
					Logger.log(Logger.WARN, "WARNING: LDP created new view thread before last finished!");
				}
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
	
	public void setProfile(ViewProfile np) {
		this.profile = np;
		footer.setupComboBox();
	}
	
	public ViewProfile currentProfile() {
		return profile;
	}
	
	private class DisplayFooter extends javax.swing.JPanel {
		
		private void setupComboBox() {
			Collection<ViewProfile> set = ViewProfile.PROFILES.values();
			viewProfileBox.setModel(new DefaultComboBoxModel<>(
					set.toArray(new ViewProfile[0])));
			
			if (set.contains(outerThis.profile)) {
				viewProfileBox.setSelectedItem(outerThis.profile);
			} else {
				ViewProfile def = ViewProfile.DEFAULT_PROFILE;
				viewProfileBox.setSelectedItem(def);
				outerThis.profile = def;
			}
		}

	    public DisplayFooter() {
	        initComponents();
	        setupComboBox();
	        
	        viewProfileBox.addActionListener(new ActionListener(){
				@Override
				public void actionPerformed(ActionEvent e) {
					ViewProfile prof = (ViewProfile) viewProfileBox.getSelectedItem();
					if (prof != null) {
						outerThis.profile = prof;
					}
				}
	        });
	        
	        Center.listen(Events.ViewProfileSetChanged.class, new ViewProfileSetChanged(){
	        	public void viewProfileSetChanged(Object src) {
	        		setupComboBox();
	        	}
	        }, true);	        
	    }

	    private void initComponents() {

	        streamBox = new javax.swing.JCheckBox();
	        streamField = new javax.swing.JTextField();
	        viewProfileBox = new javax.swing.JComboBox<>();

	        streamBox.setText("stream logs containing");

	        streamField.setText("TOP");
	        streamField.setMinimumSize(new java.awt.Dimension(50, 28));
	        streamField.setPreferredSize(new java.awt.Dimension(100000, 28));

	        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
	        this.setLayout(layout);
	        layout.setHorizontalGroup(
	            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	            .addGroup(layout.createSequentialGroup()
	                .addContainerGap()
	                .addComponent(streamBox)
	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
	                .addComponent(streamField, javax.swing.GroupLayout.DEFAULT_SIZE, 191, Short.MAX_VALUE)
	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
	                .addComponent(viewProfileBox, javax.swing.GroupLayout.PREFERRED_SIZE, 158, javax.swing.GroupLayout.PREFERRED_SIZE)
	                .addGap(5, 5, 5))
	        );
	        layout.setVerticalGroup(
	            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
	                .addGap(0, 0, Short.MAX_VALUE)
	                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
	                    .addComponent(streamBox)
	                    .addComponent(streamField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
	                    .addComponent(viewProfileBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
	        );
	    }// </editor-fold>                        

	    // Variables declaration - do not modify                     
	    private javax.swing.JCheckBox streamBox;
	    private javax.swing.JTextField streamField;
	    private javax.swing.JComboBox<ViewProfile> viewProfileBox;
	    // End of variables declaration
	}

	private JScrollPane wrap(ViewParent vp) {
		JScrollPane jsp = new JScrollPane(vp);
		return jsp;
	}
}
