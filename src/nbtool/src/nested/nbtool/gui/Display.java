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
import nbtool.util.Center;
import nbtool.util.Center.NBToolShutdownListener;
import nbtool.util.Debug;
import nbtool.util.ToolSettings;
import nbtool.util.UserSettings;
import nbtool.util.UserSettings.ExtBounds;
import nbtool.util.Utility;
import static nbtool.util.Debug.*;
public final class Display extends JFrame {
	private static final long serialVersionUID = 1L;
	
	public Display() {
		super("nbtool v" + ToolSettings.VERSION + "." + ToolSettings.MINOR_VERSION);
		setMinimumSize(MIN_SIZE);
		setBounds(UserSettings.bounds);
		
		//Register hook to save preferences.
		final JFrame _display = this;
		
		Center.listen(new NBToolShutdownListener(){
			@Override
			public void nbtoolShutdownCallback() {
				UserSettings.bounds = _display.getBounds();
				UserSettings.leftSplitLoc = split1.getDividerLocation();
				UserSettings.rightSplitLoc = split2.getDividerLocation();
				
				UserSettings.BOUNDS_MAP.put(LogDisplayPanel.MAIN_LOG_DISPLAY_KEY,
						new ExtBounds(null, ldp.currentProfile()));
			}
		});
				
		left = new JTabbedPane();
		right = new JTabbedPane();
		
		ldp = new LogDisplayPanel(true);
		LogDisplayPanel.main = ldp;
		
		cntrlp = new ControlPanel();
		lc = new LogChooser();
		left.addTab("control", cntrlp);
		left.addTab("logs", lc);
		
		statusp = new StatusPanel();
		right.addTab("status", statusp);
		
		cp = new CrossPanel(lc);
		right.addTab("nbcross", cp);
		
		up = new OptionsAndUtilities();
		right.addTab("misc", up);
		
		right.setMinimumSize(new Dimension(0,0));
		left.setMinimumSize(new Dimension(0,0));
		
		split1 = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, true, left, ldp);
		split2 = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, true, split1, right);
		split1.setBorder(null);
		
		split1.setDividerSize(5);
		split2.setDividerSize(5);
		
		add(split2);
		split1.setResizeWeight(.08);
		split2.setResizeWeight(.85);
		split1.setDividerLocation(UserSettings.leftSplitLoc);
		split2.setDividerLocation(UserSettings.rightSplitLoc);
		
		KeyBind.left = left;
		KeyBind.right = right;
		KeyBind.controlPanel = cntrlp;
		KeyBind.mainPanel = ldp;
		
		KeyBind.setupKeyBinds(UserSettings.MISC_MAP);
				
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
		
		System.out.println("---------------------------------- <end initialization>\n\n");
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
