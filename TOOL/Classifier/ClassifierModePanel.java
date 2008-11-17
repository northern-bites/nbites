package edu.bowdoin.robocup.TOOL.Classifier;

import javax.swing.JPanel;
import javax.swing.BoxLayout;
import java.awt.GridLayout;
import java.awt.Dimension;

import java.awt.BorderLayout;
import java.awt.Image;
import javax.swing.ButtonGroup;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import java.util.Observer;
import java.util.Observable;

import javax.swing.JComponent;

import javax.swing.InputMap;
import javax.swing.ActionMap;
import java.awt.event.InputEvent;

import javax.swing.AbstractAction;
import java.awt.event.ActionEvent;
import javax.swing.KeyStroke;
import java.awt.event.KeyEvent;
/**
 * @author Nicholas Dunn
 * @date   05/09/2008
 * An amalgamation of View/Controller.  It's view in the sense that it displays
 * information from the model (which mode we're in) but it's
 * a controller in the sense that it fires off commands to the model upon
 * press of the different buttons.
 */


public class ClassifierModePanel extends JPanel implements Observer {
  
    // The order of these buttons determines the order in which the buttons
    // are laid out in the panel
    private static enum Buttons { 
        SELECTION("Selection",
                  ClassifierModeButton.getSelectionImage(),
                  ClassifierModel.ClassifierMode.SELECTION),
            LINE("Line", ClassifierModeButton.getLineImage(),
                 ClassifierModel.ClassifierMode.LINE),
            BALL("Ball", ClassifierModeButton.getBallImage(),
                 ClassifierModel.ClassifierMode.BALL),
            

            CENTER_CIRCLE("Center Circle", 
                          ClassifierModeButton.getCenterCircleImage(),
                          ClassifierModel.ClassifierMode.CENTER_CIRCLE),
            YELLOW_ARC("Yellow Arc", ClassifierModeButton.getYellowArcImage(),
                       ClassifierModel.ClassifierMode.YELLOW_CORNER_ARC),
            BLUE_ARC("Blue Arc", ClassifierModeButton.getBlueArcImage(),
                     ClassifierModel.ClassifierMode.BLUE_CORNER_ARC),


            INNER_L_CORNER("Inner L Corner", 
                           ClassifierModeButton.getInnerLCornerImage(),
                           ClassifierModel.ClassifierMode.INNER_L_CORNER),
            OUTER_L_CORNER("Outer L Corner",
                           ClassifierModeButton.getOuterLCornerImage(),
                           ClassifierModel.ClassifierMode.OUTER_L_CORNER),
            T_CORNER("T Corner", ClassifierModeButton.getTCornerImage(),
                     ClassifierModel.ClassifierMode.T_CORNER),
            
           
            BLUE_RIGHT_POST("Blue Right Post", 
                            ClassifierModeButton.getBlueRightPostImage(),
                            ClassifierModel.ClassifierMode.BLUE_GOAL),
            BLUE_BACKSTOP("Blue Backstop",
                          ClassifierModeButton.getBlueBackstopImage(),
                          ClassifierModel.ClassifierMode.BLUE_GOAL),
            BLUE_LEFT_POST("Blue Left Post",
                           ClassifierModeButton.getBlueLeftPostImage(),
                           ClassifierModel.ClassifierMode.BLUE_GOAL),

            YELLOW_RIGHT_POST("Yellow Right Post", 
                       ClassifierModeButton.getYellowRightPostImage(),
                              ClassifierModel.ClassifierMode.YELLOW_GOAL),
            YELLOW_BACKSTOP("Yellow Backstop",
                            ClassifierModeButton.getYellowBackstopImage(),
                            ClassifierModel.ClassifierMode.YELLOW_GOAL),
            YELLOW_LEFT_POST("Yellow Left Post",
                             ClassifierModeButton.getYellowLeftPostImage(),
                             ClassifierModel.ClassifierMode.YELLOW_GOAL),

            YELLOW_BLUE_BEACON("Yellow Blue Beacon",
                               ClassifierModeButton.getYBBeaconImage(),
                               ClassifierModel.ClassifierMode.YELLOW_BLUE_BEACON),
            BLUE_YELLOW_BEACON("Blue Yellow Beacon",
                                ClassifierModeButton.getBYBeaconImage(),
                               ClassifierModel.ClassifierMode.BLUE_YELLOW_BEACON),
            DOG("Dog",
                null,
                ClassifierModel.ClassifierMode.DOG);
     private String toolTip;
     private Image image;
     private ClassifierModel.ClassifierMode mode;
     Buttons(String toolTip, Image image, ClassifierModel.ClassifierMode mode) {
         this.toolTip = toolTip;
         this.image = image;
         this.mode = mode;
     }
     public String getToolTip() { return toolTip; }
     public Image getImage() { return image; }
     public ClassifierModel.ClassifierMode getMode() { return mode; }
    };
    
        
    private ClassifierModel model;
    private ClassifierView view;
    private ClassifierModeButton[] buttons;
    private int selectedButtonIndex;

    public ClassifierModePanel(ClassifierModel model, ClassifierView view) {
        super();
        this.model = model;
        this.view = view;
        model.addObserver(this); // Be notified automatically of changes

        setLayout(new BorderLayout());

        // Used to create all of our ClassifierModeButtons
        Buttons[] values = Buttons.values();
        int numButtons = values.length;
        int numButtonsPerRow = 3;
        // round UP
        int numRows = (int) Math.ceil(1.0 * numButtons / numButtonsPerRow);

        JPanel buttonPanel = 
            new JPanel(new GridLayout(numRows, numButtonsPerRow));

        // Create the panel to have a little extra space in x and y dimensions
        // to space out the buttons
        Dimension panelSize =
            new Dimension((numButtonsPerRow + 1) * 
                          ClassifierModeButton.ICON_SIZE,
                          (numRows + 1) * ClassifierModeButton.ICON_SIZE);
        setPreferredSize(panelSize);
        setMaximumSize(panelSize);

        

        buttons = new ClassifierModeButton[numButtons];
        // Ensures that at most one button is ever selected
        ButtonGroup modes = new ButtonGroup();

        for (int i = 0; i < buttons.length; i++) {
            buttons[i] = new ClassifierModeButton(values[i].getToolTip(),
                                                  i + 1, values[i].getImage(),
                                                  values[i].getMode());
            modes.add(buttons[i]);
            buttonPanel.add(buttons[i]);
            buttons[i].addActionListener(new ModeButtonListener());
        }
        
        add(buttonPanel, BorderLayout.NORTH);

        setMode(model.getMode());
        // Only let the user click on the buttons if a data set has been loaded
        setEnabled(model.hasDataSetLoaded());

        setupShortcuts();
    }

    
    /**
     * Sets up the keyboard shortcuts so that corresponding number keys 
     * press the appropriately numbered button.
     */
    private void setupShortcuts() {
        InputMap im = this.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
        ActionMap am = this.getActionMap();

        int maxNumShortcutButtons = Math.min(9, Buttons.values().length);
        for (int i = 0; i < maxNumShortcutButtons; i++) {
            // Setup a link between the keystroke and a string we'll add to
            // an action map
            
            // standard 1-9 keys
            im.put(KeyStroke.getKeyStroke(KeyEvent.VK_1 + i, 0), "mode" + 
                   (i + 1));
            // numpad keys
            im.put(KeyStroke.getKeyStroke(KeyEvent.VK_NUMPAD1 + i, 0), "mode" + 
                   (i + 1));
            
            
            // Add the string from the input map to a corresponding action
            am.put("mode" + (i+1), new SetModeButtonAction(i));
        }
    }

    /**
     * Either enables or disables all the buttons 
     * (grays it out from being selected)
     */
    public void setEnabled(boolean choice) {
        for (int i = 0; i < buttons.length; i++) {
            buttons[i].setEnabled(choice);
        }
    }

    /**
     * Given a ClassifierMode, searches for the button with the corresponding
     * value 
     */
    public void setMode(ClassifierModel.ClassifierMode m) {
        Buttons values[] = Buttons.values();
        for (int i = 0; i < buttons.length; i++) {
            if (values[i].getMode() == m) {
                buttons[i].doClick();
                selectedButtonIndex = i;
            }
        }
    }

    /**
     * Programatically clicks button with index i.  
     * @precondition 0 <= i < buttons.length
     */
    public void setMode(int i) {
        buttons[i].doClick();
        selectedButtonIndex = i;
    }
   
    public void advanceMode(int i) {
        // Avoid the problem with negative mods in java by adding the buttons
        // length; we wrap around the end
        setMode((buttons.length + selectedButtonIndex + i) % buttons.length);
    }
    
    public int getNumButtons() {
        return buttons.length;
    }

    /**
     * Fulfills the Observer interface.  Called whenever our model changes and
     * we must refresh.
     */ 
    public void update(Observable o, Object arg) {
        ClassifierModel model = (ClassifierModel) o;
        setEnabled(model.hasDataSetLoaded());
    }

    /**
     * Every time one of our buttons is clicked, change the mode in the model
     */
    class ModeButtonListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            ClassifierModeButton pressed = (ClassifierModeButton) e.getSource();
            model.setMode(pressed.getMode());
        }
    }

    /**
     * Used to map the keyboard number pad to the buttons
     */
    class SetModeButtonAction extends AbstractAction {
        private int i;
        public SetModeButtonAction(int i) {
            this.i = i;
        }
        public void actionPerformed(ActionEvent e) {
            // Make sure user doesn't change modes in the middle of defining
            // a shape
            if (!view.isDrawing()) {
                setMode(i);
            }
        }
    }
}