package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import javax.swing.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import java.io.*;
import edu.bowdoin.robocup.TOOL.TOOL;
import edu.bowdoin.robocup.TOOL.Image.PlayBookEditorViewer;
import edu.bowdoin.robocup.TOOL.Data.Field;
import edu.bowdoin.robocup.TOOL.Data.AiboField2008;
import edu.bowdoin.robocup.TOOL.Data.NaoField2008;
import java.util.ListIterator;
import java.util.LinkedList;

/**
 * @author Tucker Hermans
 *
 * Visual Editor for quickly building components of the coordinated
 * behavior tree
 *
 * Tree Components are of the types: Strategy, Formation, Role, and SubRole
 */


public class PlayBookEditor extends JPanel implements MouseListener,
						      ActionListener
{
    // Constants
    protected final static String NO_SELECTION = "NONE";
    protected final static String EDITING_PROMPT = "Currently Editing: ";
    protected final static String SELECT_PROMPT = "Current Selection: ";
    protected final static String AIBO_PB_PATH ="../../trunk/dog/pycode/Coop/";
    protected final static String NAO_PB_PATH ="../../trunk/man/noggin/playBook/";
    // Tool things
    protected TOOL tool;

    // Components of the drawn image
    protected JLabel currentSelectionLabel, currentlyEditingLabel;
    protected String selected, currentEdit;
    protected BehaviorTree treePanel;
    protected PlayBookEditorViewer fieldPanel;
    protected PlayBookEditorPanel buttonPanel;

    // All of our mode panels
    protected JPanel modeSpecificPanel, selectionsPanel;
    protected PBTestModePanel testModePanel;
    protected PBPlayBookModePanel playBookModePanel;
    protected PBStrategyModePanel strategyModePanel;
    protected PBFormationModePanel formationModePanel;
    protected PBRoleModePanel roleModePanel;
    protected PBSubRoleModePanel subRoleModePanel;

    // Important things for the handling of playbook data
    protected Field field;
    protected AiboField2008 aiboField;
    protected NaoField2008 naoField;
    protected Ball ball;
    protected PlayBook playBook;
    protected ComponentLibrary playBookComponents;

    // Constructor and setup methods
    /**
     * Constructor requires a TOOL module be passed in.
     * @param t The tool object of which we will be a member
     */
    public PlayBookEditor(TOOL t) {
        super();
        tool = t;
        setupWindowsAndListeners();
	setupPlayBook();
    }

    /**
     * Initialize the windows, add key and mouse listeners
     */
    protected void setupWindowsAndListeners() {
        // Initialize components
	// Setup the overhead information section
	selectionsPanel = new JPanel(new GridLayout(2,1));
	currentEdit = NO_SELECTION;
	currentlyEditingLabel = new JLabel(EDITING_PROMPT + currentEdit,
					   JLabel.CENTER);
	selected = NO_SELECTION;
	currentSelectionLabel = new JLabel("Current Selection: " + selected,
					   JLabel.CENTER);

	// Setup the main field area
        aiboField = new AiboField2008();
        naoField = new NaoField2008();
        field = aiboField;
	fieldPanel = new PlayBookEditorViewer(tool, field, this);

	// Setup the bottom field drawing components
        buttonPanel = new PlayBookEditorPanel(this, fieldPanel);

	// Setup the left hand display panel
	treePanel = new BehaviorTree();

	// Setup the right hand editing panel
	playBookModePanel = new PBPlayBookModePanel(this);
	strategyModePanel = new PBStrategyModePanel(this);
	formationModePanel = new PBFormationModePanel(this);
	roleModePanel = new PBRoleModePanel(this);
	subRoleModePanel = new PBSubRoleModePanel(this);
	testModePanel = new PBTestModePanel(this);

	modeSpecificPanel = new JPanel(new CardLayout());
	modeSpecificPanel.add(playBookModePanel, "PlayBook Edit");
	modeSpecificPanel.add(strategyModePanel, "Strategy Edit");
	modeSpecificPanel.add(formationModePanel, "Formation Edit");
	modeSpecificPanel.add(roleModePanel, "Role Edit");
	modeSpecificPanel.add(subRoleModePanel, "SubRole Edit");
	modeSpecificPanel.add(testModePanel, "Test Mode");

	selectionsPanel.add(currentlyEditingLabel);
	selectionsPanel.add(currentSelectionLabel);
	// Setup the layout and add the components
	setLayout(new BorderLayout(30,30));
	add(selectionsPanel, BorderLayout.NORTH);
	add(treePanel, BorderLayout.WEST);
	add(fieldPanel, BorderLayout.CENTER);
        add(buttonPanel, BorderLayout.SOUTH);
	fieldPanel.repaint();
	add(modeSpecificPanel, BorderLayout.EAST);

	// Associate event listeners
        addMouseListener(this);
    }

    /**
     * Setup the PlayBook and associated objects
     */
    protected void setupPlayBook()
    {

	// Here we have all the possible components to load into a play book
	playBookComponents = new ComponentLibrary(AIBO_PB_PATH);

	// Temp things for testing Tree Building
	playBook = new PlayBook("Let's Play");
	Strategy win = new Strategy("Win");
	win.addFormation(new Formation("Kick Ass"),
			 new Zone("Ass Zone", 250, 250, 400, 500, field));
	playBook.addStrategy(win);
	playBook.addStrategy(new Strategy("Tie"));
	ball = new Ball((int)field.CENTER_FIELD_X, (int)field.CENTER_FIELD_Y);

	// Recursively read in our PlayBook to be able to be viewed
	try {
	    treePanel.loadPlayBook(this);
	} catch (IOException e) {
	    System.err.println("Error reading in PlayBook"+ playBook.getID());
        }
    }

    // Setters
    public void setLabelText(String s) {
        if (currentSelectionLabel != null) {
	    if ( s.length() < 3) {
		currentSelectionLabel.setText(SELECT_PROMPT + NO_SELECTION);
	    } else {
		currentSelectionLabel.setText(s);
	    }
        }
    }

    /**
     * Set the text of the currently editing
     */
    public void setEditingText(String s)
    {
        if (currentlyEditingLabel != null) {
	    currentlyEditingLabel.setText(EDITING_PROMPT + s);
	}
    }

    /**
     * Method to switch between Aibo and Nao fields
     */
    public void switchFields() {
        if (field == aiboField) {
            field = naoField;
        }
        else if (field == naoField) {
            field = aiboField;
        }
        fieldPanel.setField(field);
    }

    public PlayBookEditorViewer getViewer() { return fieldPanel; }

    public PlayBookEditorPanel getPanel() { return buttonPanel; }

    public BehaviorTree getTree() {return treePanel;}

    public Field getField() {return field;}
    // Listener Required Methods
    public void actionPerformed(ActionEvent e) {}
    public void mousePressed(MouseEvent e) {}
    public void mouseReleased(MouseEvent e) {}
    public void mouseClicked(MouseEvent e) {}
    public void mouseEntered(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}

    public void refreshButtons() {
        buttonPanel.refreshButtons();
    }

    public PlayBook getPlayBook() {
	return playBook;
    }

    public ComponentLibrary getComponentLibrary() {
	return playBookComponents;
    }

    public Ball getBall() { return ball;}

    public void setBallVisible(boolean visible)
    {
	fieldPanel.setBallVisible(visible);
    }

    public String getMode() {return subRoleModePanel.getMode();}
    public PBTestModePanel getTestPanel() {return testModePanel;}
    public PBSubRoleModePanel getSubRolePanel() {return subRoleModePanel;}
}
