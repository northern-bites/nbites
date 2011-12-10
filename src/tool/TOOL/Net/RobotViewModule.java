
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
import java.awt.event.ActionListener;

import java.util.HashMap;
import java.util.Map;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JMenu;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JTextField;
import javax.swing.JTextArea;
import javax.swing.JScrollPane;
import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;

import javax.swing.JComboBox;

import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;

import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import TOOL.TOOL;
import TOOL.Calibrate.VisionState;
import TOOL.Data.DataTypes;
import TOOL.Data.Frame;
import TOOL.Data.DataTypes.DataType;
import TOOL.Image.ImagePanel;
import TOOL.Image.ProcessedImage;
import TOOL.Image.TOOLImage;
import TOOL.Image.ThresholdedImageOverlay;
import TOOL.Net.DataRequest;

import TOOL.TOOLModule;
import TOOL.TOOLException;

public class RobotViewModule extends TOOLModule implements PopupMenuListener {

    private NetworkModule net;

    private JPanel displayPanel;
    private Map<JButton, DataType> typeMap;
    private Map<JMenuItem, RemoteRobot> robotMap;
    private JButton robotButton;
    private JPopupMenu robotMenu;
    private JTextArea textArea;
    private JTextField textField;

    private ImagePanel imagePanel;

    private RemoteRobot selectedRobot;

    private Thread streamingThread;
    private DataType streamType = DataTypes.DataType.THRESH;
    private boolean isStreaming = false;
    private boolean isSavingStream = false;
    private JButton startStopButton, streamButton;
    private JCheckBox saveStreamBox;

    private String saveFramePath = null;

    private TOOL tool;

    public static final long FRAME_LENGTH_MILLIS = 40;

    public RobotViewModule(TOOL t, NetworkModule net_mod) {
        super(t);

        tool = t;

        net = net_mod;
        displayPanel = new JPanel();
        typeMap = new HashMap<JButton, DataType>();
        robotMap = new HashMap<JMenuItem, RemoteRobot>();
        imagePanel = new ImagePanel();

        selectedRobot = null;

        createStreamingThread();
        streamingThread.start();

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

        createUpdateButtons(subPanel);
        createStreamingButtons(subPanel);
        displayPanel.add(subPanel);

        subPanel = new JPanel();
        subPanel.setLayout(new BoxLayout(subPanel, BoxLayout.PAGE_AXIS));
        subPanel.add(Box.createHorizontalGlue());
        subPanel.add(imagePanel);

        displayPanel.add(subPanel);
        setupCommandInput(subPanel);
    }

    private void setupCommandInput(JPanel subPanel){
        JPanel cmdPanel = new JPanel(new GridBagLayout());

        textField = new JTextField(20);
        textField.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e){
                    String cmd = textField.getText();
                    if (selectedRobot != null){
                        textArea.append(cmd + "\n");
                        selectedRobot.sendCmd(cmd);
                    }
                }
            });

        textArea = new JTextArea(10, 20);
        textArea.setEditable(false);
        JScrollPane scrollPane = new JScrollPane(textArea);

        GridBagConstraints c = new GridBagConstraints();
        c.gridwidth = GridBagConstraints.REMAINDER;

        c.fill = GridBagConstraints.HORIZONTAL;
        cmdPanel.add(textField, c);

        c.fill = GridBagConstraints.BOTH;
        c.weightx = 1.0;
        c.weighty = 1.0;
        cmdPanel.add(scrollPane, c);
        cmdPanel.setMaximumSize(new Dimension(500, 600));
        subPanel.add(cmdPanel, "SOUTH");
    }

    private void createUpdateButtons(JPanel subPanel){
        subPanel.add(new JLabel("Update:"));
        subPanel.add(Box.createRigidArea(new Dimension(30, 10)));

        for (DataType t : DataTypes.types()) {
            JButton b = new JButton(DataTypes.title(t));
            typeMap.put(b, t);
            b.addActionListener(this);
            subPanel.add(b);
        }
    }

    private void createStreamingButtons(JPanel panel){
        JPanel subPanel = new JPanel();
        subPanel.setLayout(new BoxLayout(subPanel, BoxLayout.PAGE_AXIS));
        panel.add(subPanel);

        subPanel.add(new JLabel("Streaming:"));


        String streamingTypes[] = {DataTypes.title(DataTypes.DataType.THRESH),
                                   DataTypes.title(DataTypes.DataType.IMAGE),
                                   DataTypes.title(DataTypes.DataType.OBJECTS)};
        JComboBox streamComboBox = new JComboBox(streamingTypes);

        Dimension maxDim = new Dimension(200,40);
        Dimension prefDim = new Dimension(200,40);
        streamComboBox.setAlignmentX(Component.LEFT_ALIGNMENT);
        streamComboBox.setMaximumSize(maxDim);
        streamComboBox.setPreferredSize(prefDim);

        streamComboBox.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e){
                    JComboBox box = (JComboBox)e.getSource();
                    if (box.getSelectedItem() ==
                        DataTypes.title(DataTypes.DataType.THRESH)){
                        streamType = DataTypes.DataType.THRESH;
                    }
                    else if (box.getSelectedItem() ==
                             DataTypes.title(DataTypes.DataType.IMAGE)){
                    streamType = DataTypes.DataType.IMAGE;
                    }
                    else if (box.getSelectedItem() ==
                            DataTypes.title(DataTypes.DataType.OBJECTS)){
                        streamType = DataTypes.DataType.OBJECTS;
                    }
                }
            });

        subPanel.add(streamComboBox);

        startStopButton = new JButton("Start");
        startStopButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e){
                    if (selectedRobot == null)
                        return;

                    // Assuming we are using proper directory structure,
                    // set the default save folder to
                    // $ROBOCUP/man/frames/stream.

                    if (saveFramePath == null) {
                        if (tool.CONSOLE.pathExists("../man/frames/stream")) {
                            saveFramePath = tool.CONSOLE.formatPath("../man/frames/stream");
                        } else {
                            saveFramePath = tool.CONSOLE.promptDirOpen("Save Destination",
                                                                       "../man/frames");
                        }
                    }

                    if (saveFramePath == null)
                        return;

                    isStreaming = !isStreaming;

                    if (isStreaming){
                        startStopButton.setText("Stop");
                    } else {
                        startStopButton.setText("Start");
                    }

                }
            });
        subPanel.add(startStopButton);

        JButton frameDestButton = new JButton("Set destination");
        frameDestButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e){
                    saveFramePath = tool.CONSOLE.promptDirOpen("Save Destination",
                                                               saveFramePath);
                }
            });
        subPanel.add(frameDestButton);

        saveStreamBox = new JCheckBox("Save stream");
        saveStreamBox.addItemListener( new ItemListener() {
                public void itemStateChanged(ItemEvent e) {

                    if (e.getStateChange() == ItemEvent.SELECTED){
                        isSavingStream = true;
                    } else if (e.getStateChange() == ItemEvent.DESELECTED){
                        isSavingStream = false;
                    }

                }
            });
        subPanel.add(saveStreamBox);
    }

    // Pretty tremendous hack for streaming images from Nao, probably could
    // and should be more elegant. Oh well.
    private void createStreamingThread() {

        streamingThread = new Thread(new Runnable() {
                public void run() {
                    int numFramesStreamed = 0;

                    long startTime = 0;
                    long timeSpent = 0;

                    VisionState visionState = null;

                    try {
                        while (true){
                            startTime = System.currentTimeMillis();
                            if (!isStreaming){
                                Thread.sleep(1500);
                                continue;
                            }

                            TOOLImage img = null;
                            ThresholdedImageOverlay threshOverlay = null;
                            Frame f = new Frame();

                            if (streamType == DataTypes.DataType.THRESH) {
                                img = selectedRobot.retrieveThresh();
                                imagePanel.setOverlayImage(null);
                            } else if (streamType == DataTypes.DataType.IMAGE) {
                                img = selectedRobot.retrieveImage();
                                imagePanel.setOverlayImage(null);
                            } else if (streamType == DataTypes.DataType.OBJECTS) {
                                selectedRobot.fillNewFrame(f);
                                if (f != null){
                                    if (visionState == null){
                                        visionState = new VisionState(f,
                                                tool.getColorTable());
                                        img = visionState.getThreshImage();
                                        threshOverlay =
                                            visionState.getThreshOverlay();
                                    }
                                    else {
                                        visionState.newFrame(f,
                                                             tool.getColorTable());
                                        //visionState.setColorTable(tool.getColorTable());
                                        visionState.update();
                                        img = visionState.getThreshImage();
                                        threshOverlay =
                                            visionState.getThreshOverlay();
                                    }
                                }
                                imagePanel.setOverlayImage(threshOverlay);
                            }
                            if (img != null) {
                                imagePanel.updateImage(img);
                                //    imagePanel.setOverlayImage(null);
                            }
                            //if (threshOverlay != null) {
                            //   imagePanel.setOverlayImage(threshOverlay);
                            //}

                            if (isSavingStream){
                                // Write image to a frame
                                Frame newFrame = selectedRobot.get(0);
                                selectedRobot.fillNewFrame(newFrame);
                                selectedRobot.load(0);
                                selectedRobot.store(numFramesStreamed,
                                                    selectedRobot.get(0),
                                                    0,
                                                    saveFramePath);
                                numFramesStreamed++;
                            }
                            timeSpent = System.currentTimeMillis() - startTime;
                            if (timeSpent < FRAME_LENGTH_MILLIS){
                                Thread.sleep(FRAME_LENGTH_MILLIS - timeSpent);
                            }
                        }
                    } catch (InterruptedException e){
                    } catch (TOOLException e) {}

                }
            });
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
            }
            else {
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

	public RemoteRobot getSelectedRobot() {
		return selectedRobot;
	}
}
