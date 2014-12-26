package nbclient.gui;

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

import nbclient.data.SessionHandler;
import nbclient.io.CppIO;
import nbclient.util.N;
import nbclient.util.P;
import nbclient.util.U;

public final class Display extends JFrame implements KeyEventPostProcessor {
	private static final long serialVersionUID = 1L;
	public static void main(String[] args) {
		//Run static setup.
		U.oldErr = System.err;
		U.oldOut = System.out;
		CppIO.ref(); //The init code doesn't seem to be called unless you ask for a reference.
		
		SwingUtilities.invokeLater(new Runnable(){

			@Override
			public void run() {
				U.w("Creating nbclient.gui.Display instance...");
				new Display();
			}
			
		});
	}
	
	public Display() {
		super("nbclient");
		setTitle("nbclient");
		setMinimumSize(MIN_SIZE);
		setBounds(P.getBounds());
		
		/*
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		}); */
		
		Runnable r = new Runnable() {
			public void run() {
				saveBounds();
			}
		};
		Runtime.getRuntime().addShutdownHook(new Thread(r));
		
		handler = new SessionHandler();
		
		left = new JTabbedPane();
		right = new JTabbedPane();
		
		ldp = new LogDisplayPanel(handler);
		
		sp = new ControlPanel(handler, ldp);
		lc = new LogChooser(handler);
		left.addTab("status", sp);
		left.addTab("logs", lc);
		
		cp = new CppPane(lc);
		right.addTab("c++", cp);
		
		/*
		rp = new RecordPanel();
		right.addTab("record", rp); */
		
		up = new UtilPane();
		right.addTab("prefs/utils", up);
		
		split1 = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, true, left, ldp);
		split2 = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, true, split1, right);
		split1.setBorder(null);
		
		split1.setDividerSize(5);
		split2.setDividerSize(5);
		
		add(split2);
		split1.setResizeWeight(.2);
		split2.setResizeWeight(.85);
		
		KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventPostProcessor(this);
		
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
	}
	
	public void saveBounds() {
		P.putBounds(this.getBounds());
	}
	
	public void useSize(Dimension size) {
		/*
		Insets is = new Insets(5,5,25,5);
	
		int height = size.height - is.top - is.bottom;
		int width = size.width - is.left - is.right;
		
		int x_offset = is.left;
		
		split2.setBounds(is.left, is.top, width, height); */
	}
	
	public boolean postProcessKeyEvent(KeyEvent e) {

		if (!e.isConsumed() && (e.getID() == KeyEvent.KEY_TYPED)) {
			
			Character c = e.getKeyChar();
			if (Character.isDigit(c)) {
				ldp.trySetFocus(Character.getNumericValue(c) - 1);
			}
	
			if (c == '\n' && left.getSelectedIndex() == 0) {
				sp.modelReturnAction();
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
	
	private ControlPanel sp;
	private LogChooser lc;
		
	private LogDisplayPanel ldp;
	
	private CppPane cp;
	private RecordPanel rp;
	
	private UtilPane up;
	
	private SessionHandler handler;
	
	private JSplitPane split1;
	private JSplitPane split2;
	
	private static final Dimension MIN_SIZE = new Dimension(800, 600);
}
