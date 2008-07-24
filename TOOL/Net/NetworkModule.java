
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

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import java.io.File;
import java.net.InetAddress;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import TOOL.TOOL;
import TOOL.TOOLModule;
import TOOL.Data.DataSet;
import TOOL.Data.DataSource;
import TOOL.Data.SourceHandler;
import TOOL.GUI.DataSetObject;
import TOOL.GUI.ListPanel;
import TOOL.GUI.ListObject;
import TOOL.GUI.RobotOptionPanel;

public class NetworkModule extends TOOLModule implements DataSource,
        ItemListener, RobotListener, SourceHandler {

    public static final int INTERFACE_ROW_HEIGHT = 35;
    public static final int ADDRESS_ROW_HEIGHT = 35;
    public static final int ROBOT_ROW_HEIGHT = 40;

    // Display name for the module
    public static final String NET_MODULE_NAME = "Network";
    // String representation of the path to the network source
    public static final String REMOTE_PATH = "net://";
    // Default file folder location for stored images
    public static final String DEFAULT_STORAGE_PATH = "robots";

    public static final String START_TEXT = "Start";
    public static final String STOP_TEXT = "Stop";
    public static final String REFRESH_TEXT = "Refresh";

    private TOOLNet net;

    private JPanel displayPanel;
    private ListPanel<ListObject> intPanel;
    private ListPanel<ListObject> addrPanel;
    private ListPanel<DataSetObject> robotPanel;
    private JTextField hostEntry;
    // Robot-specific options
    private JPanel optionPanel;
    private Vector<RobotOptionPanel> robotPanels;

    private JButton start, stop, refresh;

    private List<ListObject> intObjects;
    private Map<ListObject, InetAddress> addrObjects;
    private List<RemoteRobot> robots;
    private List<DataSetObject> robotObjects;

    private String storagePath;

    public NetworkModule(TOOL t) {
        super(t);

        net = new TOOLNet();
        net.addRobotListener(this);
        net.start();

        storagePath = DEFAULT_STORAGE_PATH;

        intObjects = new Vector<ListObject>();
        addrObjects = new HashMap<ListObject, InetAddress>();
        robots = new Vector<RemoteRobot>();
        robotObjects = new Vector<DataSetObject>();
        robotPanels = new Vector<RobotOptionPanel>();

        initDisplay();

        t.getSourceManager().DEFAULT_NET_HANDLER = this;
        t.getSourceManager().addSource(REMOTE_PATH);
    }

    private void initDisplay() {
        initListPanels();

        start = new JButton(START_TEXT);
        stop = new JButton(STOP_TEXT);
        refresh = new JButton(REFRESH_TEXT);
        start.setActionCommand(TOOLNet.START_ACTION);
        stop.setActionCommand(TOOLNet.STOP_ACTION);
        refresh.setActionCommand(TOOLNet.REFRESH_ACTION);
        start.addActionListener(net);
        stop.addActionListener(net);
        refresh.addActionListener(net);


        displayPanel = new JPanel();
        displayPanel.setLayout(new GridLayout(1, 2));


        JPanel metaPanel = new JPanel();
        metaPanel.setLayout(new GridLayout(2, 1));

        JPanel subPanel = new JPanel();
        subPanel.setLayout(new GridLayout(1, 2));
        subPanel.add(intPanel);
        subPanel.add(addrPanel);

        metaPanel.add(subPanel);
        metaPanel.add(robotPanel);

        displayPanel.add(metaPanel);

        JPanel sidePanel = new JPanel();
        sidePanel.setLayout(new BorderLayout());
        
        JPanel addPanel = new JPanel();
        addPanel.setLayout(new BoxLayout(addPanel, BoxLayout.LINE_AXIS));
        JButton addButton = new JButton("Add host");
        addButton.setActionCommand("add");
        addButton.addActionListener(this);
        addPanel.add(addButton);
        addPanel.add(Box.createRigidArea(new Dimension(5, 0)));
        hostEntry = new JTextField();
        hostEntry.addActionListener(this);
        addPanel.add(hostEntry);
        sidePanel.add(addPanel, BorderLayout.PAGE_START);

        optionPanel = new JPanel();
        optionPanel.setLayout(new GridLayout(1, 1));
        sidePanel.add(optionPanel, BorderLayout.CENTER);

        displayPanel.add(sidePanel);
    }

    private void initListPanels() {

        intPanel = new ListPanel<ListObject>(ListPanel.SINGLE);
        intPanel.setRowHeight(INTERFACE_ROW_HEIGHT);
        intPanel.addItemListener(this);

        addrPanel = new ListPanel<ListObject>(ListPanel.MULTIPLE);
        addrPanel.setRowHeight(ADDRESS_ROW_HEIGHT);
        addrPanel.addItemListener(this);

        robotPanel = new ListPanel<DataSetObject>(ListPanel.SINGLE);
        robotPanel.setRowHeight(ROBOT_ROW_HEIGHT);
        robotPanel.addItemListener(this);

        for (String intface : net.availableInterfaces()) {
            ListObject o = new ListObject(intface);
            intObjects.add(o);
            intPanel.add(o);
            o.getLayout().layoutContainer(o);
        }

        intPanel.updateInformation();
    }

    private void showRobotOptions(RemoteRobot r) {
        int i = robots.indexOf(r);
        if (i >= 0) {
            optionPanel.removeAll();
            optionPanel.add(robotPanels.get(i));
            optionPanel.validate();
        }
    }

    //
    // Abstract TOOLModule methods
    //

    public String getDisplayName() {
        return NET_MODULE_NAME;
    }

    public Component getDisplayComponent() {
        return displayPanel;
    }

    //
    // ActionListener contract
    //

    public void actionPerformed(ActionEvent ev) {
        try {
            if (hostEntry.getText().length() > 0)
                net.addCheckHost(hostEntry.getText());
        }catch (Exception e) {
            TOOL.CONSOLE.error(e, "Unable to resolve manually entered host " +
                "for TOOLNet broadcasts.");
        }
    }

    //
    // DataSource contract
    //
    
    public SourceHandler getHandler() {
        return this;
    }

    public int numDataSets() {
        return robots.size();
    }

    public String getPath() {
        return REMOTE_PATH + storagePath;
    }

    public String getType() {
        return ROBOT_SOURCE_TYPE;
    }

    public DataSet getDataSet(int i) {
        return robots.get(i);
    }

    /**
     * Retrive a list os all the DataSets in this source.
     *
     * @return a java.util.List of DataSet objects
     */
    public List<DataSet> getDataSets() {
        return new Vector<DataSet>(robots);
    }

    //
    // ItemListener contract
    //

    public void itemStateChanged(ItemEvent e) {
        if (e.getItem() == null)
            return;

        if (e.getSource() == intPanel) {

            if (e.getStateChange() == ItemEvent.SELECTED)
                fillAddrPanel(((ListObject)e.getItem()).getText());

        }else if (e.getSource() == addrPanel) {

            InetAddress addr = addrObjects.get((ListObject)e.getItem());

            if (e.getStateChange() == ItemEvent.SELECTED &&
                    !net.usingAddress(addr))
                net.useAddress(addr);

            else if (e.getStateChange() == ItemEvent.DESELECTED &&
                    net.usingAddress(addr))
                net.haltAddress(addr);

        }else if (e.getSource() == robotPanel) {

            showRobotOptions((RemoteRobot)
                ((DataSetObject)e.getItem()).getSet());

        }
    }

    public void fillAddrPanel(String intface) {
        addrObjects.clear();
        addrPanel.clear();

        for (InetAddress addr : net.availableAddresses(intface)) {
            ListObject o = new ListObject(addr.getHostAddress());
            addrObjects.put(o, addr);
            addrPanel.add(o);
            if (net.usingAddress(addr))
                addrPanel.select(addrPanel.length() - 1);
        }
    }

    public DataSet createNew(String url) {

        if (!url.startsWith(REMOTE_PATH)) {
            TOOL.CONSOLE.error("Invalid url given to network module.");
            return null;
        }
        
        // calculate name and address from the url
        int split = url.indexOf('@');
        String name, s_addr;
        if (split == -1)
            name = s_addr = url.substring(REMOTE_PATH.length());
        else {
            name = url.substring(REMOTE_PATH.length(), split);
            s_addr = url.substring(split + 1);
        }

        InetAddress addr;
        try {
            addr = InetAddress.getByName(s_addr);
        }catch (Exception e) {
            TOOL.CONSOLE.error(e, "Invalid url given to network module.");
            return null;
        }

        int i = robots.size();
        String path = new File(storagePath, name).getPath();
        RemoteRobot robot;
        try {
            robot = new RemoteRobot(this, robots.size(), path, name, addr);
        }catch (Exception e) {
            TOOL.CONSOLE.error(e, "Unable to create RemoteRobot DataSet for " +
                url);
            return null;
        }
        DataSetObject object = new DataSetObject(robot);

        // Add DataSet to the vectors and ListObject to the ListPanel
        robots.add(robot);
        robotObjects.add(object);
        robotPanel.add(object);
        robotPanels.add(new RobotOptionPanel(robot));

        return robot;
    }

    //
    // RobotListener contract
    //

    public void robotFound(String name, InetAddress addr) {
        System.out.println("Robot found!  Creating dataset for " + name);

        DataSet set = createNew(REMOTE_PATH + name + '@' 
                                + addr.getHostAddress());

        if (set != null && robots.size() == 1)
            robotPanel.select(0);
    }

    public void robotLost(String name) {
        int i;
        for (i = 0; i < robots.size(); i++)
            if (robots.get(i).name().equals(name))
                break;

        if (i < robots.size()) {
            robots.remove(i);
            robotPanel.remove(robotObjects.get(i));
            robotObjects.remove(i);
        }
    }

    //
    // SourceHandler contract
    //

    public boolean checkAvailable(String url) {
        return url.equals(REMOTE_PATH);
    }

    public DataSource getSource(String url) {
        if (checkAvailable(url))
            return this;
        return null;
    }


    //
    // Direct RemoteRobot access methods
    //

    public List<RemoteRobot> getRobots() {
        return new Vector<RemoteRobot>(robots);
    }

}
