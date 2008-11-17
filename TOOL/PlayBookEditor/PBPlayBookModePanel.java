package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import java.awt.*;
import javax.swing.*;
import javax.swing.undo.*;
import java.awt.event.*;
import java.util.Vector;
import edu.bowdoin.robocup.TOOL.Image.PlayBookEditorViewer;


/**
 * Here are the components used for controlling the PlayBook mode
 * @author Tucker Hermans
 */
public class PBPlayBookModePanel extends PBModePanel {

    protected JLabel optionsLabel;
    protected JButton writeToFileButton, newBookButton;
    protected JCheckBox allowRoleTestingBox;

    /**
     * Constructor to setup the components to be used in the PlayBook modePanel
     */
    public PBPlayBookModePanel(PlayBookEditor theEditor)
    {
	super(theEditor);
	// Num active robots to draw
	optionsLabel = new JLabel("PlayBook Options");
	allowRoleTestingBox = new JCheckBox("Allow role testing");
	allowRoleTestingBox.setSelected(true);
	newBookButton = new JButton("Create New PlayBook");
	newBookButton.addActionListener(this);
	writeToFileButton = new JButton("Write to file");
	// Add items
	add(optionsLabel);
	add(allowRoleTestingBox);
	add(newBookButton);
	add(writeToFileButton);

	setupListeners();
    }

    protected void setupListeners()
    {
	newBookButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    newPlayBook();
		}
	    });
    }

    protected void newPlayBook()
    {
	
    }

}
