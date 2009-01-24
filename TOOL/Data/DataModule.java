
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

package TOOL.Data;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.io.File;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import TOOL.TOOL;
import TOOL.TOOLModule;
import TOOL.Image.ColorTable;
import TOOL.GUI.AutoTextField;

/**
 * The module that handles displaying and controlling the Source and Data
 * managers.
 *
 * ** Copied from TOOL.Net.RemoteRobot **
 * Also needing to be implemented is advanced memory handling.  This may be the
 * job of the underlying FileSet, not the RemoteRobot.  Currently all frames
 * are kept in memory unsaved until explicitly stored.  No memory-collection
 * and management features have been written.  The extra space in the Data
 * module and manager may be a great place for this.  In fact I'm copying this
 * comment there.
 *
 */

public class DataModule extends TOOLModule {

    public static final int SKIP_AMOUNT = 10;

    //modes for opening an open dialog
    private static final int FILE = 1;
    private static final int DIR = 2;

    public static final int BUTTON_Y = 0;
    public static final double BUTTON_WEIGHT_X = 1;
    public static final double BUTTON_WEIGHT_Y = 1;
    //public static final int BUTTON_WIDTH = 1;
    //public static final int BUTTON_HEIGHT = 1;
    public static final int LIST_PANELS_Y = 1;
    public static final double LIST_PANELS_WEIGHT_X = 2;
    public static final double LIST_PANELS_WEIGHT_Y = 9;
    //public static final int LIST_PANELS_WIDTH = 2;
    //public static final int LIST_PANELS_HEIGHT = 9;

    private DataManager dataManager;
    private SourceManager sourceManager;

    private JPanel displayPanel;
    private JPanel optionPanel;

    private JTextField sourcePathField;

    public DataModule(TOOL tool) {
        super(tool);

        dataManager = t.getDataManager();
        sourceManager = t.getSourceManager();

        dataManager.addDataListener(this);
        sourceManager.addSourceListener(this);

        initDisplay();

        t.setColorTable(new ColorTable(t.DEFAULT_TABLE_PATH));
        t.getFrame().addKeyListener(this);
    }

    public void initDisplay() {

        displayPanel = new JPanel();
        displayPanel.setLayout(new BorderLayout());

        // Buttons panel

        JPanel subPanel = new JPanel();
        subPanel.setLayout(new BoxLayout(subPanel, BoxLayout.LINE_AXIS));

        JButton b = new JButton("Add Source");
        b.setActionCommand("add_source");
        b.addActionListener(this);
        b.setFocusable(false);
        subPanel.add(b);
        subPanel.add(Box.createRigidArea(new Dimension(5, 5)));

        sourcePathField = new AutoTextField();
        sourcePathField.setActionCommand("add_source");
        sourcePathField.addActionListener(this);
        subPanel.add(sourcePathField);

        displayPanel.add(subPanel, BorderLayout.PAGE_START);

        // List Panels

        subPanel = new JPanel();
        subPanel.setLayout(new GridLayout(1, 2));

        subPanel.add(sourceManager.getListPanel());
        subPanel.add(dataManager.getListPanel());
        displayPanel.add(subPanel, BorderLayout.CENTER);

        // Option Panel

        optionPanel = new JPanel();
        optionPanel.setLayout(new GridLayout(1, 1));
        displayPanel.add(optionPanel, BorderLayout.PAGE_END);

    }

    public String getDisplayName() {
        return "Data";
    }

    public Component getDisplayComponent() {
        return displayPanel;
    }

    //ask the user for a new directory source location
    public void addSource() {
        String path = sourcePathField.getText();

        if (path.length() == 0)
            path = TOOL.CONSOLE.promptDirOpen("Add Source", path);

        if (path != null)
            sourceManager.addSource(path);
    }

    //
    // ActionListener contract (overloaded)
    //

    public void actionPerformed(ActionEvent e) {
        if (e.getActionCommand().equals("add_source"))
            addSource();
        else
            super.actionPerformed(e);
    }

    //
    // DataListener contract
    //

    public void notifyDataSet(DataSet s, Frame f) {
        super.notifyDataSet(s, f);

        optionPanel.removeAll();
        optionPanel.add(dataManager.getOptionPanel());
        displayPanel.validate();
    }

    //
    // KeyListener contract
    //
    public void keyPressed(KeyEvent e) {
        switch (e.getKeyCode()) {
            //case KeyEvent.VK_D:
            //    dataManager.next();
            //    break;
            //case KeyEvent.VK_S:
            //    dataManager.last();
            //    break;
            case KeyEvent.VK_F:
                dataManager.advance(SKIP_AMOUNT);
                break;
            case KeyEvent.VK_A:
                dataManager.advance(-SKIP_AMOUNT);
                break;
        }
    }

    //
    // SourceListener contract
    //
    
    public void sourceChanged(DataSource src) {
        super.sourceChanged(src);
    }

}
