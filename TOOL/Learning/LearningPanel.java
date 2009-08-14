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

    private JTextField jumpToFrame;
	private JButton prevImage, nextImage, jumpToButton, writeKey, runBatch;
    private JTextPane feedback;

    private Learning learn;


	/**  Constructor.  Sets up panel and listeners.
	 */
    public LearningPanel(Learning aLearn) {
        super();
		learn = aLearn;
        setupWindow();
        setupListeners();
    }

	/** Puts on all of the buttons and formats things.
	 */
    private void setupWindow() {

        // centering text from http://forum.java.sun.com/thread.jspa?threadID=166685&messageID=504493
        feedback = new JTextPane();
        // Make the text centered
        SimpleAttributeSet set = new SimpleAttributeSet();
        StyledDocument doc = feedback.getStyledDocument();
        StyleConstants.setAlignment(set,StyleConstants.ALIGN_CENTER);
        feedback.setParagraphAttributes(set,true);

        feedback.setEditable(false);
        feedback.setText("Welcome to TOOL 1.0");
        // Make the background match in color
        feedback.setBackground(this.getBackground());

        prevImage = new JButton("Previous (S)");
        prevImage.setFocusable(false);

        nextImage = new JButton("Next (D)");
        nextImage.setFocusable(false);

        jumpToFrame = new JTextField("0", 4);

        jumpToButton = new JButton("Jump");
        jumpToButton.setFocusable(false);

		writeKey = new JButton("Write Key");
		writeKey.setFocusable(false);

		runBatch = new JButton("Run Batch");
		runBatch.setFocusable(false);

        setLayout(new BoxLayout(this, BoxLayout.LINE_AXIS));


        JPanel navigation = new JPanel();
        navigation.setLayout(new GridLayout(4,2));
        navigation.add(prevImage);
        navigation.add(nextImage);
        navigation.add(jumpToFrame);
        navigation.add(jumpToButton);
		navigation.add(writeKey);
		navigation.add(runBatch);

        // Size the navigation panel to only take up as much room as needed
        Dimension navigationSize = new Dimension(2 * (int) prevImage.getPreferredSize().getWidth(), 4 * (int) jumpToFrame.getPreferredSize().getWidth());
        navigation.setMinimumSize(navigationSize);
        navigation.setPreferredSize(navigationSize);
        navigation.setMaximumSize(navigationSize);
        add(navigation);
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
        feedback.setText(text);
    }

	/** Returns the current feedback message
		@return  the feedback string
	 */
    public String getText() {
		return feedback.getText();
    }



    /** Set the text in the box to update the frame number. */
    public void notifyFrame(Frame f) {
        jumpToFrame.setText((new Integer(f.index())).toString());
    }

	// These methods are place holders designed to fill contracts
    public void notifyDataSet(DataSet s, Frame f) {
    }
    public void keyPressed(KeyEvent e) {}
    public void keyReleased(KeyEvent e) {}
    public void keyTyped(KeyEvent e) {}

}

