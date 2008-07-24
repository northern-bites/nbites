package TOOL.PlayBookEditor;

import java.awt.*;
import javax.swing.*;
import javax.swing.undo.*;
import java.awt.event.*;
import java.util.Vector;
import TOOL.Image.PlayBookEditorViewer;


/**
 * Here are the components used for controlling the formation mode in the PBE
 * @author Tucker Hermans
 */
public class PBFormationModePanel extends PBModePanel {

    protected JButton writeToFileButton, newFormationButton;

    public PBFormationModePanel(PlayBookEditor theEditor)
    {
	super(theEditor);
	// Num active robots to draw
	newFormationButton = new JButton("Create new Formation");
	writeToFileButton = new JButton("Write to file");
	add(writeToFileButton);
    }
}
