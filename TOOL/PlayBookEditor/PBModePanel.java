package TOOL.PlayBookEditor;

import java.awt.*;
import javax.swing.*;
import javax.swing.undo.*;
import java.awt.event.*;
import java.util.Vector;
import TOOL.Image.PlayBookEditorViewer;


/**
 * Here we have a panel which is used for building different types of
 * editing modes for the PlayBook Editor
 *
 * @author Tucker Hermans
 */
public abstract class PBModePanel extends JPanel implements ActionListener {

    public static final String PLAYBOOK_MODE = "PlayBook Edit",
	STRATEGY_MODE = "Strategy Edit", FORMATION_MODE = "Formation Edit",
	ROLE_MODE = "Role Edit", SUB_ROLE_MODE = "SubRole Edit",
	TEST_MODE = "Test Mode";
    protected JLabel title, selectorTitle;
    protected JComboBox modeSelector;
    protected JSeparator separatorBar;
    protected PlayBookEditor editor;
    /**
     * Setups the top header for all ModePanels that are displayed
     */
    public PBModePanel(PlayBookEditor theEditor)
    {
	super(new GridLayout(22, 1));
	editor = theEditor;
	title = new JLabel("Editor Utilities");
	selectorTitle = new JLabel("Current Mode: ");
	separatorBar = new JSeparator();

	// Setup our selector
	Vector<String> myModes = new Vector<String>();
	//myModes.add(PLAYBOOK_MODE);
	myModes.add(STRATEGY_MODE);
	myModes.add(FORMATION_MODE);
	myModes.add(ROLE_MODE);
	myModes.add(SUB_ROLE_MODE);
	myModes.add(TEST_MODE);
	modeSelector = new JComboBox(myModes);
	modeSelector.setSelectedItem(PLAYBOOK_MODE);
	modeSelector.addActionListener(this);

	// Add our things
	add(title);
	add(separatorBar);
	add(selectorTitle);
	add(modeSelector);
    }

    public void actionPerformed(ActionEvent e)
    {
	// Perform mode selection
	String selection = (String) modeSelector.getSelectedItem();
	if (selection.equals(PLAYBOOK_MODE) ) {
	    changeMode(PLAYBOOK_MODE);
	    editor.playBookModePanel.modeSelector.setSelectedItem(PLAYBOOK_MODE);
	    editor.getViewer().setTestMode(false);
	} else if (selection.equals(STRATEGY_MODE)) {
	    changeMode(STRATEGY_MODE);
	    editor.strategyModePanel.modeSelector.setSelectedItem(STRATEGY_MODE);
	    editor.getViewer().setTestMode(false);
	} else if (selection.equals(FORMATION_MODE)) {
	    changeMode(FORMATION_MODE);
	    editor.formationModePanel.modeSelector.setSelectedItem(FORMATION_MODE);
	    editor.getViewer().setTestMode(false);
	} else if (selection.equals(ROLE_MODE)) {
	    changeMode(ROLE_MODE);
	    editor.roleModePanel.modeSelector.setSelectedItem(ROLE_MODE);
	    editor.getViewer().setTestMode(false);
	} else if (selection.equals(SUB_ROLE_MODE)) {
	    changeMode(SUB_ROLE_MODE);
	    editor.subRoleModePanel.modeSelector.setSelectedItem(SUB_ROLE_MODE);
	    editor.buttonPanel.line.setEnabled(false);
	    editor.buttonPanel.zone.setEnabled(false);
	    editor.buttonPanel.point.setEnabled(false);
	    editor.getViewer().setTestMode(false);
	} else if (selection.equals(TEST_MODE)) {
	    changeMode(TEST_MODE);
	    editor.testModePanel.modeSelector.setSelectedItem(TEST_MODE);
	    editor.buttonPanel.line.setEnabled(false);
	    editor.buttonPanel.zone.setEnabled(false);
	    editor.buttonPanel.point.setEnabled(false);
	    editor.getViewer().setTestMode(true);
	}
    }

    private void changeMode(String modeName)
    {
	CardLayout cl = (CardLayout)editor.modeSpecificPanel.getLayout();
	cl.show(editor.modeSpecificPanel, modeName);
    }

    public String getMode()
    {
	return (String)modeSelector.getSelectedItem();
    }
}
