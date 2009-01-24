package TOOL.Classifier;

import TOOL.TOOL;


import javax.swing.JPanel;
import java.awt.BorderLayout;

import TOOL.Image.DefaultDrawingPanel;

/**
 * @author Nicholas Dunn
 * @date   05/07/2008
 *
 * Combines the three elements of the ModelViewController relationship into
 * a single component which we can add to our TOOL.
 */

public class ClassifierMVC extends JPanel{
    
    protected ClassifierModel model;
    protected ClassifierView view;
    protected ClassifierController controller;
    protected ClassifierButtonPanel buttons;
    protected ClassifierModePanel modePanel;
    
    public ClassifierMVC(TOOL t) {
        model = new ClassifierModel(t);
        view = new ClassifierView(model);


        modePanel = new ClassifierModePanel(model, view);
        buttons = new ClassifierButtonPanel(model, controller);
        controller = new ClassifierController(model, view, modePanel, buttons);

        
        setupWindows();
        setupListeners();
    }
    
    private void setupWindows() {
        setLayout(new BorderLayout());
        add(view, BorderLayout.CENTER);

        
        add(buttons, BorderLayout.SOUTH);
        add(modePanel, BorderLayout.EAST);
    }

    private void setupListeners() {
        
        view.addMouseListener(controller);
        view.addMouseMotionListener(controller);
        addMouseWheelListener(controller);
    }

    public ClassifierModel getModel() { return model; }

    public ClassifierView getView() { return view; }
    
    public ClassifierController getController() { return controller; }

    public ClassifierButtonPanel getButtonPanel() { return buttons; }
}