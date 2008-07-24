
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

package TOOL.Net;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.event.ActionEvent;

import java.util.HashMap;
import java.util.Map;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;

import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;

import TOOL.TOOL;
import TOOL.Data.DataTypes;
import TOOL.Data.DataTypes.DataType;
import TOOL.Image.ImagePanel;
import TOOL.Image.TOOLImage;
import TOOL.Net.DataRequest;

import TOOL.TOOLModule;

public class RobotViewModule extends TOOLModule implements PopupMenuListener {

    private NetworkModule net;

    private JPanel displayPanel;
    private Map<JButton, DataType> typeMap;
    private Map<JMenuItem, RemoteRobot> robotMap;
    private JButton robotButton;
    private JPopupMenu robotMenu;

    private ImagePanel imagePanel;

    private RemoteRobot selectedRobot;

    public RobotViewModule(TOOL t, NetworkModule net_mod) {
        super(t);

        net = net_mod;
        displayPanel = new JPanel();
        typeMap = new HashMap<JButton, DataType>();
        robotMap = new HashMap<JMenuItem, RemoteRobot>();
        imagePanel = new ImagePanel();

        selectedRobot = null;

        initLayout();
    }

    public String getDisplayName() {
        return "Robot";
    }

    public Component getDisplayComponent() {
        return displayPanel;
    }

    private void initLayout() {
        displayPanel.setLayout(new BoxLayout(displayPanel,
                                             BoxLayout.LINE_AXIS));

        JPanel subPanel = new JPanel();
        subPanel.setLayout(new BoxLayout(subPanel, BoxLayout.PAGE_AXIS));

        robotButton = new JButton("Robot");
        robotButton.addActionListener(this);
        subPanel.add(robotButton);
        robotMenu = new JPopupMenu();
        robotMenu.addPopupMenuListener(this);
        subPanel.add(robotMenu);
        subPanel.add(Box.createRigidArea(new Dimension(10, 10)));

        subPanel.add(new JLabel("Update:"));
        subPanel.add(Box.createRigidArea(new Dimension(10, 10)));

        for (DataType t : DataTypes.types()) {
            JButton b = new JButton(DataTypes.title(t));
            typeMap.put(b, t);
            b.addActionListener(this);
            subPanel.add(b);
        }

        displayPanel.add(subPanel);

        subPanel = new JPanel();
        subPanel.setLayout(new BoxLayout(subPanel, BoxLayout.PAGE_AXIS));
        subPanel.add(Box.createHorizontalGlue());
        subPanel.add(imagePanel);

        displayPanel.add(subPanel);

    }

    public void retrieveType(DataType t) {
        if (selectedRobot == null)
            return;

        TOOLImage i;
        switch (t) {
            case IMAGE:
                TOOL.CONSOLE.message("Requesting a raw image");
                i = selectedRobot.retrieveImage();
                if (i != null)
                    imagePanel.updateImage(i);
                break;
            case THRESH:
                TOOL.CONSOLE.message("Requesting a thresholded image");
                i = selectedRobot.retrieveThresh();
                if (i != null)
                    imagePanel.updateImage(i);
                break;
        }
    }

    //
    // ActionListener contract
    //

    public void actionPerformed(ActionEvent e) {

        if (e.getSource() instanceof JButton) {
            JButton b = (JButton)e.getSource();
            if (b == robotButton) {
                // show popup menu for selecting robots
                robotMenu.show(b, b.getWidth(), 0);
            }else {
                if (!typeMap.containsKey(b))
                    // error?
                    return;
                // retrieve the type of data selected
                retrieveType(typeMap.get(b));
            }
        }else if (e.getSource() instanceof JMenuItem) {
            JMenuItem m = (JMenuItem)e.getSource();
            if (!robotMap.containsKey(m))
                // error?
                return;
            // select the given robot for future data retrievals
            selectedRobot = robotMap.get(m);
            robotButton.setText(selectedRobot.name());
            robotMenu.setVisible(false);
        }
    }

    //
    // PopupMenuListener contract
    //

    public void popupMenuCanceled(PopupMenuEvent e) {
    }

    public void popupMenuWillBecomeInvisible(PopupMenuEvent e) {
        robotMenu.removeAll();
    }

    public void popupMenuWillBecomeVisible(PopupMenuEvent e) {
        boolean none = true;
        robotMap.clear();
        for (RemoteRobot robot : net.getRobots()) {
            none = false;
            JMenuItem b = new JMenuItem(robot.name());
            robotMap.put(b, robot);
            b.addActionListener(this);
            robotMenu.add(b);
        }

        if (none)
            robotMenu.add("None available");
    }
}
