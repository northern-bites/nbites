package nbtool.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.LinkedList;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.SwingUtilities;
import javax.swing.Timer;

import nbtool.util.Utility;
import nbtool.util.Debug;
import nbtool.util.Debug.DebugSettings;
import nbtool.util.Debug.LogLevel;

public class ToolMessage extends JFrame implements ActionListener {
	private static final long serialVersionUID = 1L;

	public static void displayInfo(String format, Object ... args) {
		if (Debug.level.shows(LogLevel.levelINFO)) {
			dbs.info(format, args);
			String displayed = String.format("nbt: " + format, args);
			display(displayed, Color.GREEN);
		}
	}
	
	public static void displayWarn(String format, Object ... args) {
		if (Debug.level.shows(LogLevel.levelWARN)) {
			dbs.warn(format, args);
			String displayed = String.format("nbt: " + format, args);
			display(displayed, Color.ORANGE);
		}
	}
	
	public static void displayError(String format, Object ... args) {
		if (Debug.level.shows(LogLevel.levelERROR)) {
			dbs.error(format, args);
			String displayed = String.format("nbt: " + format, args);
			display(displayed, Color.RED);
		}
	}
	private static final int toolMessageSpace = 40;
	private static final ToolMessage[] spaces =
			new ToolMessage[Toolkit.getDefaultToolkit().getScreenSize().height / toolMessageSpace - 1];
	private int yIndex = -1;
	static {
		for (int i = 0; i < spaces.length; ++i)
			spaces[i] = null;
	}
	
	public static void display(final String message, final Color color) {
		SwingUtilities.invokeLater(new Runnable(){
			@Override
			public void run() {
				final ToolMessage tm = new ToolMessage(message, color);
			}
		});
	}
	
	private static DebugSettings dbs = new DebugSettings(true, true, true, null, null){
		@Override
		protected String instanceLocation() {
			StackTraceElement rel = Utility.codeLocation(5);
			return formatLocation(rel);
		}
	};
		
	private ToolMessage(String message, Color textColor) {
        super();
        synchronized(spaces) {
        	for (int i = 0; i < spaces.length; ++i) {
        		if (spaces[i] == null) {
        			yIndex = i;
        			spaces[i] = this;
        			break;
        		}
        	}
        }
        
//        Debug.plain("%d out of %d", yIndex, spaces.length);
        if (yIndex < 0) yIndex = 1;
        
        setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        initComponents(message, textColor);
        setVisible(true);
        
        disposeTimer = new Timer(5000, this);
        disposeTimer.start();
        
        this.addMouseListener(new MouseAdapter(){
        	@Override 
        	public void mouseClicked(MouseEvent e) {
        		actionPerformed(null);
        	}
        });
    }

	private final int fontSize = 16;
	private final int labelHeight = 30;
	private Timer disposeTimer;

	private void initComponents(String msg, Color color) {
		
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        setLayout(null);
        setUndecorated(true);
        setAlwaysOnTop(true);
        setFocusable(false);
        setOpacity(0.8F);
        
        JLabel msgLabel = new JLabel(msg);
        msgLabel.setForeground(color);
        msgLabel.setFont(msgLabel.getFont().deriveFont((float) fontSize));
        add(msgLabel);
        
        Font labelFont = msgLabel.getFont();
        int stringWidth = msgLabel.getFontMetrics(labelFont).stringWidth(msg);

        final int labelWidth = stringWidth + 10;
        msgLabel.setBounds(0, 0, labelWidth, labelHeight);
        setBounds(screenSize.width - labelWidth - 50,
        		50 + toolMessageSpace*yIndex, labelWidth,
        		labelHeight);
    }

	@Override
	public void actionPerformed(ActionEvent e) {
		if (disposeTimer != null) {
			disposeTimer.stop();
			disposeTimer = null;
			
			synchronized(spaces) {
				if (spaces[yIndex] == this)
					spaces[yIndex] = null;
			}
			
			this.setVisible(false);
			this.dispose();
		}
	}
	
	//testing
	public static void main(String[] args) throws InterruptedException {
		for(;;Thread.sleep(1000))
			new ToolMessage("a longer message than the last one", Color.RED);		
	}

	
}
