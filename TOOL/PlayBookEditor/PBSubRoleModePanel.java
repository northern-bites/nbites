package TOOL.PlayBookEditor;

import java.awt.*;
import javax.swing.*;
import javax.swing.undo.*;
import java.awt.event.*;
import java.util.Vector;
import TOOL.Image.PlayBookEditorViewer;
import java.io.IOException;

/**
 * Here are the components used for controlling the SubRole mode in the PBE
 * @author Tucker Hermans
 */
public class PBSubRoleModePanel extends PBModePanel
{
    // GUI Components
    protected JButton writeToFileButton, newSubRoleButton, displaySubRoleButton,
	associateButton, reloadButton;
    protected JLabel posTypeLabel, xOffLabel, yOffLabel;
    protected JSpinner xSpinner, ySpinner;
    protected JComboBox posTypeBox;
    // backend stuff
    private SubRole editing;
    private Vector<String> posTypes;

    public PBSubRoleModePanel(PlayBookEditor theEditor)
    {
	super(theEditor);
	// Setup buttons
	newSubRoleButton = new JButton("New SubRole");
	writeToFileButton = new JButton("Write to file");
	associateButton = new JButton("Associate Zone");
	reloadButton = new JButton("Reload SubRoles");
	// Positioning type selector
	posTypeLabel = new JLabel("SubRole Type:");
	posTypes = new Vector<String>();
	posTypes.add("Zone");
	posTypes.add("Line");
	posTypes.add("Point");
	posTypeBox = new JComboBox(posTypes);

	// Setup the offset spinners
	xOffLabel = new JLabel("X Offset:");
	yOffLabel = new JLabel("Y Offset:");
	xSpinner = new JSpinner(new SpinnerNumberModel(100.0, -500.0,
						       500., 10.));
	ySpinner = new JSpinner(new SpinnerNumberModel(100.0, -750.0,
						       750., 10.));

	displaySubRoleButton = new JButton("Display Selected");

	add(posTypeLabel);
	add(posTypeBox);
	add(xOffLabel);
	add(xSpinner);
	add(yOffLabel);
	add(ySpinner);
	add(displaySubRoleButton);
 	add(newSubRoleButton);
	add(reloadButton);
	add(associateButton);
	add(writeToFileButton);

	setupListeners();
	usingOffset(true);
    }

    // Add device listeners
    private void setupListeners()
    {
	newSubRoleButton.addActionListener(new ActionListener(){
		public void actionPerformed(ActionEvent e) {
		    newSubRole();
		}
	    });
	writeToFileButton.addActionListener(new ActionListener(){
		public void actionPerformed(ActionEvent e) {
		    // Show that we can't write out sometimes
		    if (editing == null) {
			JOptionPane.showMessageDialog(null,
						      "No Selected SubRole!",
						      "Error",
						     JOptionPane.ERROR_MESSAGE);

			return;
		    }
		    if (editing.getPosition() == null) {
			JOptionPane.showMessageDialog(null, "SubRole " +
						      editing.toString()+
					  " has no associated position!",
						      "Error",
						      JOptionPane.
						      ERROR_MESSAGE);
			return;
		    }

		    try {
			editor.getComponentLibrary().writeOutSubRole(editing);
		    } catch (IOException ioe) {
			System.err.println("Error writing subRole" +
					   editing.toString() +
					   " to file.");
			System.err.println(e);
		    }
		}
	    });
	posTypeBox.addActionListener(new ActionListener(){
		public void actionPerformed(ActionEvent e) {
		    String mode = (String) posTypeBox.getSelectedItem();

		    // Don't use the offsets with the point type
		    usingOffset(mode != "Point");

		    // Set draw mode based on drop down selection
		    if (mode == "Zone") {
			editor.buttonPanel.zone.setSelected(true);
			editor.getViewer().setMode(
				      PlayBookEditorViewer.DrawingMode.ZONE);
		    } else if (mode == "Line") {
			editor.buttonPanel.line.setSelected(true);
			editor.getViewer().setMode(
				      PlayBookEditorViewer.DrawingMode.LINE);
		    } else if (mode == "Point") {
			editor.buttonPanel.point.setSelected(true);
			editor.getViewer().setMode(
				      PlayBookEditorViewer.DrawingMode.POINT);
		    }
		}
	    });

	displaySubRoleButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    loadSelected();
		}
	    });
	associateButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    PlayBookObject curZone;
		    curZone = editor.getViewer().getSelections().getFirst();
		    // Set ID of current selection to that of the SubRole
		    curZone.setID(editing.toString());
		    // Associate the zone with our subRole
		    editing.associatePosition(curZone.generateRobotPosition());
		    System.out.println("Associated Zone...");
		}
	    });
	reloadButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    editor.getComponentLibrary().reloadSubRoles();
		    try {
			editor.getTree().loadPlayBook(editor);
		    } catch (IOException ioe) {
			System.err.println(e);
		    }
		}
	    });
    }

    /**
     * Method to create a new subRole
     */
    protected void newSubRole()
    {
	// Popup a fancy dialog to ask for the name
	String prompt = "Enter the new role name:";
	String newName = JOptionPane.showInputDialog(null, prompt);
	if (newName == null) {
	    return;
	}
	BehaviorTree t = editor.treePanel;
	SubRole nsr = new SubRole(newName);
	editor.playBookComponents.subRoles.add(nsr);

	// Change our current view
	t.treeScroller.getViewport().setView(t.subRoleView);
	t.displaySelector.setSelectedItem(t.SUBROLE_ID);
	t.subRoleView.setSelectedValue(nsr, true);

	loadEditable(nsr);
    }

    protected void loadEditable(SubRole toEdit)
    {
	if ( !toEdit.isEditable() ){
	    JOptionPane.showMessageDialog(null, "SubRole " +toEdit.toString()+
					  "is not editable!", "Error",
					  JOptionPane.ERROR_MESSAGE);
	    return;
	}
	editing = toEdit;
	editor.setEditingText(toEdit.toString());

	// See if we don't have an attached Position
	if (editing.getPosition() == null) {
	    System.out.println("No attatched position to subRole " +
			       toEdit.toString());
	    return;
	}
	// Set drawing mode
	if ( toEdit.getPosition().getType() == RobotPosition.POINT_TYPE) {
	    posTypeBox.setSelectedItem("Point");
	} else if ( toEdit.getPosition().getType() == RobotPosition.X_LINE_TYPE) {
	    posTypeBox.setSelectedItem("Line");
	    xSpinner.setValue(toEdit.getPosition().getXOffset());
	    ySpinner.setValue(toEdit.getPosition().getYOffset());
	    xSpinner.setEnabled(true);
	    ySpinner.setEnabled(false);
	} else if ( toEdit.getPosition().getType() == RobotPosition.Y_LINE_TYPE) {
	    posTypeBox.setSelectedItem("Line");
	    xSpinner.setValue(toEdit.getPosition().getXOffset());
	    ySpinner.setValue(toEdit.getPosition().getYOffset());
	    xSpinner.setEnabled(false);
	    ySpinner.setEnabled(true);
	} else if ( toEdit.getPosition().getType() == RobotPosition.BOX_TYPE) {
	    posTypeBox.setSelectedItem("Zone");
	    xSpinner.setValue(toEdit.getPosition().getXOffset());
	    ySpinner.setValue(toEdit.getPosition().getYOffset());
	}

	// Draw the actual image
	PlayBookObject toDraw;
	toDraw = toEdit.getPosition().getPlayBookObject(toEdit.toString(),
							editor.getField());

	editor.getViewer().add(toDraw);
    }
    public void loadSelected()
    {
	loadEditable((SubRole)editor.treePanel.subRoleView.getSelectedValue());
    }

    private void usingOffset(boolean shouldUse)
    {
	xOffLabel.setEnabled(shouldUse);
	xSpinner.setEnabled(shouldUse);
	yOffLabel.setEnabled(shouldUse);
	ySpinner.setEnabled(shouldUse);
    }
}
