package TOOL.PlayBookEditor;

import java.awt.*;
import javax.swing.*;
import javax.swing.undo.*;
import java.awt.event.*;
import java.util.Vector;
import java.util.Enumeration;
import TOOL.Image.PlayBookEditorViewer;


/**
 * Here are the components used for controlling the test mode in the PBE
 * @author Tucker Hermans
 */
public class PBStrategyModePanel extends PBModePanel
{
    // Class Components
    protected JButton writeToFileButton, newStrategyButton, editSelectedButton,
	associationButton, selectKickoffReadyButton, selectNonKickReadyButton;
    protected Strategy editing; // The currently selected strategy to edit
    protected JCheckBox useFinderBox, useKickoffBox;
    // Need some way of showing which Ready Formation(s) are used.
    protected JLabel kickoffReadyLabel, nonKickReadyLabel, kickoffChosenLabel,
	nonKickChosenLabel;

    public PBStrategyModePanel(PlayBookEditor theEditor)
    {
	super(theEditor);
	// Setup buttons
	newStrategyButton = new JButton("New Strategy");
	editSelectedButton = new JButton("Edit Selected");
	writeToFileButton = new JButton("Write to file");
	associationButton = new JButton("Associate Zone");
	selectKickoffReadyButton = new JButton("Select Kickoff");
	selectNonKickReadyButton = new JButton("Select Non-Kickoff");

	// Setup labels
	kickoffReadyLabel = new JLabel("Ready Kickoff:");
	kickoffChosenLabel = new JLabel("NONE");
	nonKickReadyLabel = new JLabel("Ready Non-Kickoff:");
	nonKickChosenLabel = new JLabel("NONE");
	editing = null;

	//Setup CheckBoxes
	useFinderBox = new JCheckBox("Use Finder");
	useKickoffBox = new JCheckBox("Use Kickoff");

	// Add the components
	add(kickoffReadyLabel);
	add(kickoffChosenLabel);
	add(nonKickReadyLabel);
	add(nonKickChosenLabel);
	add(useFinderBox);
	add(useKickoffBox);
	// Buttons
	add(newStrategyButton);
	add(editSelectedButton);
	add(associationButton);
	add(selectKickoffReadyButton);
	add(selectNonKickReadyButton);
	add(writeToFileButton);

	setupListeners();
    }

    protected void setupListeners()
    {
	newStrategyButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    newStrategy();
		}
	    });
	editSelectedButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    loadEditable((Strategy)editor.treePanel.stratView.getSelectedValue());
		}
	    });

	associationButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    associateZoneAndFormation();
		}
	    });
    }

    protected void newStrategy()
    {
	String prompt = "Enter the new strategy name:";
	String newName = JOptionPane.showInputDialog(null, prompt);
	if (newName == null) {
	    return;
	}
	BehaviorTree t = editor.treePanel;
	Strategy ns = new Strategy(newName);
	editor.playBookComponents.strategies.add(ns);

	// Change the selection to strategies
	t.treeScroller.getViewport().setView(t.stratView);
	t.displaySelector.setSelectedItem(t.STRAT_ID);
	t.stratView.setSelectedValue(ns, true); // Highlight the new strategy

	loadEditable(ns);
    }

    /**
     * Method to load in the next Strategy to edit.
     *
     * @param toEdit The strategy to now be edited
     */
    protected void loadEditable(Strategy toEdit)
    {
	if ( !toEdit.isEditable() ){
	    JOptionPane.showMessageDialog(null, "Strategy " +toEdit.toString()+
					  "is not editable!", "Error",
					  JOptionPane.ERROR_MESSAGE);
	    return;
	}

	editing = toEdit;
	editor.setEditingText(toEdit.toString());

	// Clear the field of whatever is on it
	// Draw the zones defined for this Strategy
	Enumeration<Formation> toDraw = editing.getChildren().elements();
	Formation f;
	PlayBookObject drawing;
	for (; toDraw.hasMoreElements();) {
	    f = toDraw.nextElement();
	    drawing = editing.getAssociatedZone(f);
	    editor.getViewer().add(drawing);
	}
    }

    public void associateZoneAndFormation()
    {
	Formation curForm;
	Zone curZone;
	curForm = (Formation) editor.treePanel.formView.getSelectedValue();
	curZone = (Zone) editor.getViewer().getSelections().getFirst();
	editing.addFormation(curForm, curZone);
	curZone.setID(curForm.toString());
	editor.getViewer().repaint();
    }

}
