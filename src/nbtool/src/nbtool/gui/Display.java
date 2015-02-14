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

import javax.swing.JFrame;
import javax.swing.JSplitPane;
import javax.swing.JTabbedPane;
import javax.swing.SwingUtilities;

import nbtool.data.SessionHandler;
import nbtool.data.SessionMaster;
import nbtool.data.Stats;
import nbtool.io.CppIO;
import nbtool.util.N;
import nbtool.util.NBConstants;
import nbtool.util.P;
import nbtool.util.U;
import nbtool.util.N.EVENT;

public final class Display extends JFrame implements KeyEventPostProcessor {
	private static final long serialVersionUID = 1L;
	public static void main(String[] args) {
		
		if (!Display.class.desiredAssertionStatus()) {
			System.out.println("nbtool should always be run with assertions ON (vm argument -ea)");
			System.out.println("if you want to disable this, you'll have to edit the source code.");
			return;
		}
		
		//Run static setup.
		U.w("static singleton Stats..." + Stats.INST.toString());
		U.w("static singleton SessionMaster..." + SessionMaster.INST.toString());
		U.w("static singleton CppIO server ..." + 
				CppIO.current.toString() + " live:" + CppIO.thread.isAlive()); 
		
		SwingUtilities.invokeLater(new Runnable(){

			@Override
			public void run() {
				U.w("Creating nbtool.gui.Display instance...");
				new Display();
			}
			
		});
	}
	
	public Display() {
		super("nbtool v" + NBConstants.VERSION);
		setMinimumSize(MIN_SIZE);
		setBounds(P.getBounds());
		
		Runnable r = new Runnable() {
			public void run() {
				saveBounds();
			}
		};
		Runtime.getRuntime().addShutdownHook(new Thread(r));
				
		left = new JTabbedPane();
		right = new JTabbedPane();
		
		ldp = new LogDisplayPanel();
		
		cntrlp = new ControlPanel();
		lc = new LogChooser();
		left.addTab("control", cntrlp);
		left.addTab("logs", lc);
		
		statusp = new StatusPanel();
		right.addTab("status", statusp);
		
		cp = new CppPane();
		right.addTab("c++", cp);
		
		up = new UtilPane();
		right.addTab("utility", up);
		
		split1 = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, true, left, ldp);
		split2 = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, true, split1, right);
		split1.setBorder(null);
		
		split1.setDividerSize(5);
		split2.setDividerSize(5);
		
		add(split2);
		split1.setResizeWeight(.2);
		split2.setResizeWeight(.85);
		
		KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventPostProcessor(this);
		
		N.notifyEDT(EVENT.STATUS, this, NBConstants.STATUS.IDLE, NBConstants.MODE.NONE);
		
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
	}
	
	public void saveBounds() {
		P.putBounds(this.getBounds());
	}
	
	public boolean postProcessKeyEvent(KeyEvent e) {

		if (!e.isConsumed() && (e.getID() == KeyEvent.KEY_TYPED)) {
			
			Character c = e.getKeyChar();
			if (Character.isDigit(c)) {
				ldp.trySetFocus(Character.getNumericValue(c) - 1);
			}
	
			if (c == '\n' && left.getSelectedIndex() == 0) {
				//cntrlp.modelReturnAction();
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
	
	private CppPane cp;	
	private UtilPane up;
		
	private JSplitPane split1;
	private JSplitPane split2;
	
	private static final Dimension MIN_SIZE = new Dimension(800, 600);
}
