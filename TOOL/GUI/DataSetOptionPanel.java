
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

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.io.File;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JTextField;

import TOOL.TOOL;
import TOOL.Data.DataSet;
import TOOL.Data.DataManager;
import TOOL.GUI.AutoTextField;

public class DataSetOptionPanel extends JPanel implements ActionListener {

    private DataManager manager;
    private DataSet dataset;

    private JTextField saveAsField;

    public DataSetOptionPanel(DataManager mgr, DataSet set) {
        manager = mgr;
        dataset = set;

        initLayout();
    }

    public void initLayout() {
        setLayout(new BoxLayout(this, BoxLayout.PAGE_AXIS));
        setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Color.black),
            BorderFactory.createEmptyBorder(5, 5, 5, 5)
            ));

        // Save options
        
        JPanel subPanel = new JPanel();

        subPanel.setLayout(new BoxLayout(subPanel, BoxLayout.LINE_AXIS));
        JButton save = new JButton("Save Data Set");
        save.setActionCommand("save");
        save.addActionListener(this);
        subPanel.add(save);

        subPanel.add(Box.createRigidArea(new Dimension(10, 10)));

        JButton saveAs = new JButton("Save as...");
        saveAs.setActionCommand("saveas");
        saveAs.addActionListener(this);
        subPanel.add(saveAs);

        subPanel.add(Box.createRigidArea(new Dimension(10, 10)));

        saveAsField = new AutoTextField();
        saveAsField.setActionCommand("saveas");
        saveAsField.addActionListener(this);
        subPanel.add(saveAsField);

        add(subPanel);
    }

    //
    // ActionListener contract
    //

    public void actionPerformed(ActionEvent e) {
        String cmd = e.getActionCommand();

        if (cmd.equals("save"))
            manager.saveCurrent();

        else if (cmd.equals("saveas")) {
            String path = saveAsField.getText();

            if (path.length() == 0)
                path = TOOL.CONSOLE.promptDirSave("Save DataSet As...", path);

            if (path != null)
                manager.saveCurrentAs(saveAsField.getText());
        }
    }
}
