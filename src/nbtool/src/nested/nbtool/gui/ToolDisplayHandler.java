package nbtool.gui;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.IOException;
import java.lang.management.ManagementFactory;
import java.nio.file.FileStore;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.List;
import java.util.Vector;

import javax.swing.AbstractAction;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTree;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;
import javax.swing.Timer;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.TreeCellRenderer;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

import nbtool.data.OrderedSet;
import nbtool.data.ViewProfile;
import nbtool.data.json.JsonArray;
import nbtool.data.json.JsonParser.JsonParseException;
import nbtool.data.json.Json.JsonValue;
import nbtool.data.log.Log;
import nbtool.data.log.LogReference;
import nbtool.data.log.LogSearching;
import nbtool.data.log.LogSearching.Criteria;
import nbtool.data.log.LogSorting;
import nbtool.data.group.*;
import nbtool.gui.logdnd.LogDND;
import nbtool.gui.logdnd.LogDND.LogDNDSource;
import nbtool.gui.logdnd.LogDND.LogDNDTarget;
import nbtool.gui.logviews.misc.GroupView;
import nbtool.gui.utilitypanes.UtilityManager;
import nbtool.gui.utilitypanes.UtilityParent;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.nio.RobotConnection.RobotFlag;
import nbtool.nio.FileIO;
import nbtool.nio.LogRPC;
import nbtool.nio.RobotConnection;
import nbtool.util.Center;
import nbtool.util.Debug;
import nbtool.util.Events;
import nbtool.util.SharedConstants;
import nbtool.util.ToolSettings;
import nbtool.util.UserSettings;
import nbtool.util.UserSettings.DisplaySettings;
import nbtool.util.Utility;
import nbtool.util.Center.NBToolShutdownListener;
import nbtool.util.Center.ToolEvent;
import nbtool.util.Events.ViewProfileSetChanged;
import nbtool.util.Utility.Pair;

public class ToolDisplayHandler implements 
	IOFirstResponder, Events.LogsFound, Events.GroupAdded {
	
	private final long id = Utility.getNextIndex(this);
	private final ToolDisplayHandler outerThis = this;
	
	private static final Debug.DebugSettings debug = Debug.createSettings(Debug.INFO);
	
	@Override
	public String toString() {
		return this.getClass().getSimpleName() + "-" + id;
	}

	protected ToolDisplayHandler() {
		ToolMessage.displayInfo("creating new display and handler: %s", this);
		display = new ToolDisplay();
		listener = new TitleListener();
		
		setupUtilitiesTab();
		setupControlTab();
		setupLogsTab();
		setupLogDisplay();
		setupFooter();
		
		Center.listen(Events.LogsFound.class, this, true);
		Center.listen(Events.GroupAdded.class, this, true);

		final String boundsKey = this.toString();		
		Center.listen(new NBToolShutdownListener(){
			@Override
			public void nbtoolShutdownCallback() {
				DisplaySettings end = new DisplaySettings(
						display.getBounds(),
						viewProfile,
						display.topLevelSplit.getDividerLocation()
						);
				
				UserSettings.BOUNDS_MAP.put(boundsKey, end);
			}
		});
		
		DisplaySettings ds = UserSettings.BOUNDS_MAP.get(boundsKey);
		if (ds != null) {
			display.setBounds(ds.bounds);
			display.topLevelSplit.setDividerLocation(ds.splitLocation);
			viewProfile = ds.profile == null ?
					ViewProfile.DEFAULT_PROFILE : ds.profile;
		}
		
		display.setTitle("nbtool v" + ToolSettings.VERSION + "." + ToolSettings.MINOR_VERSION);
		display.setMinimumSize(MIN_SIZE);
		
		if (id == 0) {
			display.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		} else {
			display.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
		}
		
		setupKeyBindings();
		
		display.topLevelSplit.setContinuousLayout(false);
		display.topLevelSplit.setAutoscrolls(false);
		
		Dimension min = ToolSettings.DEFAULT_BOUNDS.getSize();
		min.width = 0;
		
		display.leftSideTabs.setMinimumSize(min);
		display.displayTabs.setMinimumSize(min);
		
		display.topLevelSplit.requestFocus();
	}

	public void show(boolean vis) {
		display.setVisible(vis);
	}
	
	public boolean isVisible() {
		return display.isVisible();
	}

	//^^ EXTERNAL
	//------------------------------------------
	//vv INTERNAL
	
	private static final Dimension MIN_SIZE = new Dimension(800, 600);

	private final ToolDisplay display;
	private RobotConnection robot = null;
	private Group lastGroup = null;
	private ViewProfile viewProfile = ViewProfile.DEFAULT_PROFILE;
	
	private static <T> void updateComboBoxAndSettings(JComboBox<T> box, OrderedSet<T> set, T newest) {
		if (newest != null)
			set.update(newest);
		box.setModel(new DefaultComboBoxModel<T>(set.vector()));
		
		if (newest != null)
			box.setSelectedItem(newest);
		else if (set.vector().isEmpty()){
			box.setSelectedIndex(-1);
		} else {
			box.setSelectedIndex(0);
		}
	}
	
	private void setupKeyBindings() {
		
		AbstractAction switchTabsAction = new AbstractAction(){
			@Override
			public void actionPerformed(ActionEvent e) {
				char key = e.getActionCommand().charAt(0);
				if ( Character.isDigit(key) ) {
					int n = (Character.getNumericValue(key) - 1);
					if (n >= 0 && n < display.displayTabs.getTabCount()) {
						display.displayTabs.setSelectedIndex(n);
					}
					
					return;
				}
			}
		};
		
		AbstractAction loadAction = new AbstractAction(){
			@Override
			public void actionPerformed(ActionEvent e) {
				if (display.leftSideTabs.getSelectedComponent() == display.controlTab)
					display.loadButton.doClick();
			}
		};
		
		AbstractAction toggleLeftSideAction = new AbstractAction(){
			@Override
			public void actionPerformed(ActionEvent e) {
				if (display.topLevelSplit.getDividerLocation() > 10) {
					display.topLevelSplit.setDividerLocation(0);
				} else {
					display.topLevelSplit.setDividerLocation(0.5);
					display.topLevelSplit.requestFocus();
				}
			}
		};
		
		display.topLevelSplit.getActionMap().put("switchTabsAction", switchTabsAction);
		display.topLevelSplit.getActionMap().put("loadAction", loadAction);
		display.topLevelSplit.getActionMap().put("toggleLeftSideAction", toggleLeftSideAction);

		for (char c = '1'; c <= '9'; ++c) {
			display.topLevelSplit.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).
				put(KeyStroke.getKeyStroke(c), "switchTabsAction");
		}
		
		display.topLevelSplit.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).
			put(KeyStroke.getKeyStroke(KeyEvent.VK_ENTER, 0), "loadAction");
		
		display.topLevelSplit.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).
			put(KeyStroke.getKeyStroke('`'), "toggleLeftSideAction");
	}

	private void setupViewProfileBox() {
		Collection<ViewProfile> set = ViewProfile.PROFILES.values();
		display.viewProfileBox.setModel(new DefaultComboBoxModel<>(
				set.toArray(new ViewProfile[0])));

		if (set.contains(viewProfile)) {
			display.viewProfileBox.setSelectedItem(viewProfile);
		} else {
			ViewProfile def = ViewProfile.DEFAULT_PROFILE;
			display.viewProfileBox.setSelectedItem(def);
			viewProfile = def;
		}
	}
	
	private void setupUtilitiesTab() {
		display.debugLevelBox.setModel(new DefaultComboBoxModel<Debug.LogLevel>(Debug.LogLevel.values()));
		display.debugLevelBox.setEditable(false);
		display.debugLevelBox.setSelectedIndex(Arrays.asList(Debug.LogLevel.values()).indexOf(Debug.level));
		display.debugLevelBox.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				int sel = display.debugLevelBox.getSelectedIndex();
				Debug.level = Debug.LogLevel.values()[sel];
				UserSettings.logLevel = Debug.level;
				
				Debug.print("changed Debug.level to: %s", Debug.level);
			}
		});

		setupViewProfileBox();

		display.viewProfileBox.addActionListener(new ActionListener(){
			@Override
			public void actionPerformed(ActionEvent e) {
				ViewProfile prof = (ViewProfile) display.viewProfileBox.getSelectedItem();
				if (prof != null) {
					viewProfile = prof;
				}
			}
		});

		Center.listen(Events.ViewProfileSetChanged.class, new ViewProfileSetChanged(){
			public void viewProfileSetChanged(Object src) {
				setupViewProfileBox();
			}
		}, true);	
		
		display.venueField.setText(UserSettings.venue);
		
		display.venueField.addKeyListener(new KeyAdapter(){
			@Override
			public void keyTyped(KeyEvent e) {
				UserSettings.venue = display.venueField.getText();
			}
		});

		JPanel utilityHolder = new JPanel();
		//utilityHolder.setLayout(new BoxLayout(utilityHolder, BoxLayout.Y_AXIS));
		utilityHolder.setLayout(new GridLayout(0, 1));
		for (UtilityParent up : UtilityManager.utilities) {
			utilityHolder.add(new UtilityPanel(up));
		}
//		utilityHolder.add(Box.createVerticalGlue());
		
		display.utilityScrollPane.setViewportView(utilityHolder);
	}
	
	private void controlSelectAction() {
		Path selected = PathChooser.chooseDirPath(display, null);
		if (selected != null) {
			if (FileIO.isValidLogFolder(selected)) {
				updateComboBoxAndSettings(display.pathBox, UserSettings.loadPathes, selected);
			} else {
				ToolMessage.displayError("invalid path to logs: %s", selected);
			}
		}
	}
	
	private void controlLoadAction() {
		if (robot == null) {
			Path selected = (Path) display.pathBox.getSelectedItem();
			if (selected == null) {
				Debug.error("null path");
				ToolMessage.display("load action: null path", Color.RED);
				return;
			}
			
			if (FileIO.isValidLogFolder(selected)) {
				updateComboBoxAndSettings(display.pathBox, UserSettings.loadPathes, selected);
				
				lastGroup = Group.groupFromPath(selected);
				LogReference[] added;
				try {
					added = FileIO.readAllRefsFromPath(selected);
				} catch (Throwable e) {
					ToolMessage.displayError("error {%s} (see below) reading Log refs from %s", 
							e.getMessage(), selected);
					e.printStackTrace();

					return;
				}
				lastGroup.add(added);
				
				Log[] addedLogs = new Log[added.length];
				for (int i = 0; i < added.length; ++i)
					addedLogs[i] = added[i].get();
				
				ToolMessage.displayInfo("loaded %d logs into %s", addedLogs.length, lastGroup);
				
				Events.GGroupAdded.generate(this, lastGroup);
				Events.GLogsFound.generate(this, addedLogs);
				
				display.leftSideTabs.setSelectedComponent(display.logTab);
			} else {
				Debug.error("invalid Log folder: " + selected.toString());
				ToolMessage.display("invalid Log folder: " + selected.toString(), Color.RED);
				return;
			}
			
		} else {
			Debug.error("cannot load session while streaming (%s)", robot);
			ToolMessage.display("cannot load session while streaming", Color.ORANGE);
		}
	}
	
	private void controlRequestFlags() {
		LogRPC.requestFlags(new IOFirstResponder(){
			@Override
			public void ioFinished(IOInstance instance) { assert(false); }
			@Override
			public void ioReceived(IOInstance inst, int ret, Log... out) {
				if (inst != robot) {
					Debug.error("got requestFlags return from instance %s, expected from %s!", 
							inst, robot);
				} else {
					assert(out.length == 1);
					Log flags = out[0];
					assert(flags.logClass.equals(SharedConstants.LogClass_Flags()));
					
					RobotFlag[] parsedFlags = RobotFlag.parseLog(flags);
					
					JPanel container = new JPanel();
					container.setLayout(new GridLayout(parsedFlags.length, 1));
					for (RobotFlag val : parsedFlags) {
						container.add(new FlagPanel(robot, val));
					}
					
					container.setMinimumSize(container.getPreferredSize());
					display.rpcScrollPane.setViewportView(container);
				}
			}
			
			@Override
			public boolean ioMayRespondOnCenterThread(IOInstance inst) {
				return false;
			}
			
		}, robot);
	}
	
	private class ControlConnectRunnable extends Center.EventRunnable {
		private final String robotAddress;
		
		ControlConnectRunnable(String s) {this.robotAddress = s;}

		protected void run() {
			Debug.warn("trying to connect to %s", robotAddress);
			robot = RobotConnection.connectToRobot(robotAddress, outerThis);
			
			if (robot == null) {
				ToolMessage.displayError("connection failed to: %s", robotAddress);
				
				SwingUtilities.invokeLater(new Runnable(){
					@Override
					public void run() {
						display.connectButton.setEnabled(true);
					}
				});
				
				return;
			} else {
				ToolMessage.displayWarn("SUCCESS: connected to %s (%s)", robotAddress, robot);
				
				SwingUtilities.invokeLater(new Runnable(){
					@Override
					public void run() {
						display.connectButton.setEnabled(true);
						
						display.connectButton.setText("disconnect");
						display.loadButton.setEnabled(false);
						updateComboBoxAndSettings(display.robotAddressBox,
								UserSettings.addresses, robotAddress);
						
						controlRequestFlags();
						
						lastGroup = Group.groupForStream(robotAddress);
						Events.GGroupAdded.generate(this, lastGroup);
					}
				});
			}
		}
	}
	
	private void controlConnectAction() {
		if (robot == null) {
			assert(display.connectButton.getText().equals("connect"));
		} else {
			assert(display.connectButton.getText().equals("disconnect"));
			Debug.info("trying to kill %s", robot);
			
			final RobotConnection _robot = robot;
			Center.addEvent(new Center.EventRunnable(){
				@Override
				protected void run() {
					_robot.kill();
				}
			});
			
			return;
		}
		
		String address = (String) display.robotAddressBox.getSelectedItem();
		if (address == null) {
			ToolMessage.displayError("choose valid address");
			return;
		}
		
		address = address.trim();
		
		if (display.localCheckBox.isSelected() && !address.endsWith(".local")) {
			address += ".local";
		}
		
		display.connectButton.setEnabled(false);		
		Center.addEvent(new ControlConnectRunnable(address));
	}
	
	private void setupKeepSlider() {
		Hashtable<Integer, JLabel> labelTable = new Hashtable<>();
		labelTable.put( new Integer( 0 ), new JLabel("0.0") );
		labelTable.put( new Integer( 1 ), new JLabel("0.01") );
		labelTable.put( new Integer( 2 ), new JLabel("0.1") );
		labelTable.put( new Integer( 3 ), new JLabel("0.2") );
		labelTable.put( new Integer( 4 ), new JLabel("1.0") );
		
		display.keepSlider.setSnapToTicks(true);
		display.keepSlider.setMinimum(0);
		display.keepSlider.setMaximum(labelTable.size() - 1);
		display.keepSlider.setLabelTable( labelTable );
		display.keepSlider.setPaintLabels(true);
		display.keepSlider.setPaintTicks(true);
		display.keepSlider.setValue(labelTable.size() - 1);
	}
	
	private int keepMod() {
		int val = display.keepSlider.getValue();
		switch(val) {
		case 0: return 0;
		case 1: return 100;
		case 2: return 10;
		case 3: return 5;
		case 4: return 1;
		default:
			debug.error("bad keepSlider value: %d", val);
			return -1;
		}
	}
	
	private boolean shouldKeep(long index) {
		int km = keepMod();
		return (km > 0) && ((index % km) == 0);
	}
	
	@SuppressWarnings("unchecked")
	private void setupControlTab() {
		updateComboBoxAndSettings(display.pathBox, UserSettings.loadPathes, null);
		updateComboBoxAndSettings(display.robotAddressBox, UserSettings.addresses, null);
		
		final String last = "don't stream";
		Vector<String> streamVector = new Vector<String>();
		for (Pair<String, Criteria> pair : LogSearching.CRITERIA) {
			streamVector.add("stream: " + pair.a);
		}
		streamVector.add(last);
		
		display.streamComboBox.setModel(new DefaultComboBoxModel<String>(streamVector));
		display.streamComboBox.setSelectedItem(last);
				
		display.selectButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				controlSelectAction();
			}
		});
		
		display.loadButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				controlLoadAction();
			}
		});
		
		display.connectButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				controlConnectAction();
			}
		});
		
		setupKeepSlider();
		
		//Stop streaming if the user selects a log...
		Center.listen(Events.LogSelected.class, new Events.LogSelected() {
			@Override
			public void logSelected(Object source, Log first, List<Log> alsoSelected) {
				debug.info("Log{%s} selected, %s disabling streaming!", first, outerClassThis);
				display.streamComboBox.setSelectedItem(last);
			}
		}, true);
		
		display.leftSideTabs.setSelectedComponent(display.controlTab);;
	}
	
	private ArrayList<LogReference> currentlyDisplayedFrom(Group group) {
		ArrayList<LogReference> refs = new ArrayList<>((group).logs);
		LogSearching.Criteria criteria = 
				LogSearching.criteriaAt(display.sasStreamProfile.getSelectedIndex());
		if (criteria != null) {
			for (int i = 0; i < refs.size(); ) {
				if ( !criteria.fits(refs.get(i).description) ) 
					refs.remove(i);
				else ++i;
			}
		}
		
		LogSorting.Sort sort = (LogSorting.Sort) display.sasOrderBox.getSelectedItem();
		if (sort != null) {
			Collections.sort(refs, sort.comparator);
		}
		
		return refs;
	}
	
	private class LogTreeModel implements TreeModel, TreeSelectionListener, LogDNDSource, TreeCellRenderer {
		
		private final DefaultTreeCellRenderer renderer = new DefaultTreeCellRenderer();
		
		@Override
		public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded,
				boolean leaf, int row, boolean hasFocus) {
			JLabel rendered = (JLabel) 
					renderer.getTreeCellRendererComponent(tree, value, selected,
							expanded, leaf, row, hasFocus);
			
			if (value instanceof Group) {
				Group group = (Group) value;
				rendered.setText(group.guiString());
			} else if (value instanceof LogReference) {
				LogReference reference = (LogReference) value;
				rendered.setText(reference.guiString());
			}
			
			return rendered;
		}

		@Override
		public void valueChanged(TreeSelectionEvent e) {
			if (!e.isAddedPath()) {
				//path unselected, ignore
				return;
			}
			
			TreePath[] selected = display.logTree.getSelectionPaths();
			switch(selected[0].getPathCount()) {
			case 0:
				Debug.error("null selection!");
				break;
			case 1:
				Debug.error("root selected!");
				break;
			case 2: {
				Group group = (Group) selected[0].getLastPathComponent();
				displayGroup(group);
				Events.GGroupSelected.generate(this, group);
			} return;
			case 3: {
				List<Log> all = new LinkedList<>();
				for (TreePath path : selected) {
					if (path.getPathCount() != 3)
						continue;	//it isn't a Log selection
					
					LogReference ref = (LogReference) path.getLastPathComponent();
					all.add(ref.get());
				}
				
				Log first = all.remove(0);
				displayLog(first, all);
				Events.GLogSelected.generate(outerClassThis, first, all);
			} return;
			default:
				Debug.error("selection count %d %s", selected[0].getPathCount(),
						selected[0].getLastPathComponent());
			}
		}

		@Override
		public Object getRoot() {
			return this;
		}

		@Override
		public Object getChild(Object parent, int index) {
			if (parent == this) {
				return AllGroups.get(index);
			} else if (parent instanceof Group) {
				Group group = (Group) parent;
				return currentlyDisplayedFrom(group).get(index);
			} else return null;
		}

		@Override
		public int getChildCount(Object parent) {
			if (parent == this) {
				return AllGroups.getGroupCount();
			} else if (parent instanceof Group) {
				return currentlyDisplayedFrom((Group) parent).size();
			} else {
				return 0;
			}
		}

		@Override
		public boolean isLeaf(Object node) {
			return node.getClass() == LogReference.class;
		}

		@Override
		public void valueForPathChanged(TreePath path, Object newValue) {
			Debug.error("logTree should not be editable...");
		}

		@Override
		public int getIndexOfChild(Object parent, Object child) {
			if (parent == this) {
				return AllGroups.allGroups.indexOf(child);
			} else if (parent instanceof Group) {
				return currentlyDisplayedFrom((Group) parent).indexOf(child);
			} else {
				Debug.error("parent %s not container!", parent);
				return -1;
			}
		}

		private final ArrayList<TreeModelListener> listeners = new ArrayList<TreeModelListener>();
		@Override
		public void addTreeModelListener(TreeModelListener l) {
			listeners.add(l);
		}
		@Override
		public void removeTreeModelListener(TreeModelListener l) {
			listeners.remove(l);
		}

		@Override
		public Log[] supplyLogsForDrag() {
			ArrayList<Log> refs = new ArrayList<>();
			for (TreePath tp : display.logTree.getSelectionPaths()) {
				if (tp.getPathCount() == 3) {
					LogReference reference = (LogReference) tp.getLastPathComponent();
					refs.add(reference.get());
				}
			}
			
			return refs.toArray(new Log[0]);
		}
		
		protected void deleteCurrent() {
			for (TreePath tp : display.logTree.getSelectionPaths()) {
				if (tp.getPathCount() == 3) {
					Group group = (Group) tp.getPath()[1];
					LogReference reference = (LogReference) tp.getLastPathComponent();
					
					Debug.warn("deleting {%s} from {%s}", reference, group);
					group.remove(reference);
					
					TreeModelEvent removeEvent = new TreeModelEvent(this, 
							new Object[]{this, group});
					
					for (TreeModelListener listener : listeners)
						listener.treeStructureChanged(removeEvent);
					
				} else {
					Debug.warn("cannot delete {%s}", tp.getLastPathComponent());
				}
			}
		}
		
		protected void showGroupAdded(Group group) {
//			TreeModelEvent tme = new TreeModelEvent(this, new Object[]{root},
//					new int[]{AllGroups.allGroups.indexOf(group)},
//					new Object[]{group});
			Debug.event("LogModel group added");
			
			TreeModelEvent changed = new TreeModelEvent(this, 
					new Object[]{this});
			for (TreeModelListener listener : listeners)
				listener.treeStructureChanged(changed);
		}
		
		protected void showReferenceAdded(LogReference reference) {
			Debug.event("LogModel reference added");

			TreeModelEvent tme = new TreeModelEvent(this, 
					new Object[]{this, reference.container});
			for (TreeModelListener l : listeners) {
				l.treeStructureChanged(tme);
			}
		}
		
		protected void treeReSorted() {
			Debug.event("LogModel tree resorted");

			TreeModelEvent tme = new TreeModelEvent(this, 
					new Object[]{this});
			for (TreeModelListener l : listeners) {
				l.treeStructureChanged(tme);
			}
		}
	}
	
	private LogTreeModel model;
	private final ToolDisplayHandler outerClassThis = this;
	private void setupLogsTab() {
		String last = "don't filter";
		Vector<String> streamVector = new Vector<String>();
		for (Pair<String, Criteria> pair : LogSearching.CRITERIA) {
			streamVector.add("search for: " + pair.a);
		}
		streamVector.add(last);
		
		ActionListener tellModel = new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				model.treeReSorted();
			}
		};
		
		display.sasStreamProfile.setModel(new DefaultComboBoxModel<String>(streamVector));
		display.sasStreamProfile.setSelectedItem(last);
		display.sasStreamProfile.addActionListener(tellModel);
		
		display.sasOrderBox.setModel(new DefaultComboBoxModel<>(LogSorting.Sort.values()));
		display.sasOrderBox.setSelectedItem(LogSorting.Sort.BY_ARRIVAL);
		display.sasOrderBox.addActionListener(tellModel);
		
		model = new LogTreeModel();
		display.logTree.setModel(model);
		display.logTree.setCellRenderer(model);
		
		display.logTree.setEditable(false);
		display.logTree.setRootVisible(false);
		display.logTree.setScrollsOnExpand(true);
		display.logTree.getSelectionModel().setSelectionMode(TreeSelectionModel.DISCONTIGUOUS_TREE_SELECTION);
		display.logTree.addTreeSelectionListener(model);
		LogDND.makeComponentSource(display.logTree, model);
		
		display.logTree.addKeyListener(new KeyAdapter(){
			@Override
			public void keyReleased(KeyEvent e) {
				if (e.getKeyCode() == KeyEvent.VK_DELETE ||
						e.getKeyCode() == KeyEvent.VK_BACK_SPACE) {
					model.deleteCurrent();
				}
			}
		});
	}
	
	private void displayLog(Log log) {
		displayLog(log, new LinkedList<Log>());
	}
	
	private void displayLog(Log log, List<Log> also) {
		tabDisplayer.setContents(viewProfile, log, also);
	}
	
	private void displayGroup(Group g) {
		tabDisplayer.setContents(g);
	}
	
	private ToolDisplayTabs tabDisplayer = null;
	private void setupLogDisplay() {
		LogDND.makeComponentTarget(display.displayTabs, new LogDNDTarget(){
			@Override
			public void takeLogsFromDrop(Log[] log) {
				if (log.length < 1)
					return;
				List<Log> list = new LinkedList<>(Arrays.asList(log));
				displayLog(list.remove(0), list);
			}
		});
		
		tabDisplayer = new ToolDisplayTabs(display);
	}
	
	TitleListener listener = null;
	
	class TitleListener implements Events.CrossStatus, Events.RobotConnectionStatus {
		
		TitleListener() {
			Debug.event("TitleListener() listening...");
			Center.listen(Events.CrossStatus.class, this, true);
			Center.listen(Events.RobotConnectionStatus.class, this, true);
		}
		
		int numCross = 0;
		int numRobot = 0;

		@Override
		public void robotStatus(RobotConnection inst, boolean up) {
			numRobot += up? 1 : -1;
			set();
		}

		@Override
		public void nbCrossFound(CrossInstance inst, boolean up) {
			numCross += up? 1 : -1;
			set();
		}
		
		private void set() {
			display.setTitle(String.format("nbtool - %d CrossInstance, %d RobotConnection", 
					numCross, numRobot));
		}
		
	}
	
	private Timer footerJvmTimer = null;
	private Timer footerDiskTimer = null;
	private final int progressSize = 50;
	
	private FileStore usedFileStore = null;
	private void setupFooter() {
		try {
			usedFileStore = Files.getFileStore(ToolSettings.NBITES_DIR_PATH);
		} catch(Exception e) {
			e.printStackTrace();;
			throw new Error(e);
		}
		
		Debug.warn("Tool footer using fileStore: %s", usedFileStore.name());
		
		display.diskAvailLabel.setText(Utility.progressString(progressSize, 0.5));
		display.jvmAvailLabel.setText(Utility.progressString(progressSize, 0.5));

		footerJvmTimer = new Timer(1000,	//ms
				new ActionListener(){
					@Override
					public void actionPerformed(ActionEvent e) {
						footerJvmAction();
					}
		});
		
		footerJvmTimer.start();
		
		footerDiskTimer = new Timer(1000, new ActionListener(){
			@Override
			public void actionPerformed(ActionEvent e) {
				footerDiskAction();
			}
		});
		footerDiskTimer.start();
	}
	
	private void footerJvmAction() {
		long jvmUsed = ManagementFactory.getMemoryMXBean().getHeapMemoryUsage().getUsed();
	    long jvmMax = ManagementFactory.getMemoryMXBean().getHeapMemoryUsage().getMax();
	    
	    double jvmFrac = ((double) jvmUsed) / ((double) jvmMax);
	    display.jvmAvailLabel.setText(Utility.progressString(progressSize, jvmFrac));
//	    
	    display.footerExtraLabel.setText(String.format("   %d logs, %sused, %smax", 
	    		AllGroups.getLogCount(), Utility.byteString(jvmUsed, true, true, false, false),
	    		Utility.byteString(jvmMax, true, true, false, false)));
	    
//	    Debug.print("jvm %d %s", jvmFrac,
//	    		String.format("%d logs, %s used memory, %s max", 
//	    				AllGroups.getLogCount(), Utility.byteString(jvmUsed, true, true, false, false),
//	    				Utility.byteString(jvmMax, true, true, false, false))
//	    		);	 
	}
	
	private void footerDiskAction() {
	    long diskSpace;
	    long diskUsed;
		try {
			diskSpace = usedFileStore.getTotalSpace();
			diskUsed = diskSpace - usedFileStore.getUnallocatedSpace();
		} catch (IOException e) {
			e.printStackTrace();
			throw new Error(e);
		}
	     
	    double diskFrac = ((double) diskUsed) / ((double) diskSpace);
	    display.diskAvailLabel.setText(Utility.progressString(progressSize, diskFrac));
	}

	/*
	 * Events interface implementations...
	 */
	
	@Override
	public void groupAdded(Object source, Group group) {
		model.showGroupAdded(group);
	}

	@Override
	public void logsFound(Object source, Log... found) {
		for (Log log : found) {
			LogReference ref = log.getReference();
			if (ref != null) {
				model.showReferenceAdded(ref);
			}
		}
	}

	/*
	 * IO implementation...
	 */
	
	@Override
	public void ioFinished(IOInstance instance) {
		if (instance == robot) {
			ToolMessage.displayWarn("robot %s disconnected!", instance);
			display.rpcScrollPane.setViewportView(new JLabel("no connection"));
			display.connectButton.setText("connect");
			display.loadButton.setEnabled(true);
			robot = null;
		} else {
			Debug.error("informed of dead connection %s but robot=%s", instance, robot);
		}
	}

	@Override
	public void ioReceived(IOInstance inst, int ret, Log... out) {		
		if (inst == robot) {
			
			//Deal with streaming...
			LogSearching.Criteria criteria = 
					LogSearching.criteriaAt(display.streamComboBox.getSelectedIndex());
			
			if (criteria != null) {
				Log pass = null;
				
				for (Log l : out) {
					if (criteria.fits(l.getFullDescription())) 
						pass = l;
				}
				
				if (pass != null)
					displayLog(pass);
			}
			
			//Deal with adding to groups (i.e. keeping)...
			assert(lastGroup != null);
			for (Log l : out) {
				if (shouldKeep(l.jvm_unique_id))
					lastGroup.add(LogReference.referenceFromLog(l));
			}
			
			Events.GLogsFound.generate(this, out);
		} else {
			Debug.error("%s got %d surprising logs from %s", this, out.length, inst);
		}
	}

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		return false;
	}
}
