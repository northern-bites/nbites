package TOOL.Classifier;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class ClassifierButtonController implements ActionListener {
    
    ClassifierModel model;

    public ClassifierButtonController(ClassifierModel model) {
        this.model = model;
    }

    public void actionPerformed(ActionEvent e) {}
    
}