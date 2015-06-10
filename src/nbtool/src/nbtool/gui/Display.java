package nbtool.gui;

import java.awt.Dimension;
import java.awt.Insets;
import java.awt.KeyEventPostProcessor;
import java.awt.KeyboardFocusManager;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import javax.swing.JFrame;
import javax.swing.JSplitPane;
import javax.swing.JTabbedPane;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UIManager.LookAndFeelInfo;
import javax.swing.UnsupportedLookAndFeelException;

import nbtool.data.SessionMaster;
import nbtool.data.ToolStats;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.gui.utilitypanes.LogToViewUtility;
import nbtool.gui.utilitypanes.UtilityManager;
import nbtool.io.CrossIO;
import nbtool.util.Logger;
import nbtool.util.NBConstants;
import nbtool.util.Prefs;
import nbtool.util.Utility;
import static nbtool.util.Logger.*;
public final class Display extends JFrame implements KeyEventPostProcessor {
	private static final long serialVersionUID = 1L;
	
	public Display() {
		super("nbtool v" + NBConstants.VERSION);
		setMinimumSize(MIN_SIZE);
		setBounds(Prefs.bounds);
		
		//Register hook to save preferences.
		final JFrame _display = this;
		Runnable savePrefsRunnable = new Runnable() {
			public void run() {
				Prefs.bounds = _display.getBounds();
				Prefs.leftSplitLoc = split1.getDividerLocation();
				Prefs.rightSplitLoc = split2.getDividerLocation();
				
				Map<String, Class<? extends ViewParent>[]> lshown = new HashMap<String, Class<? extends ViewParent>[]>();
				LogToViewUtility ltvu = UtilityManager.instanceOfLTV();
				for (String t : NBConstants.POSSIBLE_VIEWS.keySet()) {
					lshown.put(t, (Class<? extends ViewParent>[]) ltvu.selected(t));
				}
				
				Prefs.last_shown = lshown;
				
				try {
					Prefs.savePreferences();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		};
		Runtime.getRuntime().addShutdownHook(new Thread(savePrefsRunnable));
				
		left = new JTabbedPane();
		right = new JTabbedPane();
		
		ldp = new LogDisplayPanel();
		
		cntrlp = new ControlPanel();
		lc = new LogChooser();
		left.addTab("control", cntrlp);
		left.addTab("logs", lc);
				
		statusp = new StatusPanel();
		right.addTab("status", statusp);
		
		cp = new CrossPanel(lc);
		right.addTab("nbcross", cp);
		
		up = new OptionsAndUtilities();
		right.addTab("utility", up);
		
		split1 = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, true, left, ldp);
		split2 = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, true, split1, right);
		split1.setBorder(null);
		
		split1.setDividerSize(5);
		split2.setDividerSize(5);
		
		add(split2);
		split1.setResizeWeight(.08);
		split2.setResizeWeight(.85);
		split1.setDividerLocation(Prefs.leftSplitLoc);
		split2.setDividerLocation(Prefs.rightSplitLoc);
		
		KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventPostProcessor(this);
				
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
	}
	
	public boolean postProcessKeyEvent(final KeyEvent e) {

		if (!e.isConsumed() && (e.getID() == KeyEvent.KEY_TYPED)) {
			
			Character c = e.getKeyChar();
			if (Character.isDigit(c)) {
				ldp.trySetFocus(Character.getNumericValue(c) - 1);
			}
			
			if (Character.isLetter(c)) {
				switch (c) {
				case 'q':
					left.setSelectedIndex(0);
					break;
				case 'w':
					left.setSelectedIndex(1);
					break;
				case 'e':
					right.setSelectedIndex(0);
					break;
				case 'r':
					right.setSelectedIndex(1);
					break;
				case 't':
					right.setSelectedIndex(2);
					break;
				}
			}
		}
		
		return false;
	}

	private JTabbedPane left;
	private JTabbedPane right;
	
	private ControlPanel cntrlp;
	private StatusPanel statusp;
	private LogChooser lc;
		
	private LogDisplayPanel ldp;
	
	private CrossPanel cp;	
	private OptionsAndUtilities up;
		
	private JSplitPane split1;
	private JSplitPane split2;
	
	private static final Dimension MIN_SIZE = new Dimension(800, 600);
}
