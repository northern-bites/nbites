package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import java.awt.*;
import javax.swing.*;
import javax.swing.undo.*;
import java.awt.event.*;
import java.util.Vector;
import java.util.Enumeration;
import edu.bowdoin.robocup.TOOL.Image.PlayBookEditorViewer;


/**
 * Here are the components used for controlling the Role mode in the PBE
 * @author Tucker Hermans
 */
public class PBRoleModePanel extends PBModePanel {

    protected JButton writeToFileButton, newRoleButton, editSelectedButton;
    protected Role editing;

    public PBRoleModePanel(PlayBookEditor theEditor)
    {
	super(theEditor);
	// Num active robots to draw
	newRoleButton = new JButton("New Role");
	newRoleButton.addActionListener(this);
	editSelectedButton = new JButton("Edit Selected Role");
	editSelectedButton.addActionListener(this);
	writeToFileButton = new JButton("Write to file");
	writeToFileButton.addActionListener(this);

	// Add our stuff
	add(newRoleButton);
	add(writeToFileButton);

	setupListeners();
    }

    protected void setupListeners()
    {
	newRoleButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    newRole();
		}
	    });
	editSelectedButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    loadEditable((Role)editor.treePanel.roleView.getSelectedValue());
		}
	    });

    }

    protected void newRole()
    {
	String prompt = "Enter the new role name:";
	String newName = JOptionPane.showInputDialog(null, prompt);
	if (newName == null) {
	    return;
	}
	BehaviorTree t = editor.treePanel;
	Role ns = new Role(newName);
	editor.playBookComponents.roles.add(ns);

	// Change the selection to roles
	t.treeScroller.getViewport().setView(t.roleView);
	t.displaySelector.setSelectedItem(t.ROLE_ID);
	t.roleView.setSelectedValue(ns, true); // Highlight the new role

	loadEditable(ns);
    }

    /**
     * Method to load in the next Role to edit.
     *
     * @param toEdit The role to now be edited
     */
    protected void loadEditable(Role toEdit)
    {
	if ( !toEdit.isEditable() ){
	    JOptionPane.showMessageDialog(null, "Role " +toEdit.toString()+
					  "is not editable!", "Error", 
					  JOptionPane.ERROR_MESSAGE);
	    return;
	}

	editing = toEdit;
	editor.setEditingText(toEdit.toString());

	// Clear the field of whatever is on it
	// Draw the zones defined for this Role
	Enumeration<SubRole> toDraw = editing.getChildren().elements();
	SubRole f;
	while(toDraw.hasMoreElements()) {
	    f = toDraw.nextElement();
	    //editor.drawZone(editing.getAssociatedZone(f));
	}
    }


}
