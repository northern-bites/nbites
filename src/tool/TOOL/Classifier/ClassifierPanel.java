package TOOL.Classifier;

import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.ButtonGroup;
import javax.swing.JComboBox;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import java.awt.event.ActionListener;
import java.awt.event.KeyListener;
import java.awt.event.KeyEvent;
import java.awt.event.ActionEvent;
import TOOL.Image.ClassifierDrawingPanel;



/**
 * @author Nicholas Dunn
 * @date   February 14, 2008
 * Holds the GUI components of the Classifier system.  This does not include
 * the image being classified, merely the auxiliary buttons.
 */

public class ClassifierPanel extends JPanel implements ActionListener {

    
    public static final String[] choiceStrs = {
        "Ball",
        "Blue Goal",
        "Yellow Goal",
        "Beacon",
        "Line",
        "Center Circle",
        "L Corner",
        "T Corner",
        "Blue Corner arc",
        "Yellow corner arc"
    };


    public static final int BALL = 0;
    public static final int BLUE_GOAL = 1;
    public static final int YELLOW_GOAL = 2;
    public static final int BEACON = 3;
    public static final int LINE = 4;
    public static final int CENTER_CIRCLE = 5;
    public static final int L_CORNER = 6;
    public static final int T_CORNER = 7;
    public static final int BLUE_CORNER_ARC = 8;
    public static final int YELLOW_CORNER_ARC = 9;
    
    protected JComboBox choices;
    protected Classifier classifier;
    protected ClassifierDrawingPanel drawer;
    protected JTextArea echo;
    protected JButton reset, undo, redo, savelog, next, prev, jumpToButton;
	protected JTextField jumpToFrame;

    public ClassifierPanel(Classifier c) {
        super();
        classifier = c;
        drawer = classifier.getDrawingPanel();
        setupWindow();
        registerListeners();
    }


    private void setupWindow() {
        choices = new JComboBox(choiceStrs);
        choices.setSelectedIndex(LINE);
        echo = new JTextArea("Classifier V 1.0");
        echo.setEditable(false);
        echo.setBackground(getBackground());

        reset = new JButton("Reset");
        undo = new JButton("Undo");
        redo = new JButton("Redo");
        savelog = new JButton("Save Log");
        prev = new JButton("Previous");
        next = new JButton("Next");
	
        jumpToButton = new JButton("Jump");
        jumpToFrame = new JTextField("0", 4);

        add(echo);
        add(prev);
        add(next);
        add(jumpToFrame);
        add(jumpToButton);
        add(choices);
        add(reset);
        add(undo);
        add(redo);
        add(savelog);
    }
    
    public void setText(String s) {
        echo.setText(s);
    }


    private void registerListeners() {
        choices.addActionListener(this);
        reset.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    drawer.clearCanvas();
                    classifier.clearCanvas();
                }
            });

        prev.setMnemonic(KeyEvent.VK_S);
        prev.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e){
                    classifier.getLastImage();
                }
            });
	
        next.setMnemonic(KeyEvent.VK_D);
        next.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    classifier.getNextImage();
                }
            });
	
        jumpToFrame.addKeyListener(new KeyListener(){
                public void keyPressed(KeyEvent e){
                    if (e.getKeyCode() == KeyEvent.VK_ESCAPE) {
                        jumpToFrame.transferFocus();
                    } else if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                        jumpToButton.doClick();
                    }
                }
                public void keyTyped(KeyEvent e) {}
                public void keyReleased(KeyEvent e){}
            });
        
        jumpToButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    try {
                        int newIndex = Integer.parseInt(jumpToFrame.getText());
                        classifier.setImage(newIndex);
                        jumpToFrame.transferFocus();
                    }
                    catch (NumberFormatException f) { return; }
                }
            });
	
        undo.setMnemonic(KeyEvent.VK_Z);
        undo.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    drawer.undo();
                }
            });
        redo.setMnemonic(KeyEvent.VK_Y);
        redo.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    drawer.redo();
                }
            });
        savelog.addActionListener(new ActionListener() {
            	public void actionPerformed(ActionEvent e) {
                    classifier.writeLogFile();
                }
            });
    }
    
    
	public void setFrame(String s) {
		jumpToFrame.setText(s);
	}

    /**
     * Parses the choice in the JComboBox and propagates the change through
     * to the Classifier model.
     */
    public void actionPerformed(ActionEvent e) {
        JComboBox cb = (JComboBox)e.getSource();
        int choice = cb.getSelectedIndex();

        switch (choice) {
        case BALL:
            classifier.setMode(Classifier.ClassMode.BALL);
            break;
        case BLUE_GOAL:
            classifier.setMode(Classifier.ClassMode.BLUE_GOAL);
            break;
        case YELLOW_GOAL:
            classifier.setMode(Classifier.ClassMode.YELLOW_GOAL);
            break;
        case BEACON:
            classifier.setMode(Classifier.ClassMode.BEACON);
            break;
        case LINE:
            classifier.setMode(Classifier.ClassMode.LINE);
            break;
        case CENTER_CIRCLE:
             classifier.setMode(Classifier.ClassMode.CENTER_CIRCLE);
             break;
        case L_CORNER:
            classifier.setMode(Classifier.ClassMode.L_CORNER);
            break;
        case T_CORNER:
            classifier.setMode(Classifier.ClassMode.T_CORNER);
            break;
        case BLUE_CORNER_ARC:
            classifier.setMode(Classifier.ClassMode.BLUE_CORNER_ARC);
            break;
        case YELLOW_CORNER_ARC:
            classifier.setMode(Classifier.ClassMode.YELLOW_CORNER_ARC);
            break;
        default:
            System.err.println("Bad input in ClassifierPanel "+
                               "actionPerformed()");
        }
    }
}
