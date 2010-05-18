package TOOL.Learning;

import javax.swing.JPanel;
import javax.swing.JCheckBox;
import javax.swing.JTextArea;
import java.awt.Component;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import javax.swing.JComponent;
import javax.swing.JSlider;
import javax.swing.JButton;
import java.awt.event.KeyListener;
import java.awt.event.KeyEvent;

import java.awt.event.MouseWheelListener;
import java.awt.event.MouseWheelEvent;


import javax.swing.InputMap;
import javax.swing.ActionMap;
import java.awt.event.InputEvent;
import javax.swing.KeyStroke;
import javax.swing.AbstractAction;
import TOOL.Data.DataListener;
import TOOL.Data.Frame;
import TOOL.Data.DataSet;
import javax.swing.BoxLayout;
import javax.swing.JTextPane;
import javax.swing.JTextField;
import javax.swing.JComboBox;
import javax.swing.text.*;
import java.awt.GridLayout;
import java.awt.Font;
import java.awt.Dimension;
import java.awt.Cursor;

import TOOL.Image.ImageOverlay;

import TOOL.Calibrate.ColorSwatchParent;

import TOOL.TOOL;

/**  Panel used to navigate through frames in the learning system.  It also is used to
	 tell when the user wants the edited information written out to a file.  This is
	 largely copied from the Calibration version of the same panel, but rips out a lot
	 of calibration stuff that isn't needed here.  There is probably a decent amount of
	 code that can still be removed.
	 @author Eric Chown, 2009
 */

public class LearningPanel extends JPanel implements DataListener, KeyListener {

    private InputMap im;
    private ActionMap am;
    private JTextField jumpToFrame;
	private JButton prevImage, nextImage, jumpToButton, writeKey, runBatch,
		useLastFrame, runRecursive, learnColor;
    private JTextPane feedback;

    private Learning learn;

	private JPanel logs, screen;
	private JCheckBox falseGoals, falseCrosses, falseBalls, falseLCorners,
		falseTCorners, falseCcCorners;
	private JCheckBox missedGoals, missedCrosses, missedBalls, missedLCorners,
		missedTCorners, missedCcCorners;

	private JCheckBox onlyBalls, onlyGoals, onlyCrosses, onlyBots,
		onlyLCorners, onlyTCorners, onlyCcCorners;


	/**  Constructor.  Sets up panel and listeners.
	 */
    public LearningPanel(Learning aLearn) {
        super();
		learn = aLearn;
        setupWindow();
		setupShortcuts();
        setupListeners();
    }

	/** Puts on all of the buttons and formats things.
	 */
    private void setupWindow() {
		logs = new JPanel();
		logs.setLayout(new GridLayout(2, 3));
		falseGoals = new JCheckBox("False Goals");
		falseCrosses = new JCheckBox("False Cross");
		falseBalls = new JCheckBox("False Balls");
		falseLCorners = new JCheckBox("False LCorners");
		falseTCorners = new JCheckBox("False TCorners");
		falseCcCorners = new JCheckBox("False CcCorners");

		missedGoals = new JCheckBox("Missed Goals");
		missedCrosses = new JCheckBox("Missed Crosses");
		missedBalls = new JCheckBox("Missed Balls");
		missedLCorners = new JCheckBox("Missed LCorners");
		missedTCorners = new JCheckBox("Missed TCorners");
		missedCcCorners = new JCheckBox("Missed CcCorners");
		logs.add(falseGoals);
		logs.add(falseCrosses);
		logs.add(falseBalls);
		logs.add(falseLCorners);
		logs.add(falseTCorners);
		logs.add(falseCcCorners);

		logs.add(missedGoals);
		logs.add(missedCrosses);
		logs.add(missedBalls);
		logs.add(missedLCorners);
		logs.add(missedTCorners);
		logs.add(missedCcCorners);

		// checkboxes for screening frames - e.g. only give me frames with goals
		screen = new JPanel();
		screen.setLayout(new GridLayout(2, 2));
		onlyGoals = new JCheckBox("Need Goals");
		onlyCrosses = new JCheckBox("Need Cross");
		onlyBalls = new JCheckBox("Need Ball");
		onlyBots = new JCheckBox("Need Bots");
		onlyLCorners = new JCheckBox("Need LCorners");
		onlyCcCorners = new JCheckBox("Need CcCorners");
		onlyTCorners = new JCheckBox("Need TCorners");
		screen.add(onlyBalls);
		screen.add(onlyGoals);
		screen.add(onlyCrosses);
		screen.add(onlyBots);
		screen.add(onlyLCorners);
		screen.add(onlyCcCorners);
		screen.add(onlyTCorners);

        prevImage = new JButton("Previous (S)");
        prevImage.setFocusable(false);

        nextImage = new JButton("Next (D)");
        nextImage.setFocusable(false);

        jumpToFrame = new JTextField("0", 4);
		jumpToFrame.setFocusable(true);

        jumpToButton = new JButton("Jump");
        jumpToButton.setFocusable(false);

		writeKey = new JButton("Write Key (W)");
		writeKey.setFocusable(false);

		runBatch = new JButton("Run Batch");
		runBatch.setFocusable(false);

		runRecursive = new JButton("Run Recursive");
		runRecursive.setFocusable(false);

		useLastFrame = new JButton("Use Last");
		useLastFrame.setFocusable(false);

		learnColor = new JButton("Learn Color");
		learnColor.setFocusable(false);

        setLayout(new BoxLayout(this, BoxLayout.LINE_AXIS));


        JPanel navigation = new JPanel();
        navigation.setLayout(new GridLayout(5,2));
        navigation.add(prevImage);
        navigation.add(nextImage);
        navigation.add(jumpToFrame);
        navigation.add(jumpToButton);
		navigation.add(writeKey);
		navigation.add(runBatch);
		navigation.add(runRecursive);
		navigation.add(useLastFrame);
		navigation.add(learnColor);

        // Size the navigation panel to only take up as much room as needed
        Dimension navigationSize = new Dimension(2 * (int) prevImage.getPreferredSize().getWidth(), 4 * (int) jumpToFrame.getPreferredSize().getWidth());
        navigation.setMinimumSize(navigationSize);
        navigation.setPreferredSize(navigationSize);
        navigation.setMaximumSize(navigationSize);
		add(screen);
        add(navigation);
		add(logs);
    }

	private void setupShortcuts(){
		im = this.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);

        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_D, 0), "nextImage");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_S, 0), "prevImage");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_W, 0), "writeData");

		am = this.getActionMap();

		am.put("prevImage", new AbstractAction("preImage"){
				public void actionPerformed(ActionEvent e){
					prevImage.doClick();
				}
			});
		am.put("nextImage", new AbstractAction("nextImage"){
				public void actionPerformed(ActionEvent e){
					nextImage.doClick();
				}
			});

		am.put("writeData", new AbstractAction("writeData"){
				public void actionPerformed(ActionEvent e){
					writeKey.doClick();
				}
			});
	}


	/** Sets up listeners for all of the buttons.
	 */
    private void setupListeners() {
        learn.getTool().getDataManager().addDataListener(this);

        prevImage.addActionListener(new ActionListener(){
                public void actionPerformed(ActionEvent e) {
                    learn.getLastImage();
                }
            });

        nextImage.addActionListener(new ActionListener(){
                public void actionPerformed(ActionEvent e) {
                    learn.getNextImage();
                }
            });

		writeKey.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					learn.writeData();
				}
			});

		runBatch.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					learn.runBatch();
				}
			});

		runRecursive.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
                    String topDir =
                        learn.getTool().
                        CONSOLE.promptDirOpen("Top Directory for Recursion",
                                              "../man/frames/");
                    System.out.println(topDir);
					learn.runRecursiveBatch(topDir);
				}
			});

		learnColor.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					learn.runRecursiveBatchLearning();
				}
			});

		useLastFrame.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					learn.useLast();
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
                        learn.setImage(newIndex);
                        jumpToFrame.transferFocus();
                    }
                    // If they didn't have a legit number in the field, this
                    // catches the problem to avoid a crash.
                    catch (NumberFormatException f) {
                        jumpToFrame.setText("Invalid");
                        jumpToFrame.setSelectionStart(0);
                        jumpToFrame.setSelectionEnd("Invalid".length());
                        return;
                    }
                }
            });

    }

	/* return true    if false ball logging has been checked
	 */
	public boolean getFalseBalls() {
		return falseBalls.isSelected();
	}

	/* return true    if false goal logging has been checked
	 */
	public boolean getFalseGoals() {
		return falseGoals.isSelected();
	}

	/* return true    if false cross logging has been checked
	 */
	public boolean getFalseCrosses() {
		return falseCrosses.isSelected();
	}

	/* return true    if false LCorner logging has been checked
	 */
	public boolean getFalseLCorners() {
		return falseLCorners.isSelected();
	}

	/* return true    if false TCorner logging has been checked
	 */
	public boolean getFalseTCorners() {
		return falseTCorners.isSelected();
	}

	/* return true    if false CcCorner logging has been checked
	 */
	public boolean getFalseCcCorners() {
		return falseCcCorners.isSelected();
	}

	/* return true    if missed ball logging has been checked
	 */
	public boolean getMissedBalls() {
		return missedBalls.isSelected();
	}

	/* return true    if missed goal logging has been checked
	 */
	public boolean getMissedGoals() {
		return missedGoals.isSelected();
	}

	/* return true    if missed cross logging has been checked
	 */
	public boolean getMissedCrosses() {
		return missedCrosses.isSelected();
	}

	/* return true    if missed LCorner logging has been checked
	 */
	public boolean getMissedLCorners() {
		return missedLCorners.isSelected();
	}

	/* return true    if missed TCorner logging has been checked
	 */
	public boolean getMissedTCorners() {
		return missedTCorners.isSelected();
	}

	/* return true    if missed CcCorner logging has been checked
	 */
	public boolean getMissedCcCorners() {
		return missedCcCorners.isSelected();
	}

	/* return true    if screening all but balls
	 */
	public boolean getOnlyBalls() {
		return onlyBalls.isSelected();
	}

	/* return true    if screening all but goals
	 */
	public boolean getOnlyGoals() {
		return onlyGoals.isSelected();
	}

	/* return true    if screening all but crosses
	 */
	public boolean getOnlyCrosses() {
		return onlyCrosses.isSelected();
	}

	/* return true    if screening all but robots
	 */
	public boolean getOnlyBots() {
		return onlyBots.isSelected();
	}

	/* return true    if screening all but LCorners
	 */
	public boolean getOnlyLCorners() {
		return onlyLCorners.isSelected();
	}

	/* return true    if screening all but TCorners
	 */
	public boolean getOnlyTCorners() {
		return onlyTCorners.isSelected();
	}

	/* return true    if screening all but CcCorners
	 */
	public boolean getOnlyCcCorners() {
		return onlyCcCorners.isSelected();
	}

    /**
     * Greys out buttons depending on whether we can actually use them at this
     * moment;
     */
    public void fixButtons() {
        prevImage.setEnabled(learn.canGoBackward());
        nextImage.setEnabled(learn.canGoForward());
        jumpToFrame.setEnabled(learn.hasImage());
        jumpToButton.setEnabled(learn.hasImage());
		writeKey.setEnabled(!learn.getQuietMode());
    }

	/** Sets the feedback text.
		@param text   the new feedback message
	 */
    public void setText(String text) {
    }

	/** Returns the current feedback message
		@return  the feedback string
	 */
    public String getText() {
		return "Yo!";
    }



    /** Set the text in the box to update the frame number. */
    public void notifyFrame(Frame f) {
        jumpToFrame.setText((new Integer(f.index())).toString());
    }

	// These methods are place holders designed to fill contracts
    public void notifyDataSet(DataSet s, Frame f) {
        jumpToFrame.setText((new Integer(f.index())).toString());
    }
    public void keyPressed(KeyEvent e) {}
    public void keyReleased(KeyEvent e) {}
    public void keyTyped(KeyEvent e) {}

}

