package TOOL.Classifier;

import TOOL.TOOL;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.Arrays;

 
/** 
 *  @author: Nick Dunn
 *  @date:   5/24/2007
 *
 * plugin to AiboConnect.java.  Displays a variety of buttons in a new JFrame
 * so that user can log what on field objects are visible.  These user
 * inputted counts can then be checked against what the robot vision sees
 * in order to determine whether or not vision changes were helpful or not.
 *
 *
 * left click on a button increments that count.  right click decrements.
 *
 * Saves the user's input to "log.txt" file within the directory where
 * user was logging .FRM images.
 *
 */


public class ClassifierFrame extends JFrame implements ActionListener, 
						       MouseListener,
						       KeyListener {

    private TOOL tool;
    private int pixelSize;

  // Strings for key generating menu
    
    public final static String WINDOW_TITLE = "Picture Classifier Palette";   
    public final static String KEY_GEN_STRING = "Key Generation";
    public final static String KEY_GEN_ACTION = "generate key";
  

    
    // Strings for buttons for key generating
    public final static String BALL_STRING = "Ball: ";
    public final static String BLUE_YELLOW_STRING = "Blue Yellow Post: ";
    public final static String YELLOW_BLUE_STRING = "Yellow Blue Post: ";
    public final static String BLUE_GOAL_LEFT_STRING = "Blue Goal Left Post: ";
    public final static String BLUE_GOAL_RIGHT_STRING = 
	"Blue Goal Right Post: ";
    public final static String BLUE_BACKSTOP_STRING =
	"Blue Goal Backstop: ";
    public final static String YELLOW_BACKSTOP_STRING = 
	"Yellow Goal Backstop: ";
    public final static String YELLOW_GOAL_LEFT_STRING = 
	"Yellow Goal Left Post: ";
    public final static String YELLOW_GOAL_RIGHT_STRING = 
	"Yellow Goal Right Post: ";

   


    // Buttons with action listeners only
    public final static String LAST_FRAME_STRING = "Previous Frame";
    public final static String LAST_FRAME_ACTION = "go back";

    public final static String NEXT_FRAME_STRING = "Next Frame";
    public final static String NEXT_FRAME_ACTION = "advance";

    public final static String RESET_STRING = "Reset Current Frame Count";
    public final static String RESET_ACTION = "reset";

    public final static String SAVE_STRING = "Save to Log File";
    public final static String SAVE_ACTION = "save log";

    public final static String SAVE_QUIT_STRING = "Save and Quit";
    public final static String SAVE_QUIT_ACTION = "save quit";

    public final static String QUIT_WO_SAVE_STRING = "Quit Mode Without Saving";
    public final static String QUIT_WO_SAVE_ACTION = "quit wo save";

    
    public final static String QUIT_QUESTION = "Do you want to save "+
	"before closing window?";
    
    public final static int LEFT_CLICK_INCREMENT = 1;
    public final static int RIGHT_CLICK_DECREMENT = -1;
	


    // Two dimensional array for describing what objects are in what frame
    private int[][] seen;
    private int totalOfflineImages;

    private JPanel keyGenPanel;
    private JButton yellowGoalLeftButton, yellowGoalRightButton, 
	blueGoalLeftButton, blueGoalRightButton, blueYellowPostButton, 
	yellowBluePostButton, blueBackstopButton, yellowBackstopButton, 
	ballButton, reset, lastFrame, nextFrame, save, saveAndQuit, quit;

    
    public final static char NEXT_KEY = 'n';
    public final static char PREVIOUS_KEY = 'g';


    
    private File directory;
    // the log where we write our frame information (number of posts in frame etc.)
    private File log;

    // Constants for deciding which place in the array is updated upon clicking
    // a button
    private static final int NUM_OBJECTS = 37;    
    // Misc objects
    private static final int BALL_INDEX = 0;
    private static final int BLUE_YELLOW_POST_INDEX = 1;
    private static final int YELLOW_BLUE_POST_INDEX = 2;
    // Blue goal
    private static final int BLUE_GOAL_LEFT_INDEX = 3;
    private static final int BLUE_GOAL_RIGHT_INDEX = 4;
    private static final int BLUE_BACKSTOP_INDEX = 5;
    // Yellow goal
    private static final int YELLOW_GOAL_LEFT_INDEX = 6;
    private static final int YELLOW_GOAL_RIGHT_INDEX = 7;
    private static final int YELLOW_BACKSTOP_INDEX = 8;

    // ABSTRACT CORNER IDS FIRST
    
    

    private static final int L_INNER_CORNER = 9;
    private static final int L_OUTER_CORNER = 10;
    private static final int T_CORNER = 11;

    private static final int CENTER_CIRCLE = 12;

    // FUZZY/CLEAR CORNER IDS
    private static final int BLUE_GOAL_T = 13;
    private static final int YELLOW_GOAL_T = 14;
    private static final int BLUE_GOAL_RIGHT_L_OR_YELLOW_GOAL_LEFT_L = 15;
    private static final int BLUE_GOAL_LEFT_L_OR_YELLOW_GOAL_RIGHT_L = 16;
    private static final int CORNER_INNER_L = 17;
    private static final int GOAL_BOX_INNER_L = 18;
    private static final int CORNER_OR_GOAL_INNER_L = 19;
    private static final int BLUE_GOAL_OUTER_L = 20;
    private static final int YELLOW_GOAL_OUTER_L = 21;
    private static final int CENTER_T = 22;

    // SPECIFIC CORNER IDS
    private static final int BLUE_CORNER_LEFT_L = 23;
    private static final int BLUE_CORNER_RIGHT_L = 24;
    private static final int BLUE_GOAL_LEFT_T = 25;
    private static final int BLUE_GOAL_RIGHT_T = 26;
    private static final int BLUE_GOAL_LEFT_L = 27;
    private static final int BLUE_GOAL_RIGHT_L = 28;
    private static final int CENTER_BY_T = 29;
    private static final int CENTER_YB_T = 30;
    private static final int YELLOW_CORNER_LEFT_L = 31;
    private static final int YELLOW_CORNER_RIGHT_L = 32;
    private static final int YELLOW_GOAL_LEFT_T = 33;
    private static final int YELLOW_GOAL_RIGHT_T = 34;
    private static final int YELLOW_GOAL_LEFT_L = 35;
    private static final int YELLOW_GOAL_RIGHT_L = 36;
    

    private static final String[] cornerTitles = {
        "L inner corner",
        "L outer corner",
        "T corner",
        "Center circle",
        "Blue goal T",
        "Yellow goal T",
        "Blue goal right L or Yellow goal left L",
        "Blue goal left L or Yellow goal right L",
        "Corner inner L",
        "Goal box innter L",
        "Corner or goal inner L",
        "Blue goal outer L",
        "Yellow goal outer L",
        "Center T",
        "Blue corner left L",
        "Blue corner right L",
        "Blue goal left T",
        "Blue goal right T",
        "Blue goal left L",
        "Blue goal right L",
        "Center BY T",
        "Center YB T",
        "Yellow corner left L",
        "Yellow corner right L",
        "Yellow goal left T",
        "Yellow goal right T",
        "Yellow goal left L",
        "Yellow goal right L"
    };

    private static final String[] LCornerStrings = {
        cornerTitles[L_INNER_CORNER],
        cornerTitles[L_OUTER_CORNER],
        cornerTitles[BLUE_GOAL_RIGHT_L_OR_YELLOW_GOAL_LEFT_L],
        cornerTitles[BLUE_GOAL_LEFT_L_OR_YELLOW_GOAL_RIGHT_L],
        cornerTitles[CORNER_INNER_L],
        cornerTitles[GOAL_BOX_INNER_L],
        cornerTitles[CORNER_OR_GOAL_INNER_L],
        cornerTitles[BLUE_GOAL_OUTER_L],
        cornerTitles[YELLOW_GOAL_OUTER_L],
        cornerTitles[BLUE_CORNER_LEFT_L],
        cornerTitles[BLUE_CORNER_RIGHT_L],
        cornerTitles[BLUE_GOAL_LEFT_L],
        cornerTitles[BLUE_GOAL_RIGHT_L],
        cornerTitles[YELLOW_CORNER_LEFT_L],
        cornerTitles[YELLOW_CORNER_RIGHT_L],
        cornerTitles[YELLOW_GOAL_LEFT_L],
        cornerTitles[YELLOW_GOAL_RIGHT_L]
    };

    private static final String[] TCornerStrings = {
        cornerTitles[T_CORNER],
        cornerTitles[BLUE_GOAL_T],
        cornerTitles[YELLOW_GOAL_T],
        cornerTitles[CENTER_T],
        cornerTitles[BLUE_GOAL_LEFT_T],
        cornerTitles[BLUE_GOAL_RIGHT_T],
        cornerTitles[CENTER_BY_T],
        cornerTitles[CENTER_YB_T],
        cornerTitles[YELLOW_GOAL_LEFT_T],
        cornerTitles[YELLOW_GOAL_RIGHT_T],
    };



    

    
    public ClassifierFrame(TOOL t, int x, int y) {
	
	super(WINDOW_TITLE);
	setDefaultLookAndFeelDecorated(true);

	addKeyListener(this);
	
        tool = t;

	setFocusable(true);
	setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
	setLocation(x, y);
	initKeyPanel();

	getContentPane().add(keyGenPanel);
	pack();
	setVisible(false);
	//addWindowListener(this);
	

	// Warning error adapted from 
	// http://forum.java.sun.com/thread.jspa?threadID=
	// 599296&messageID=3683248

	addWindowListener( new WindowAdapter() {
		public void windowClosing(WindowEvent e) {
		    int selection = JOptionPane.showConfirmDialog(null,
								  QUIT_QUESTION,								  "Warning",
	                        JOptionPane.YES_NO_CANCEL_OPTION,
	                        JOptionPane.WARNING_MESSAGE);
	                if( selection == JOptionPane.NO_OPTION ) {
	                    // Close frame
                            //			    ai.stopKeyGenerating();
	                    
	                }
			// Save and close
			else if (selection == JOptionPane.YES_OPTION ) {
			    //writeLogFile();
			    //ai.stopKeyGenerating();
			}
			// if he hit cancel, no need to do anything
	                
	            }            
	        });

    }


    /** Method attempts to read a log file from the directory user loaded.
     *  If the log file exists, it is parsed via the parseLogFile() method.
     *  Otherwise, we make a new log file and will write to it upon a save
     * call.
     */
    public void readOrMakeLog(File directory, int totalOfflineImages) {
	
	this.directory = directory;
	this.totalOfflineImages = totalOfflineImages;


	log = new File(directory, "log.txt");
	try {
	    BufferedReader logRead =
		new BufferedReader(new FileReader(log));
	    System.out.println("Loaded 'log.txt' successfully");

	    parseLogFile(logRead, totalOfflineImages);
	}
	catch (IOException E) {
	    System.out.println("No log file found, creating 'log.txt'");
	    
	    // Initialize the array for quantifying what is in each frame
	    // (i.e. there was no log previously made, so make a fresh
	    // array)
	    seen = new int[totalOfflineImages][NUM_OBJECTS];

	}
	updateCounts();
    }
   

    

    // Makes all the buttons for the key gen phase
    public void initKeyPanel() {

	JPanel buttons = new JPanel();
	
	keyGenPanel = new JPanel();
	keyGenPanel.setLayout(new GridLayout(0, 1));
	
	buttons.setLayout(new GridLayout(0, 3));
	
	
	//Set up labels for the key generating phase
	
	// Post buttons
	blueYellowPostButton = new JButton(BLUE_YELLOW_STRING);
	blueYellowPostButton.addMouseListener(this);
	blueYellowPostButton.setVisible(true);

	yellowBluePostButton = new JButton(YELLOW_BLUE_STRING);
	yellowBluePostButton.addMouseListener(this);
	yellowBluePostButton.setVisible(true);
	
	// Blue goal buttons
	blueGoalLeftButton = new JButton(BLUE_GOAL_LEFT_STRING);
	blueGoalLeftButton.addMouseListener(this);
	blueGoalLeftButton.setVisible(true);

	blueGoalRightButton = new JButton(BLUE_GOAL_RIGHT_STRING);
	blueGoalRightButton.addMouseListener(this);
       	blueGoalRightButton.setVisible(true);

	blueBackstopButton = new JButton(BLUE_BACKSTOP_STRING);
	blueBackstopButton.addMouseListener(this);
	blueBackstopButton.setVisible(true);


	// Yellow goal buttons
	yellowGoalLeftButton = new JButton(YELLOW_GOAL_LEFT_STRING);
	yellowGoalLeftButton.addMouseListener(this);
	yellowGoalLeftButton.setVisible(true);

	yellowGoalRightButton = new JButton(YELLOW_GOAL_RIGHT_STRING);
	yellowGoalRightButton.addMouseListener(this);
	yellowGoalRightButton.setVisible(true);
	
	yellowBackstopButton = new JButton(YELLOW_BACKSTOP_STRING);
	yellowBackstopButton.addMouseListener(this);
	yellowBackstopButton.setVisible(true);

	// Button for ball
	ballButton = new JButton(BALL_STRING);
	ballButton.addMouseListener(this);
	ballButton.setVisible(true);


	// Make panel reset button
	reset = new JButton(RESET_STRING);
	reset.setActionCommand(RESET_ACTION);
	reset.addActionListener(this);
	reset.setVisible(true);


	lastFrame = new JButton(LAST_FRAME_STRING);
	lastFrame.setActionCommand(LAST_FRAME_ACTION);
	lastFrame.addActionListener(this);
	lastFrame.setVisible(true);

	nextFrame = new JButton(NEXT_FRAME_STRING);
	nextFrame.setActionCommand(NEXT_FRAME_ACTION);
	nextFrame.addActionListener(this);
	nextFrame.setVisible(true);


	// Make panel for the saving options
	save = new JButton(SAVE_STRING);
	save.setActionCommand(SAVE_ACTION);
	save.addActionListener(this);
	save.setVisible(true);

	saveAndQuit = new JButton(SAVE_QUIT_STRING);
	saveAndQuit.setActionCommand(SAVE_QUIT_ACTION);
	saveAndQuit.addActionListener(this);
	saveAndQuit.setVisible(true);

	quit = new JButton(QUIT_WO_SAVE_STRING);
	quit.setActionCommand(QUIT_WO_SAVE_ACTION);
	quit.addActionListener(this);
	quit.setVisible(true);

	JPanel fileWrite = new JPanel();
	fileWrite.add(save);
	fileWrite.add(saveAndQuit);
	fileWrite.add(quit);


	// Add the buttons to the panel

	// Misc row
	buttons.add(ballButton);
	buttons.add(blueYellowPostButton);
	buttons.add(yellowBluePostButton);
	
	// Blue row
	buttons.add(blueGoalLeftButton);
	buttons.add(blueGoalRightButton);
	buttons.add(blueBackstopButton);
	
	// Yellow row
	buttons.add(yellowGoalLeftButton);
	buttons.add(yellowGoalRightButton);
	buttons.add(yellowBackstopButton);


	buttons.add(reset);
	buttons.add(lastFrame);
	buttons.add(nextFrame);
	
	keyGenPanel.add(buttons);
	keyGenPanel.add(fileWrite);
	

	keyGenPanel.addKeyListener(this);

	keyGenPanel.setVisible(true);


    }



    public void keyPressed(KeyEvent e) {
	
	
	if (e.getKeyChar() == NEXT_KEY) {
            //ai.advanceFrame();
	}
	else if (e.getKeyChar() == PREVIOUS_KEY) {
	    //ai.regressFrame();
	}
	   

    }
    public void keyReleased(KeyEvent e) {}
    public void keyTyped(KeyEvent e) {}




    /* Deal with the buttons that have two way functionality (i.e. increment
       and decrement counts).  A left click increments the count, a right
       click decrements the count (but not to negative numbers, as that is
       nonsensical in this context.
     */

    public void mouseClicked(MouseEvent e) {

	JButton clicked = (JButton) e.getSource();

	boolean leftClick = e.getButton() == e.BUTTON1;
	
	int change;
	// make it so a left click increments the total while a right click
	// decrements
	if (leftClick) {
	    change = LEFT_CLICK_INCREMENT;
	}
	else {
	    change = RIGHT_CLICK_DECREMENT;
	}

	// Rather than make changes for each case, we'll mark which index
	// to change and how to change the message
	int indexToChange = 0;
	String message = "";
	
	// deal with which button was pressed
	if (clicked.equals(yellowGoalLeftButton)) {
	    indexToChange = YELLOW_GOAL_LEFT_INDEX;
	    message = YELLOW_GOAL_LEFT_STRING;
	}


	else if (clicked.equals(yellowGoalRightButton)) {
	    indexToChange = YELLOW_GOAL_RIGHT_INDEX;
	    message = YELLOW_GOAL_RIGHT_STRING;
	    

	}

	else if (clicked.equals(blueGoalLeftButton)) {
	    indexToChange = BLUE_GOAL_LEFT_INDEX;
	    message = BLUE_GOAL_LEFT_STRING;

	}

	else if (clicked.equals(blueGoalRightButton)) {
	    indexToChange = BLUE_GOAL_RIGHT_INDEX;
	    message = BLUE_GOAL_RIGHT_STRING;
	}

	else if (clicked.equals(blueYellowPostButton)) {
	    indexToChange = BLUE_YELLOW_POST_INDEX;
	    message = BLUE_YELLOW_STRING;
	}

	else if (clicked.equals(yellowBluePostButton)) {
	    indexToChange = YELLOW_BLUE_POST_INDEX;
	    message = YELLOW_BLUE_STRING;
	}


	else if (clicked.equals(blueBackstopButton)) {
	    indexToChange = BLUE_BACKSTOP_INDEX;
	    message = BLUE_BACKSTOP_STRING;
	}

	else if (clicked.equals(yellowBackstopButton)) {
	    indexToChange = YELLOW_BACKSTOP_INDEX;
	    message = YELLOW_BACKSTOP_STRING;

	}

	else if (clicked.equals(ballButton)) {
	    indexToChange = BALL_INDEX;
	    message = BALL_STRING;

	}


	    
	if (!leftClick && true) {
	    //seen[ai.getCurrentOfflineImage()][indexToChange] <= 0) {
		
	    // We dont' want negative numbers, so eat up the mouse click
	    // without doing anything
	}

	// Use the info we gathered from previous if else statements to
	// change the button and array as necessary
	else {
            /*
	    seen[ai.getCurrentOfflineImage()][indexToChange] += change;
	    clicked.setText(message + "" + seen[ai.getCurrentOfflineImage()]
            [indexToChange]);*/
	    
	}
    
    
       
    }
          
    public void	mouseEntered(MouseEvent e) {}
          
    public void	mouseExited(MouseEvent e) {}
          
    public void mousePressed(MouseEvent e) {}
          
    public void mouseReleased(MouseEvent e) {}
          

    


    // Deal with the buttons that are left click only
    public void actionPerformed (ActionEvent e) {
	String command = e.getActionCommand();
	


	// Reset the numbers on all the buttons to 0
	if (command.equals(RESET_ACTION)) {
	    resetCounts();
	    
	}

	else if (command.equals(LAST_FRAME_ACTION)) {
            // ai.regressFrame();
	}
	
	else if (command.equals(NEXT_FRAME_ACTION)) {
            //	    ai.advanceFrame();
	}

	
	else if (command.equals(SAVE_ACTION)) {
	    
	    writeLogFile();
	    
	}

	else if (command.equals(SAVE_QUIT_ACTION)) {
	    writeLogFile();
	    	    
            //	    ai.stopKeyGenerating();
	}

	else if (command.equals(QUIT_WO_SAVE_ACTION)) {
	    
	    System.out.println("Logging session terminated without saving");
            //	    ai.stopKeyGenerating();

	}







    }

    // Resets the values of all the keyGenPanel JButtons to 0 if user goofed
    // Change text on buttons
    public void resetCounts() {
	
	
	// Set to 0 all ints in the array at the index of current frame
	for (int i = 0; i < NUM_OBJECTS; i++) {
            //	    seen[ai.getCurrentOfflineImage()][i] = 0;

	}

	// Misc
	ballButton.setText(BALL_STRING + "0");
	blueYellowPostButton.setText(BLUE_YELLOW_STRING + "0");
	yellowBluePostButton.setText(YELLOW_BLUE_STRING + "0");

	// Blue
	blueGoalLeftButton.setText(BLUE_GOAL_LEFT_STRING + "0");
	blueGoalRightButton.setText(BLUE_GOAL_RIGHT_STRING + "0");
	blueBackstopButton.setText(BLUE_BACKSTOP_STRING + "0");
	
	// Goal
	yellowGoalLeftButton.setText(YELLOW_GOAL_LEFT_STRING + "0");
	yellowGoalRightButton.setText(YELLOW_GOAL_RIGHT_STRING + "0");
	yellowBackstopButton.setText(YELLOW_BACKSTOP_STRING + "0");
    }


    /* Update the labels on the key generating buttons to reflect the number
       of objects seen (as loaded from the log file or previously entered during
       this session)
       e.g. after having pressed the Next Frame button, this method is called
       to change the text in the buttons to reflect that no objects have yet 
       been logged.

     */


    public void updateCounts() {/*
	// Update misc counts
	ballButton.setText(BALL_STRING + "" +
				      seen[ai.getCurrentOfflineImage()]
				      [BALL_INDEX]);

	blueYellowPostButton.setText(BLUE_YELLOW_STRING + "" +
				     seen[ai.getCurrentOfflineImage()]
				     [BLUE_YELLOW_POST_INDEX]);

	yellowBluePostButton.setText(YELLOW_BLUE_STRING + "" +
				     seen[ai.getCurrentOfflineImage()]
				     [YELLOW_BLUE_POST_INDEX]);

	// Update blue goal counts
	blueGoalLeftButton.setText(BLUE_GOAL_LEFT_STRING + "" +
				   seen[ai.getCurrentOfflineImage()]
				     [BLUE_GOAL_LEFT_INDEX]);
 
	blueGoalRightButton.setText(BLUE_GOAL_RIGHT_STRING + "" +
				    seen[ai.getCurrentOfflineImage()]
				    [BLUE_GOAL_RIGHT_INDEX]);

	blueBackstopButton.setText(BLUE_BACKSTOP_STRING + "" + 
				 seen[ai.getCurrentOfflineImage()]
				 [BLUE_BACKSTOP_INDEX]);

	
	// update yellow counts
	yellowGoalLeftButton.setText(YELLOW_GOAL_LEFT_STRING + "" +
				     seen[ai.getCurrentOfflineImage()]
				     [YELLOW_GOAL_LEFT_INDEX]);

	yellowGoalRightButton.setText(YELLOW_GOAL_RIGHT_STRING + "" +
				      seen[ai.getCurrentOfflineImage()]
				      [YELLOW_GOAL_RIGHT_INDEX]);

	yellowBackstopButton.setText(YELLOW_BACKSTOP_STRING + "" +
				     seen[ai.getCurrentOfflineImage()]
				     [YELLOW_BACKSTOP_INDEX]);

                                */
	


    }


    /* Used to save current logging session to disk
       Please note - if you change the indices for the different objects,
       you MUST CHANGE THE HEADER IN THIS METHOD.  If you don't, your
       logs will not accurately reflect what you tagged!

     */
    public void writeLogFile() {
	/*
	// Retrieve the file names
	String[] imageStrings = ai.getImageStrings();
	if (imageStrings == null) {
	    System.out.println("Error: Unable to retrieve list of file " +
			       "names; cannot write log to file");
	    return;
	}

	try {
	    BufferedWriter out = new BufferedWriter(new FileWriter(log));
	    
	    
	    // Write header to make sense of columns of data
	    out.write("Ball\tBY\tYB\tBGLP\tBGRP\tBBack\tYGLP\tYGRP\tYBack\t" +
		      "Frame Number");
	    out.newLine();
	    out.write("-----------------------------------------------------"+
		      "----------------");
	    out.newLine();

	


	    for (int i = 0; i < ai.getTotalOfflineImages() ; i++) {
		for (int j = 0; j < NUM_OBJECTS; j++) {
		    out.write("" + seen[i][j] + "\t");
		}
		out.write(imageStrings[i]);
		out.newLine();
	    }

	    out.close();
	    System.out.println("Log saved successfully");
			    

	}
	catch (IOException b) {
	    System.out.println("Error: Could not open file");
	}

	*/


    }



    /* Used to read a log file into memory.  Lost must have the following form:
       Int /t Int /t ... Int.FRM /n

       where the number of numbers per line equals NUM_OBJECTS and Int.FRM 
       is the number of the FRM that was read.

       e.g. if we had previously read a directory with only one image then our 
       log would look like the following:

       0    0    0    0    0    0    0    0.FRM
       (In the log files written by writeLogFile() method, there are two 
       header lines above the columns to explain what each column represents).

       Keyword arguments:
       reader - the BufferedReader used to read lines from the .txt file
       expectedLines - the number of lines that should be in the file, namely 
       the number of .FRM files in the image directory


     */


    public void parseLogFile(BufferedReader reader, int expectedLines) {
	
	// Since we write logs with two lines of filler (the explanation of
	// what the columns are and one line of dashes) we must read these
	// lines or we will be screwed when trying to read in ints
	try {
	    reader.readLine();
	    reader.readLine();
	}
	catch (IOException E) {
	    System.out.println("Error: log file was not long enough" +
			       "(did not accomodate header)");
	}


	
	seen = new int[expectedLines][NUM_OBJECTS];
	
	String[] theLines = new String[expectedLines];

	for (int i = 0; i < expectedLines; i++) {
	    try {
		theLines[i] = reader.readLine();
	    }

	    catch (IOException E) {
		System.out.println ("Error: the log file had fewer entries" +
				    "than there are files in directory");
	    }
	    
	    if (theLines[i] != null) {

		String[] split = theLines[i].split("\t");

		
		// We will ignore the last indexed object in the split array 
		// which represents the file name
		for (int j = 0; j < NUM_OBJECTS; j++) {
		    try{
			// Read what was in the file at that column
			seen[i][j] = Integer.parseInt(split[j]); 
		
			
		    }

		    
		    catch (NumberFormatException E) {
			System.out.println("Error: unable to cast " + split[j] 
					   + "into an int");
			// If there's an invalid entry, just fill it in with a 0
			seen[i][j] = 0;
			
		    }
		}


	    }
	    
	 
	}

	

        





    }

}



/*

	    // Write the log to file only if we've initialized the seen array
	    // and only if we're actively making keys
	    if (generatingKey && seen != null && ai.offlineImages != null) {
	    
		writeLogFile();
			    


	    
	    }
*/
