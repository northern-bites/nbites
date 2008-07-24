package TOOL.Classifier;

import javax.swing.*;
import javax.swing.undo.*;
import java.util.Observer;
import java.util.Observable;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.MouseWheelListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.KeyListener;
import java.awt.event.KeyEvent;
import java.awt.GridLayout;

import javax.swing.InputMap;
import javax.swing.ActionMap;
import java.awt.event.InputEvent;
import javax.swing.KeyStroke;
import javax.swing.AbstractAction;
import TOOL.GUI.SpecialStrings;

/**
 * @author Nicholas Dunn
 * @date   05/09/2008
 * An amalgamation of View/Controller.  It's view in the sense that it displays
 * information from the model (e.g. the state of the undo manager) but it's
 * a controller in the sense that it fires off commands to the model upon
 * press of the different buttons.
 *
 * Please note that pressing any button will never edit any of the view 
 * components directly (e.g. pressing next doesn't increment the number in
 * the frame counter).  Instead we rely on the model to call updateObservers()
 * and then we query the model for the information we need.
 */



public class ClassifierButtonPanel extends JPanel implements Observer { 

    
    // We query the model to determine what to display on our buttons and text
    // fields
    private ClassifierModel model;
    
    // The controller is what actually reacts to all the buttons
    private ClassifierController controller;
    private JTextArea echo;
    private JButton clear, delete, undo, redo, saveLog, jumpToButton, 
        next, previous;
    private JTextField jumpToFrame;
    
    public ClassifierButtonPanel(ClassifierModel model, 
                                 ClassifierController controller) {
        super();
        this.model = model;
        this.controller = controller;
        model.addObserver(this); // Be notified automatically of changes

        setupWindow();
        addActionListeners();

        setupShortcuts();

        // Start off with everything disabled; they will be
        // enabled when a data set is loaded
        setEnabled(false);
    }


    public void setupWindow() {
        echo = new JTextArea("Classifier V 2.0");
        echo.setEditable(false);
        echo.setBackground(getBackground());


        String metaChar = SpecialStrings.getCtrlOrMeta();
        if (metaChar.equals(SpecialStrings.CTRL)) {
            metaChar += "+";
        }

        next = new JButton("Next");
        next.setToolTipText("Advances to the next frame");

        previous = new JButton("Previous");
        previous.setToolTipText("Goes to the previous frame");


        clear = new JButton("Clear (" + metaChar + "D)");
        clear.setToolTipText("Clears all classified objects from the " +
                             "current frame");

        delete = new JButton("Delete (" + SpecialStrings.getDelete() + ")");
        delete.setToolTipText("Deletes the currently selected classified "
                              + "object from the current frame");

        jumpToFrame = new JTextField("0", 4);
        jumpToButton = new JButton("Jump");

        undo = new JButton("Undo (" + metaChar + "Z)");
        undo.setToolTipText("Undo the last action");
        redo = new JButton("Redo (" + metaChar + "Y)");
        redo.setToolTipText("Redo the last action");
        saveLog = new JButton("Save Log");
        saveLog.setToolTipText("Save a text file representation of the " +
                               "classified images");

        JPanel buttons = new JPanel(new GridLayout(5, 2));
        /*
        buttons.add(previous);
        buttons.add(next);
        
        buttons.add(undo);
        buttons.add(redo);

        buttons.add(jumpToFrame);
        buttons.add(jumpToButton);

        buttons.add(clear);
        buttons.add(delete);
        buttons.add(saveLog);
        */

        add(echo);

        add(previous);
        add(next);
        
        add(undo);
        add(redo);

        add(jumpToFrame);
        add(jumpToButton);

        add(clear);
        add(delete);
        add(saveLog);


        //        add(buttons);

        
    }

    public void setupShortcuts() {
        InputMap im = this.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
        
        KeyStroke metaZ = KeyStroke.getKeyStroke(KeyEvent.VK_Z,
                                                 InputEvent.META_MASK);
        KeyStroke controlZ = KeyStroke.getKeyStroke(KeyEvent.VK_Z,
                                                    InputEvent.CTRL_MASK);
        KeyStroke metaY = KeyStroke.getKeyStroke(KeyEvent.VK_Y,
                                                 InputEvent.META_MASK);
        KeyStroke controlY = KeyStroke.getKeyStroke(KeyEvent.VK_Y,
                                                    InputEvent.CTRL_MASK);

        KeyStroke metaA = KeyStroke.getKeyStroke(KeyEvent.VK_A,
                                                 InputEvent.META_MASK);
        KeyStroke controlA = KeyStroke.getKeyStroke(KeyEvent.VK_A,
                                                    InputEvent.CTRL_MASK);


        im.put(metaA, "selectAll");
        im.put(controlA, "selectAll");
        
        im.put(metaZ, "undo");
        im.put(controlZ, "undo");

        im.put(metaY, "redo");
        im.put(controlY, "redo");

        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0), "escape");
        
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_DELETE, 0), "delete");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_BACK_SPACE, 0), "delete");
        
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_D, InputEvent.META_MASK),
               "clear");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_D, InputEvent.CTRL_MASK), 
               "clear");

        ActionMap am = this.getActionMap();
        am.put("selectAll", new AbstractAction("selectAll") {
                public void actionPerformed(ActionEvent e) {
                    model.selectAll();
                }
            });
        am.put("escape", new AbstractAction("escape") {
                public void actionPerformed(ActionEvent e) {
                    model.clearSelected();
                }
            });

        am.put("undo", new AbstractAction("undo") {
                public void actionPerformed(ActionEvent e) {
                    undo.doClick();
                }
            });
        
        am.put("redo", new AbstractAction("redo") {
                public void actionPerformed(ActionEvent e) {
                    redo.doClick();
                }
            });

        am.put("clear", new AbstractAction("clear") {
                public void actionPerformed(ActionEvent e) {
                    clear.doClick();
                }
            });
        
        am.put("delete", new AbstractAction("delete") {
                public void actionPerformed(ActionEvent e) {
                    delete.doClick();
                }
            });
    }

    public void setText(String s) {
        echo.setText(s);
    }


    public void addActionListeners() {
        next.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    model.getNextImage();
                }
            });
        previous.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    model.getLastImage();
                }
            });
        clear.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    model.clear();
                }
            });
        delete.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    model.remove(model.getSelected());
                }
            });
        undo.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    model.undo();
                }
            });
        redo.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    model.redo();
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
                        model.setImage(newIndex);
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
     * Fulfills the Observer interface.  Called whenever our model changes and
     * we must refresh.
     */ 
    public void update(Observable o, Object arg) {
        ClassifierModel model = (ClassifierModel) o;

        jumpToFrame.setEnabled(model.hasDataSetLoaded());
        jumpToButton.setEnabled(model.hasDataSetLoaded());

        jumpToFrame.setText("" + model.getCurrentImageIndex());

        // Refresh clear/delete buttons
        clear.setEnabled(model.hasClassifiedObjects());
        delete.setEnabled(model.hasSelectedObjects());

        // Refresh the undo/redo buttons
        undo.setEnabled(model.canUndo());
        redo.setEnabled(model.canRedo());
        undo.setText(model.getUndoText());
        redo.setText(model.getRedoText());
        
        // Refresh save log button
        saveLog.setEnabled(model.hasDataSetLoaded());
       
    }

    
    /**
     *
     */
    public void setEnabled(boolean choice) {
        jumpToFrame.setEnabled(choice);
        jumpToButton.setEnabled(choice);
        clear.setEnabled(choice);
        delete.setEnabled(choice);
        undo.setEnabled(choice);
        redo.setEnabled(choice);
        saveLog.setEnabled(choice);
    }

   
}
