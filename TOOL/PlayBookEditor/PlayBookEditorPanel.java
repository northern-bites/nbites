package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import javax.swing.*;
import javax.swing.undo.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.InputMap;
import javax.swing.ActionMap;
import java.awt.event.InputEvent;
import javax.swing.KeyStroke;
import javax.swing.AbstractAction;

import edu.bowdoin.robocup.TOOL.Image.PlayBookEditorViewer;


/**
 * Holds buttons for PlayBookEditor.
 * @author Nicholas Dunn
 */
public class PlayBookEditorPanel extends JPanel {

    protected JButton copy, paste, clear, delete, undo, redo, switchFields;
    protected JRadioButton zone, line, point;
    protected JComboBox modeSelector;
    protected PlayBookEditor editor;
    protected PlayBookEditorViewer viewer;
    protected UndoManager undoManager;

    public PlayBookEditorPanel(PlayBookEditor editor,
                               PlayBookEditorViewer viewer) {
        super(new GridLayout(2,6));

        this.editor = editor;
        this.viewer = viewer;

        clear = new JButton("Clear all (D)");
        clear.setMnemonic('d');

        delete = new JButton("Remove (Delete)");

        copy = new JButton("Copy (C)");
        copy.setMnemonic('c');

        paste = new JButton("Paste (V)");
        copy.setMnemonic('v');

        undo = new JButton("Undo (Z)");
        undo.setMnemonic('z');

        redo = new JButton("Redo (Y)");
        redo.setMnemonic('y');

        zone = new JRadioButton("Zone mode");
        line = new JRadioButton("Line mode");
	point = new JRadioButton("Point mode");
        switchFields = new JButton("Switch Fields");

        ButtonGroup mode = new ButtonGroup();
        mode.add(zone);
        mode.add(line);
	mode.add(point);
	zone.setSelected(true);

        add(copy);
        add(paste);
        add(clear);
        add(delete);
        add(undo);
        add(redo);
        add(zone);
        add(line);
	add(point);
        add(switchFields);

        undoManager = viewer.getUndoManager();
        refreshButtons();
        setupListeners();
        setupShortcuts();
    }

    public void refreshButtons() {
        clear.setEnabled(viewer.canClear());
        delete.setEnabled(viewer.canDelete());

        copy.setEnabled(viewer.canCopy());
        paste.setEnabled(viewer.canPaste());


        // Set undo text to reflect the last move done
        undo.setText(undoManager.getUndoPresentationName());
        // Let them press the button if and only if there are moves to undo
        undo.setEnabled(undoManager.canUndo());

        redo.setText(undoManager.getRedoPresentationName());
        redo.setEnabled(undoManager.canRedo());

    }

    private void setupListeners() {
        copy.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    viewer.copy();
                }
            });

        paste.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    viewer.paste();
                }
            });
        clear.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    viewer.clear();
                }
            });
        delete.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    viewer.delete();
                }
            });
        undo.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    viewer.undo();
                }
            });
        redo.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    viewer.redo();
                }
            });
        zone.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                     viewer.setMode(PlayBookEditorViewer.DrawingMode.ZONE);
                }
            });
        line.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    viewer.setMode(PlayBookEditorViewer.DrawingMode.LINE);
                }
            });
        point.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    viewer.setMode(PlayBookEditorViewer.DrawingMode.POINT);
                }
            });

        switchFields.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    editor.switchFields();
                }
            });
    }

    /**
     * Links keyboard presses to actions (mainly to clicking on buttons)
     */
    private void setupShortcuts() {
        InputMap im = this.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
        KeyStroke metaZ = KeyStroke.getKeyStroke(KeyEvent.VK_Z,
                                                 InputEvent.META_MASK);
        KeyStroke controlZ = KeyStroke.getKeyStroke(KeyEvent.VK_Z,
                                                    InputEvent.CTRL_MASK);
        KeyStroke metaY = KeyStroke.getKeyStroke(KeyEvent.VK_Y,
                                                 InputEvent.META_MASK);
        KeyStroke controlY = KeyStroke.getKeyStroke(KeyEvent.VK_Y,
                                                    InputEvent.CTRL_MASK);
        im.put(metaZ, "undo");
        im.put(controlZ, "undo");

        im.put(metaY, "redo");
        im.put(controlY, "redo");

        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_C, InputEvent.META_MASK),
               "copy");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_C, InputEvent.CTRL_MASK),
               "copy");

        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_V, InputEvent.META_MASK),
               "paste");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_V, InputEvent.CTRL_MASK),
               "paste");

        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_D, InputEvent.CTRL_MASK),
	       "clear");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_D, InputEvent.META_MASK),
	       "clear");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_DELETE, 0), "delete");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_BACK_SPACE, 0), "delete");

        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, 0), "moveUp");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_KP_UP, 0), "moveUp");

        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_LEFT, 0), "moveLeft");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_KP_LEFT, 0), "moveLeft");

        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_RIGHT, 0), "moveRight");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_KP_RIGHT, 0), "moveRight");

        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, 0), "moveDown");
        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_KP_DOWN, 0), "moveDown");

        ActionMap am = this.getActionMap();
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
        am.put("copy", new AbstractAction("copy") {
                public void actionPerformed(ActionEvent e) {
                    copy.doClick();
                }
            });
        am.put("paste", new AbstractAction("paste") {
                public void actionPerformed(ActionEvent e) {
                    paste.doClick();
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
        am.put("moveUp", new AbstractAction("moveUp") {
                public void actionPerformed(ActionEvent e) {
                    viewer.moveUp();
                }
            });
        am.put("moveLeft", new AbstractAction("moveLeft") {
                public void actionPerformed(ActionEvent e) {
                    viewer.moveLeft();
                }
            });
        am.put("moveRight", new AbstractAction("moveRight") {
                public void actionPerformed(ActionEvent e) {
                    viewer.moveRight();
                }
            });
        am.put("moveDown", new AbstractAction("moveDown") {
                public void actionPerformed(ActionEvent e) {
                    viewer.moveDown();
                }
            });
    }
}
