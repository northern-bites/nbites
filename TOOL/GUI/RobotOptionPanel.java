
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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.io.File;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JTextField;
import javax.swing.JPanel;

import TOOL.TOOL;
import TOOL.Data.DataTypes;
import TOOL.Data.DataTypes.DataType;
import TOOL.Net.DataRequest;
import TOOL.Net.RemoteRobot;

public class RobotOptionPanel extends JPanel implements ActionListener {

    private static final String CHECKBOX_CMD = "checkbox";


    private RemoteRobot robot;
    private JButton disconnect;
    private Vector<JCheckBox> checkBoxes;

    public RobotOptionPanel(RemoteRobot r) {
        super();

        robot = r;

        DataRequest req = r.getRequestDetails();
        disconnect = new JButton("Disconnect");
        checkBoxes = new Vector<JCheckBox>();
        for (DataType t : DataType.values()) {
            JCheckBox box = new JCheckBox(DataTypes.title(t), req.get(t));
            box.setActionCommand(CHECKBOX_CMD + t.ordinal());
            box.addActionListener(this);
            box.setEnabled(req.isImplemented(t));

            checkBoxes.add(box);
        }

        initLayout();
    }

    public void initLayout() {
        //setLayout(new BorderLayout());
        setLayout(new BoxLayout(this, BoxLayout.PAGE_AXIS));
        setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Color.gray),
            BorderFactory.createEmptyBorder(10, 5, 10, 5))
            );

        add(new JLabel("Configuration Options for " + robot.name()));
        add(Box.createRigidArea(new Dimension(0, 10)));

        JPanel tmp = new JPanel();
        tmp.setLayout(new BoxLayout(tmp, BoxLayout.LINE_AXIS));
        tmp.add(new JLabel("Requested data:"));
        tmp.add(Box.createHorizontalGlue());

        add(tmp);

        JPanel boxPanel = new JPanel();
        int half = (int)((DataRequest.LENGTH + .5) / 2);
        boxPanel.setLayout(new GridLayout(half, 2));
        boxPanel.setMaximumSize(new Dimension(Integer.MAX_VALUE, 100));

        for (int i = 0; i < half; i++) {
            boxPanel.add(checkBoxes.get(i));
            if (i + half < checkBoxes.size())
                boxPanel.add(checkBoxes.get(i + half));
        }

        add(boxPanel);

        JPanel subPanel = new JPanel();
        subPanel.setLayout(new BoxLayout(subPanel, BoxLayout.LINE_AXIS));
        disconnect.setActionCommand("disconnect");
        disconnect.addActionListener(this);
        subPanel.add(disconnect);
        subPanel.add(Box.createHorizontalGlue());

        add(subPanel);

        add(Box.createGlue());

    }

    //
    // ActionListener contract
    //

    public void actionPerformed(ActionEvent e) {
        String cmd = e.getActionCommand();
        Object src = e.getSource();

        if (src == disconnect)
            robot.disconnect();

        else if (cmd.startsWith(CHECKBOX_CMD))
            for (DataType t : DataType.values())
                if (src == checkBoxes.get(t.ordinal()))
                    robot.getRequestDetails().flip(t);
    }

}
