
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

package TOOL;

import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.*;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;
import javax.swing.*;

import TOOL.Data.DataManager;
import TOOL.Data.SourceManager;
import TOOL.Data.DataModule;
import TOOL.Calibrate.Calibrate;
import TOOL.Calibrate.CalibrateModule;
import TOOL.Calibrate.CameraCalibratePanel;
import TOOL.ColorEdit.ColorEditModule;
import TOOL.ColorEdit.ColorEdit;
import TOOL.Classifier.ClassifierModule;
import TOOL.Learning.LearningModule;
import TOOL.Console.Console;
import TOOL.GUI.MultiTabbedPane;
import TOOL.Image.*;
import TOOL.Net.NetworkModule;
import TOOL.Net.RobotViewModule;
import TOOL.PlayBookEditor.PlayBookEditorModule;
import TOOL.WorldController.WorldControllerModule;

import java.util.prefs.*;
import java.awt.event.WindowListener;
import java.awt.event.WindowEvent;
import java.awt.Window;


//import TOOL.ZSpace.ZModule;


/**
 * This class ties all the sub modules of the TOOL together.
 * Ultimately, it really shouldn't be doing much, but until we get a frame handler
 * class going, we will use TOOL to handle loading frames and color tables
 * @author Jeremy Fishman, Joho Strom
 * @see Calibrate, Image
 */
public class TOOL implements ActionListener, PropertyChangeListener{

    //temporary defaults, eventually wont want to auto load content
    public static final boolean LOAD_DEFAULTS = true;
    public static final String DEFAULT_IMAGE_DIRECTORY = "./";
    public static final String DEFAULT_TABLE_PATH = "./";


    public static final int DATA_MANAGER_HEIGHT  = 24;
    public static final int DEFAULT_PANES = 1;


    public static TOOL instance = null;
    public static Console CONSOLE = null;

    public static Preferences prefs;

    private JFrame mainWindow;
    private JSplitPane split_pane;
    private JTabbedPane tabs;
    private boolean split_changing;
    public String wcLastDirectory;
    //menu components
    JMenuItem quit, about, controls;
    //menus
    JMenu fileMenu, help;
    JMenuBar menuBar;

    //temp menus to allow color table stuff
    JMenu actions;
    JMenuItem newColorTable, saveColorTable, saveColorTableAs,loadColorTable,
        toggleAutoSave, toggleSoftColors, intersectColorTable, cameraCalibrate;
    JMenuItem addPane, removePane;
    //modules
    private Calibrate calibrate;
    private ColorEdit colorEdit;
    private DataManager dataManager;
    private SourceManager sourceManager;
    private ColorTable colorTable;

    private List<TOOLModule> modules;
    private HashMap<String, Component> moduleMap;
    private MultiTabbedPane multiPane;


    private static final int DEFAULT_WIDTH = 900;
    private static final int DEFAULT_HEIGHT = 800;

    private static final String DEFAULT_WIDTH_STRING = "default_width";
    private static final String DEFAULT_HEIGHT_STRING = "default_height";

    private static final int DEFAULT_X = 0;
    private static final int DEFAULT_Y = 0;

    private static final String DEFAULT_X_STRING = "default_x";
    private static final String DEFAULT_Y_STRING = "default_y";

    private static final String DEFAULT_1_TAB_STRING = "default_1_tab";
    private static final String DEFAULT_1_TAB = "Data";

    private static final String DEFAULT_2_TAB_STRING = "default_2_tab";
    private static final String DEFAULT_2_TAB = "Calibrate";

    private static final String DEFAULT_COLOR_TABLE_STRING =
        "default_color_table";

    private static final String DEFAULT_WC_DIRECTORY_STRING =
        "default_wc_directory_string";
    //starts an instance of a tool, which ties together all the sub modules
    public TOOL(){

        instance = this;

        // Initialize the preferences for the TOOL
        prefs = Preferences.userNodeForPackage(this.getClass());

        CONSOLE = new Console(this);

        // Try to load the color table we used last
        // If the key does not exist, the method will return the second
        // parameter passed to .get().
        String fileName = prefs.get(DEFAULT_COLOR_TABLE_STRING,
                                    null);

        // Initialize the back-end managers and module list
        sourceManager = new SourceManager();
        loadStartColorTable(fileName);
        dataManager = new DataManager();
        sourceManager.addSourceListener(dataManager);

        modules = new Vector<TOOLModule>();
        moduleMap = new HashMap<String, Component>();

        // Setup the GUI
        initMainWindow();

        // Initialize and add each of the modules
        //
        // DataManager takes care of all image loading etc.
        addModule(new DataModule(this));
        // We need to keep a reference to our calibrate module in order to
        // clear the histories when creating a new table
        CalibrateModule calibrator = new CalibrateModule(this);
        calibrate = calibrator.getCalibrate();
        addModule(calibrator);
        // classifier
        //addModule(new ClassifierModule(this));
		// learning

		addModule(new LearningModule(this));
        // color edit
        ColorEditModule colorEditModule = new ColorEditModule(this);
        colorEdit = (ColorEdit) colorEditModule.getDisplayComponent();
        addModule(colorEditModule);
        //        addModule(new DatabaseModule(this));
        // network - discover and connect to robots
        NetworkModule net_mod = new NetworkModule(this);
        addModule(net_mod);
        // playbook - edit plays on a field model
        //addModule(new PlayBookEditorModule(this));
        // robotview - view up-to-date information live from the robot
        //    depends on NetworkModule
		RobotViewModule robot_mod = new RobotViewModule(this, net_mod);
		addModule(robot_mod);

        // wordcontroller - view and control robot udp broadcasts in realtime

        // Try to get the last directory we used for the world controller
        wcLastDirectory = prefs.get(DEFAULT_WC_DIRECTORY_STRING,
                                    System.getProperty("user.dir"));
        addModule(new WorldControllerModule(this, robot_mod));

        // Tell the modules which colortable they should use.
        updateColorTableReferences();
        // Add color table listeners to the two modules that must be notified
        // whenever the color table changes
        dataManager.addColorTableListener(calibrate);
        dataManager.addColorTableListener(colorEdit);

        setupPane();

        // Listen to window closings so as to save preferences
        mainWindow.addWindowListener(new WindowPreferenceListener(this));

        // Determine where window should launch
        int startX = prefs.getInt(DEFAULT_X_STRING, DEFAULT_X);
        int startY = prefs.getInt(DEFAULT_Y_STRING, DEFAULT_Y);

        // Determine size window should start at
        int startWidth = prefs.getInt(DEFAULT_WIDTH_STRING, DEFAULT_WIDTH);
        int startHeight = prefs.getInt(DEFAULT_HEIGHT_STRING, DEFAULT_HEIGHT);

        mainWindow.setLocation(startX, startY);
        mainWindow.setSize(startWidth, startHeight);

        // Finally, make it visible.
        mainWindow.setVisible(true);
        // do this again, in case it didn't stick before window ws visible
        // (happens on linux)
        //mainWindow.setExtendedState(JFrame.MAXIMIZED_BOTH);

        final String fileSeparator = System.getProperty("file.separator");
        sourceManager.addSource(".." + fileSeparator + ".." + fileSeparator + "data" + fileSeparator
                                +"frames" + fileSeparator);
        // add the local robots folder to the source manager
        sourceManager.addSource("robots");

    }

    public void addModule(TOOLModule m) {
        modules.add(m);
        moduleMap.put(m.getDisplayName(), m.getDisplayComponent());
    }

    public void setupPane() {
        // remove old pane
        mainWindow.getContentPane().removeAll();
        // create new pane
        multiPane = new MultiTabbedPane(this, DEFAULT_PANES, moduleMap);
        // add new pane
        mainWindow.getContentPane().add(multiPane);

        String defaultModule1 = prefs.get(DEFAULT_1_TAB_STRING,
                                          DEFAULT_1_TAB);

        String defaultModule2 = prefs.get(DEFAULT_2_TAB_STRING,
                                          DEFAULT_2_TAB);


        // select default initial panel
        multiPane.performTabSelection(0, defaultModule1);
        multiPane.performTabSelection(1, defaultModule2);
        // will silently fail if there isn't more than one pane
    }

    private void initMainWindow(){

        //start the main window
        mainWindow = new JFrame("TOOL - Main Window of Awesome");
        mainWindow.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		// Try to set the look and feel to the appropriate OS setting
		try {
			UIManager.setLookAndFeel(UIManager.
									 getSystemLookAndFeelClassName());
		} catch (Exception e) {}


        //init menu needs to happen here, but before everything else, or the
        //the menu doesnt display right
        initMenu();

        //and finally,  set size of the window
        mainWindow.setSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
        //mainWindow.setExtendedState(JFrame.MAXIMIZED_VERT);

    }

    private void initMenu(){
        //make a global menu bar
        menuBar = new JMenuBar();

        //make the file menu
        fileMenu = new JMenu("File");

        quit = new JMenuItem("Quit");
        quit.setActionCommand("quit");
        quit.addActionListener(this);
        quit.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q,
                                                   InputEvent.CTRL_MASK ));
        fileMenu.add(quit);

        menuBar.add(fileMenu);


        //Temporary menu so we can load and save color tables
        actions = new JMenu("Actions");
        newColorTable = new JMenuItem("New Color Table");
        loadColorTable = new JMenuItem("Load Color Table ");
        saveColorTable = new JMenuItem("Save Color Table");
        saveColorTableAs = new JMenuItem("Save Color Table As");
		intersectColorTable = new JMenuItem("Intersect Color Table");
        toggleAutoSave = new JCheckBoxMenuItem("Autosave enabled");
        toggleSoftColors = new JCheckBoxMenuItem("Softcolors enabled");
        cameraCalibrate = new JMenuItem("Calibrate Camera");
        toggleSoftColors.setSelected(true);

        addPane = new JMenuItem("Add Pane");
        removePane = new JMenuItem("Remove Pane");

        newColorTable.addActionListener(this);
        loadColorTable.addActionListener(this);
		intersectColorTable.addActionListener(this);
        saveColorTable.addActionListener(this);
        saveColorTableAs.addActionListener(this);
        toggleAutoSave.addActionListener(this);
        toggleSoftColors.addActionListener(this);
        cameraCalibrate.addActionListener(this);

        addPane.addActionListener(this);
        removePane.addActionListener(this);

        actions.add(newColorTable);
        actions.add(loadColorTable);
		actions.add(intersectColorTable);
        actions.add(saveColorTable);
        actions.add(saveColorTableAs);

        actions.addSeparator();
        actions.add(toggleAutoSave);
        actions.add(toggleSoftColors);

        actions.addSeparator();
        actions.add(addPane);
        actions.add(removePane);
        
        actions.addSeparator();
        actions.add(cameraCalibrate);

        menuBar.add(actions);

        help = new JMenu("Help");
        about = new JMenuItem("About");
        controls = new JMenuItem("Controls");
        about.addActionListener(this);
        controls.addActionListener(this);

        help.add(about);
        help.add(controls);

        menuBar.add(help);

        //finish up
        mainWindow.setJMenuBar(menuBar);

    }

    //
    // Misc access/set methods
    //

    public void setCursor(Cursor cursor){
        mainWindow.setCursor(cursor);
    }

    public Point getLocation() {
        return mainWindow.getLocation();
    }

    public ColorTable getColorTable() {
        return colorTable;
    }

    public void setColorTable(ColorTable t) {
        colorTable = t;
    }


    //
    // Public access to managers and modules
    //

    public DataManager getDataManager() {
        return dataManager;
    }

    public SourceManager getSourceManager() {
        return sourceManager;
    }

    public List<TOOLModule> getModules() {
        return modules;
    }

    public TOOLModule getModule(String s) {
        for (TOOLModule d : modules){
            if (d.getDisplayName().equals(s)) {
                return d;
            }
        }
        return null;
    }

    public JFrame getFrame() {
        return mainWindow;
    }



    //handle menu items if necessary
    public void actionPerformed(ActionEvent e){
        String cmd = e.getActionCommand();

        if(cmd.equals("quit")) {
            savePreferences();
            System.exit(0);
        }

        else if (e.getSource() == controls) {
            displayControls();
        }
        else if (e.getSource() == about) {

        }
        //added temporary way to load color tables
        else if(e.getSource() == loadColorTable){
            loadColorTable();
        }
        else if(e.getSource() == saveColorTable){
            saveColorTable();
        }
        else if(e.getSource() == saveColorTableAs){
            saveColorTableAs();
        }
        else if(e.getSource() == newColorTable){
            newColorTable();
        }
		else if(e.getSource() == intersectColorTable) {
			intersectColorTable();
		}
        else if (e.getSource() == addPane) {
            multiPane.addPane();
            if (multiPane.numPanes() > 1)
                removePane.setEnabled(true);
            mainWindow.validate();
        }
        else if (e.getSource() == removePane) {
            multiPane.removePane();
            if (multiPane.numPanes() == 1)
                removePane.setEnabled(false);
            mainWindow.validate();
        }
        else if (e.getSource() == toggleAutoSave) {
            colorTable.setAutoSave(toggleAutoSave.isSelected());
            CONSOLE.println("Auto save enabled: " +
                            toggleAutoSave.isSelected());
        }
        else if (e.getSource() == toggleSoftColors) {
            colorTable.setSoftColors(toggleSoftColors.isSelected());
            CONSOLE.println("Soft colors enabled: " +
                            toggleSoftColors.isSelected());
        }
        else if (e.getSource() == cameraCalibrate) {
            CameraCalibratePanel cameraCalibrate = new CameraCalibratePanel(calibrate);
        }

    }

    // PropertyChangeListener method
    public void propertyChange(PropertyChangeEvent e) {
        if (e.getPropertyName().equals(JSplitPane.DIVIDER_LOCATION_PROPERTY)
            && !split_changing) {
            split_changing = true;
            split_pane.setDividerLocation(DATA_MANAGER_HEIGHT);
            split_changing = false;
        }
    }





    private void displayControls() {
        String controls = "Scroll mouse wheel to select brush size.\n" +
            "Hold down shift and scroll mouse wheel to change colors.\n" +
            "Hold down control (Windows) or meta (Mac) and scroll to move" +
            " through pictures.\n" +
            "Press the 1 through 10 keys to select the corresponding color " +
            "at the bottom of the screen.  If you need any of the subsequent " +
            "colors, click on the color instead.\n" +
            "Press the 'u' key to toggle undefine mode.  In undefine mode, " +
            "the currently selected color will be undefined, wherever you " +
            "click in the image.";
        System.out.println(controls);

    }


    //temp color table thing
    public void newColorTable(){
        try{
            // Determine the size of the color table based on the way the user
            // chose.
            ColorTable.Dimension d = ColorTable.getSize();
            // They cancelled the process...
            if (d == null) {
                return;
            }
            // Create a color table of the correct size
            else {
                colorTable = new ColorTable(ColorTable.EMPTY, d);
            }

            colorTable.setSoftColors(toggleSoftColors.isSelected());
            // If they had been editing a table earlier, clear out their undos
            calibrate.clearHistory();
        }
        catch(IOException  e){
            return;
        }
        dataManager.notifyDependants();
        colorEdit.setTable(colorTable);

    }
    public void saveColorTable(){
        colorTable.saveColorTable();
    }
    public void saveColorTableAs(){
        colorTable.saveColorTableAs();
    }

    /**
     * This method uses the abstract way of prompting for a file. More often
     * than not, this will be a GUI dialog allowing you to navigate to a
     * file in some path.
     **/
    public void loadColorTable() {
        String path = CONSOLE.promptFileOpen(
                                             "Existing Color Table Location and Name",
                                             ColorTable.LOAD_TABLE_PATH);

        if (path != null) {
            loadColorTable(path);
        }

    }

    /**
     * This method uses the abstract way of prompting for a file. More often
     * than not, this will be a GUI dialog allowing you to navigate to a
     * file in some path.
     **/
    public void intersectColorTable() {
        String path = CONSOLE.promptFileOpen(
                                             "Existing Color Table Location and Name",
                                             ColorTable.LOAD_TABLE_PATH);

        if (path != null) {
			colorTable.intersect(path);
            // If they had been editing a table earlier, clear out their
            // undos
            calibrate.clearHistory();
            dataManager.notifyDependants();
        }

    }

    /**
     * This method takes in a file name and tries to load a color table from it at startup
     **/
    public void loadStartColorTable(String fileName) {

        colorTable = new ColorTable(fileName);
    }

    public void updateColorTableReferences() {
        colorEdit.setTable(colorTable);
        colorTable.setSoftColors(toggleSoftColors.isSelected());
        // If they had been editing a table earlier, clear out their
        // undos
        calibrate.clearHistory();
        dataManager.notifyDependants();
    }
    /**
     * This method takes in a file name and tries to load a color table from it
     **/
    public void loadColorTable(String fileName) {
        colorTable = new ColorTable(fileName);
        colorEdit.setTable(colorTable);

        colorTable.setSoftColors(toggleSoftColors.isSelected());
        // If they had been editing a table earlier, clear out their
        // undos
        calibrate.clearHistory();
        dataManager.notifyDependants();

    }

    public void savePreferences() {
        saveWindowPrefs(prefs, mainWindow);
        saveTabPrefs(prefs, multiPane);
        saveColorTablePrefs(prefs);
        saveDirectoryPrefs(prefs);
    }

    public void saveWindowPrefs(Preferences prefs, Window w) {
        prefs.putInt(DEFAULT_WIDTH_STRING, w.getWidth());
        prefs.putInt(DEFAULT_HEIGHT_STRING, w.getHeight());
        prefs.putInt(DEFAULT_X_STRING, w.getX());
        prefs.putInt(DEFAULT_Y_STRING, w.getY());
    }

    public void saveTabPrefs(Preferences prefs, MultiTabbedPane tabs) {
        prefs.put(DEFAULT_1_TAB_STRING, tabs.getSelectedTabName(0));
        if (tabs.getNumTabbedPanes() > 1) {
            prefs.put(DEFAULT_2_TAB_STRING, tabs.getSelectedTabName(1));
        }
    }

    public void saveColorTablePrefs(Preferences prefs) {
        if (colorTable != null) {
            String fileName = colorTable.getFileName();
            if (fileName != null)
                prefs.put(DEFAULT_COLOR_TABLE_STRING,
                          fileName);
        }
    }

    public void saveDirectoryPrefs(Preferences prefs) {
        prefs.put(DEFAULT_WC_DIRECTORY_STRING, wcLastDirectory);
    }

    public static void main(String[] args) {
        TOOL t = new TOOL();
    }

    /**
     * @author Nicholas Dunn
     * On window close, alerts TOOL that it should save its preferences
     */
    public class WindowPreferenceListener implements WindowListener {

        private TOOL tool;

        public WindowPreferenceListener(TOOL t) {
            tool = t;
        }

        public void windowActivated(WindowEvent e) {}
        public void windowClosed(WindowEvent e) {
            tool.savePreferences();
        }
        public void windowClosing(WindowEvent e) {
            tool.savePreferences();
        }
        public void windowDeactivated(WindowEvent e) {}
        public void windowDeiconified(WindowEvent e) {}
        public void windowIconified(WindowEvent e) {}
        public void windowOpened(WindowEvent e) {}
    }
}
