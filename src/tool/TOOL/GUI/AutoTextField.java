
// This file is part of TOOL, a robotics interaction and development
// package created by the Northern Bites RoboCup team of Bowdoin College
// in Brunswick, Maine.
//
// TOOL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TOOL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TOOL.  If not, see <http://www.gnu.org/licenses/>.

package TOOL.GUI;

import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;

import java.io.File;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Vector;

import javax.swing.AbstractAction;
import javax.swing.JTextField;
import javax.swing.KeyStroke;

import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

import TOOL.TOOL;

public class AutoTextField extends JTextField implements DocumentListener {

    private class CompleteAction extends AbstractAction {
        private AutoTextField field;
        public CompleteAction(AutoTextField tf) {
            field = tf;
        }

        public void actionPerformed(ActionEvent e) {
            field.complete();
        }
    }

    private boolean changing;
    private int tabCount;
    private String partialText;

    public AutoTextField(String text) {
        super(text);

        // Bind TAB key to auto-complete functionality
        KeyStroke tabStroke = KeyStroke.getKeyStroke(KeyEvent.VK_TAB, 0);
        getInputMap(WHEN_FOCUSED).put(tabStroke, "auto-complete");
        getActionMap().put("auto-complete", new CompleteAction(this));
        //getKeymap().addActionForKeyStroke(tabStroke, new CompleteAction(this));

        // Listen for other key strokes, to reset tab count
        getDocument().addDocumentListener(this);
        
        changing = false;
        partialText = null;
        tabCount = 0;
    }

    public AutoTextField() {
        this("");
    }

    public void complete() {
        tabCount++;
        changing = true;

        String text = getText();
        if (text.length() == 0) {
            setText("./");
            partialText = text;
        
        }else {
            // get the (complete or partial) file path specified, the last
            // complete directory in the path, and the (partial) name of the 
            // target file or directory
            File path = new File(text);
            File dir = path.getParentFile();
            String name = path.getName();
            if (tabCount == 1) {
                if (path.isDirectory())
                    partialText = "";
                else
                    partialText = name;
            }

            if (tabCount == 2 && path.isDirectory())
                dir = path;

            if (dir == null)
                dir = new File(".");

            // list all files in the parent directory and match the typed name
            // against the beginning of each file/dir name
            String[] paths = dir.list();
            if (paths == null) {
                reset();
                return;
            }

            List<String> matches = new Vector<String>();
            for (String p : paths)
                if (!p.startsWith(".") && (name.length() == 0 ||
                        partialText == null || p.startsWith(partialText)))
                    matches.add(p);
            Collections.sort(matches);

            if (tabCount <= 1) {
                // The first time a tab is pressed, we will only complete if
                // there is an exact match to a specific file or directory
                
                if (path.isDirectory()) {
                    // make it clear that this is a directory
                    if (!text.endsWith("/"))
                        setText(text + "/");

                }else if (matches.size() == 1) {
                    // a match!  auto-complete the name of the dir/file and
                    // reset the tab count
                    File match = new File(dir, matches.get(0));
                    if (match.isDirectory())
                        setText(match.getPath() +
				System.getProperty("file.separator"));
                    else
                        setText(match.getPath());
                    reset();

                }

            }else {
                if (tabCount - 2 >= matches.size()) {
                    // end of list, reset to beginning
                    tabCount = 1;
                    if (partialText.length() == 0)
                        setText(dir.getPath() + '/');
                    else
                        setText(new File(dir, partialText).getPath());

                }else {
                    // auto-complete to the next option
                    setText(new File(dir,
                                     matches.get(tabCount - 2)
                                     ).getPath());
                }
            }
        }
        changing = false;
    }

    private void reset() {
        tabCount = 0;
        partialText = null;
    }

    //
    // Overridden methods
    //

    public boolean isManagingFocus() {
        return true;
    }

    //
    // DocumentListener contract
    //

    public void changedUpdate(DocumentEvent e) {
        if (!changing)
            reset();
    }

    public void insertUpdate(DocumentEvent e) {
        if (!changing)
            reset();
    }

    public void removeUpdate(DocumentEvent e) {
        if (!changing)
            reset();
    }
}

