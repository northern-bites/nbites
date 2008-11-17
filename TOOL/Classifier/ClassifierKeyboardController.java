package edu.bowdoin.robocup.TOOL.Classifier;

import java.awt.event.KeyListener;
import java.awt.event.KeyEvent;

public class ClassifierKeyboardController implements KeyListener {
    


    ClassifierModel model;

    public ClassifierKeyboardController(ClassifierModel model) {
        this.model = model;
    }

    public void keyPressed(KeyEvent e) {}
    public void keyReleased(KeyEvent e) {}
    public void keyTyped(KeyEvent e) {}


}