package TOOL.Learning;

import javax.swing.JPanel;
import javax.swing.JCheckBox;
import javax.swing.JTextArea;
import java.awt.Component;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import javax.swing.JComponent;
import javax.swing.JSlider;
import javax.swing.JButton;
import java.awt.event.KeyListener;
import java.awt.event.KeyEvent;

import java.awt.event.MouseWheelListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;

import javax.swing.InputMap;
import javax.swing.ActionMap;
import java.awt.event.InputEvent;
import javax.swing.KeyStroke;
import javax.swing.AbstractAction;
import TOOL.Data.DataListener;
import TOOL.Data.Frame;
import TOOL.Data.DataSet;
import javax.swing.BoxLayout;
import javax.swing.JTextPane;
import javax.swing.JTextField;
import javax.swing.JComboBox;
import javax.swing.text.*;
import java.awt.GridLayout;
import java.awt.Font;
import java.awt.Dimension;
import java.awt.Cursor;

import TOOL.Image.ImageOverlay;

import TOOL.Calibrate.ColorSwatchParent;
import TOOL.Data.Classification.KeyFrame.*;
import TOOL.TOOL;

/** A simple panel containing various buttons to set the data for a frame.  As we
   extend the information that we learn on, we'll have to extend this panel
   accordingly.

   This panel communicates directly with the main "Learning" class.  Getting info
   from it on what to display.  And informing it when fields are set by the user.

   @author Eric Chown
 */


public class KeyPanel extends JPanel implements ItemListener {

	// All of the stuff the user can set
	private JButton   human;
    private JCheckBox ball;
	private JComboBox blueGoal;
	private JComboBox yellowGoal;
	private JComboBox cross;
	private JComboBox redRobots;
	private JComboBox blueRobots;
	private JComboBox lCorners;
	private JComboBox tCorners;
	private JComboBox ccCorners;
    private Learning learn;

	// labels showing what the vision system has found for this frame
	private JTextField visionHuman, visionBall, visionYellow, visionBlue;
	private JTextField visionCross, visionRedRobot, visionBlueRobot;
	private JTextField visionLCorner, visionTCorner, visionCcCorner;


	/** Create the panel and set up the listeners.
	 */
	public KeyPanel(Learning aLearn)  {
		super();
		learn = aLearn;
		setUpWindows();
		setUpListeners();
		setUpShortcuts();
	}

	/**
	 * Setup the keyboard shortcuts
	 */
	public void setUpShortcuts(){
		InputMap im = this.getInputMap(WHEN_IN_FOCUSED_WINDOW);
		ActionMap am = this.getActionMap();

        im.put(KeyStroke.getKeyStroke(KeyEvent.VK_U, 0), "updateInfo");

		am.put("updateInfo", new AbstractAction("updateInfo"){
				public void actionPerformed(ActionEvent e){
					human.doClick();
				}
			});

		im.put(KeyStroke.getKeyStroke(KeyEvent.VK_L, 0), "LCorner");

		am.put("LCorner", new AbstractAction("LCorner"){
				public void actionPerformed(ActionEvent e){
					int index = lCorners.getSelectedIndex();
					lCorners.setSelectedIndex((lCorners.getSelectedIndex() + 1) %
											  (lCorners.getItemCount()-1));
				}
			});

		im.put(KeyStroke.getKeyStroke(KeyEvent.VK_C, 0), "CcCorner");

		am.put("CcCorner", new AbstractAction("CcCorner"){
				public void actionPerformed(ActionEvent e){
					int index = ccCorners.getSelectedIndex();
					ccCorners.setSelectedIndex((ccCorners.getSelectedIndex() + 1) %
											  (ccCorners.getItemCount()-1));
				}
			});

		im.put(KeyStroke.getKeyStroke(KeyEvent.VK_T, 0), "TCorner");

		am.put("TCorner", new AbstractAction("TCorner"){
				public void actionPerformed(ActionEvent e){
					int index = tCorners.getSelectedIndex();
					tCorners.setSelectedIndex((tCorners.getSelectedIndex() + 1) %
											  (tCorners.getItemCount()-1));
				}
			});
	}

	/** Add all of the buttons and initialize to appropriate values.
	 */
    public void setUpWindows() {

		human = new JButton("Update Information (U)");
		ball = new JCheckBox("Ball");

		blueGoal = new JComboBox();
		blueGoal.addItem("Left Post");
		blueGoal.addItem("Right Post");
		blueGoal.addItem("Both Posts");
		blueGoal.addItem("Unknown Post");
		blueGoal.addItem("No Blue Post");
		blueGoal.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					JComboBox sourceBox = (JComboBox) e.getSource();
					setBlueGoalOverlay(sourceBox);
				}});
		yellowGoal = new JComboBox();
		yellowGoal.addItem("Left Post");
		yellowGoal.addItem("Right Post");
		yellowGoal.addItem("Both Posts");
		yellowGoal.addItem("Unknown Post");
		yellowGoal.addItem("No Yellow Post");
		yellowGoal.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					JComboBox sourceBox = (JComboBox) e.getSource();
					setYellowGoalOverlay(sourceBox);
				}});
		cross = new JComboBox();
		cross.addItem("Blue Cross");
		cross.addItem("Yellow Cross");
		cross.addItem("Both Crosses");
		cross.addItem("Unknown Cross");
		cross.addItem("No Cross");
		cross.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					JComboBox sourceBox = (JComboBox) e.getSource();
					setCrossOverlay(sourceBox);
				}});
		redRobots = new JComboBox();
		redRobots.addItem("No Red Robots");
		redRobots.addItem("One");
		redRobots.addItem("Two");
		redRobots.addItem("Three");
		redRobots.addItem("Four");
		redRobots.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					JComboBox sourceBox = (JComboBox) e.getSource();
					setRedRobotOverlay(sourceBox);
				}});
		blueRobots = new JComboBox();
		blueRobots.addItem("No Blue Robots");
		blueRobots.addItem("One");
		blueRobots.addItem("Two");
		blueRobots.addItem("Three");
		blueRobots.addItem("Four");
		blueRobots.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					JComboBox sourceBox = (JComboBox) e.getSource();
					setBlueRobotOverlay(sourceBox);
				}});

		lCorners = new JComboBox();
		lCorners.addItem("No L Corners");
		lCorners.addItem("One");
		lCorners.addItem("Two");
		lCorners.addItem("Three");
		lCorners.addItem("Four");
		lCorners.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					JComboBox sourceBox = (JComboBox) e.getSource();
					setLCornerOverlay(sourceBox);
				}});

		ccCorners = new JComboBox();
		ccCorners.addItem("No CC Corners");
		ccCorners.addItem("One");
		ccCorners.addItem("Two");
		ccCorners.addItem("Three");
		ccCorners.addItem("Four");
		ccCorners.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					JComboBox sourceBox = (JComboBox) e.getSource();
					setCcCornerOverlay(sourceBox);
				}});

		tCorners = new JComboBox();
		tCorners.addItem("No T Corners");
		tCorners.addItem("One");
		tCorners.addItem("Two");
		tCorners.addItem("Three");
		tCorners.addItem("Four");
		tCorners.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					JComboBox sourceBox = (JComboBox) e.getSource();
					setTCornerOverlay(sourceBox);
				}});


		visionHuman = new JTextField(learn.getHuman());
		visionBall = new JTextField(learn.getBallString());
		visionBlue = new JTextField(learn.getBlueGoalString());
		visionYellow = new JTextField(learn.getYellowGoalString());
		visionCross = new JTextField(learn.getCrossString());
		visionRedRobot = new JTextField(learn.getRedRobotString());
		visionBlueRobot = new JTextField(learn.getBlueRobotString());
		visionLCorner = new JTextField(learn.getLCornerString());
		visionTCorner = new JTextField(learn.getTCornerString());
		visionCcCorner = new JTextField(learn.getCcCornerString());

		add(ball);
		add(visionBall);
		add(blueGoal);
		add(visionBlue);
		add(yellowGoal);
		add(visionYellow);
		add(cross);
		add(visionCross);
		add(redRobots);
		add(visionRedRobot);
		add(blueRobots);
		add(visionBlueRobot);
		add(lCorners);
		add(visionLCorner);
		add(tCorners);
		add(visionTCorner);
		add(ccCorners);
		add(visionCcCorner);
		add(human);
		add(visionHuman);

        setLayout(new GridLayout(10,2));
	}


	/** We actually have some redundancy as we already have an item listener
		which might have to be removed.  But this listens for the buttons.
	 */
    private void setUpListeners() {
        //learn.getTool().getDataManager().addDataListener(this);
        human.addActionListener(new ActionListener(){
                public void actionPerformed(ActionEvent e) {
                    learn.setHuman(true);
                }
            });

        ball.addActionListener(new ActionListener(){
                public void actionPerformed(ActionEvent e) {
                    learn.setBall(ball.isSelected());
                }
            });
    }

	/** Displays the appropriate text for the field.  This is called by the
		main learning object.
		@param s      text to display in the human field
	 */
	public void setHuman(String s) {
		visionHuman.setText(s);
	}

	/** Displays the appropriate text for the field.  This is called by the
		main learning object.
		@param s      text to display in the ball field
	 */
	public void setBall(String s) {
		visionBall.setText(s);
	}

	/** Displays the appropriate text for the field.  This is called by the
		main learning object.
		@param s      text to display in the cross field
	 */
	public void setCross(String s) {
		visionCross.setText(s);
	}

	/** Displays the appropriate text for the field.  This is called by the
		main learning object.
		@param s      text to display in the blue goal field
	 */
	public void setBlueGoal(String s) {
		visionBlue.setText(s);
	}

	/** Displays the appropriate text for the field.  This is called by the
		main learning object.
		@param s      text to display in the yellow goal field
	 */
	public void setYellowGoal(String s) {
		visionYellow.setText(s);
	}

	/** Displays the appropriate text for the field.  This is called by the
		main learning object.
		@param s      text to display in the red robot field
	 */
	public void setRedRobot(String s) {
		visionRedRobot.setText(s);
	}

	/** Displays the appropriate text for the field.  This is called by the
		main learning object.
		@param s      text to display in the blue robot field
	 */
	public void setBlueRobot(String s) {
		visionBlueRobot.setText(s);
	}

	/** Displays the appropriate text for the field.  This is called by the
		main learning object.
		@param s      text to display in the l corner field
	 */
	public void setLCorner(String s) {
		visionLCorner.setText(s);
	}

	/** Displays the appropriate text for the field.  This is called by the
		main learning object.
		@param s      text to display in the cc corner field
	 */
	public void setCcCorner(String s) {
		visionCcCorner.setText(s);
	}

	/** Displays the appropriate text for the field.  This is called by the
		main learning object.
		@param s      text to display in the t corner field
	 */
	public void setTCorner(String s) {
		visionTCorner.setText(s);
	}

	/** Sets the input device to reflect either what the Key file says of what the
		vision system says depending on if the key file exists.
		@param h     status of human approval
	 */
	public void setHumanStatus(boolean h) {
		if (h)
			visionHuman.setText("Yes");
		else
			visionHuman.setText("No");
	}

	/** Sets the input device to reflect either what the Key file says of what the
		vision system says depending on if the key file exists.
		@param h     status of ball
	 */
	public void setBallStatus(boolean b) {
		ball.setSelected(b);
	}

	/** Sets the input device to reflect either what the Key file says of what the
		vision system says depending on if the key file exists.
		@param h     status of blue goal
	 */
	public void setBlueGoalStatus(GoalType b)
	{
		switch (b) {
		case NO_POST: blueGoal.setSelectedIndex(4); break;
		case LEFT: blueGoal.setSelectedIndex(0); break;
		case RIGHT: blueGoal.setSelectedIndex(1); break;
		case BOTH: blueGoal.setSelectedIndex(2); break;
		case UNSURE: blueGoal.setSelectedIndex(3); break;
		}
	}

	/** Sets the input device to reflect either what the Key file says of what the
		vision system says depending on if the key file exists.
		@param h     status of yellow goal
	 */
	public void setYellowGoalStatus(GoalType y)
	{
		switch (y) {
		case NO_POST: yellowGoal.setSelectedIndex(4); break;
		case LEFT: yellowGoal.setSelectedIndex(0); break;
		case RIGHT: yellowGoal.setSelectedIndex(1); break;
		case BOTH: yellowGoal.setSelectedIndex(2); break;
		case UNSURE: yellowGoal.setSelectedIndex(3); break;
		}
	}

	/** Sets the input device to reflect either what the Key file says of what the
		vision system says depending on if the key file exists.
		@param h     status of cross
	 */
	public void setCrossStatus(CrossType c) {
		switch (c) {
		case NO_CROSS: cross.setSelectedIndex(4); break;
		case BLUE: cross.setSelectedIndex(0); break;
		case YELLOW: cross.setSelectedIndex(1); break;
		case DOUBLE_CROSS: cross.setSelectedIndex(2); break;
		case UNKNOWN: cross.setSelectedIndex(3); break;
		}
	}

	/** Sets the input device to reflect either what the Key file says of what the
		vision system says depending on if the key file exists.
		@param h     status of red robots
	 */
	public void setRedRobotStatus(int num) {
		// currently we can't see more than 3 at a time
		if (num > 3) num = 3;
		redRobots.setSelectedIndex(num);
	}

	/** Sets the input device to reflect either what the Key file says of what the
		vision system says depending on if the key file exists.
		@param h     status of human approval
	 */
	public void setBlueRobotStatus(int num) {
		if (num > 3) num = 3;
		blueRobots.setSelectedIndex(num);
	}

	/** Sets the input device to reflect either what the Key file says of what the
		vision system says depending on if the key file exists.
		@param num    how many corners
	 */
	public void setLCornerStatus(int num) {
		if (num > 3) num = 3;
		lCorners.setSelectedIndex(num);
	}

	/** Sets the input device to reflect either what the Key file says of what the
		vision system says depending on if the key file exists.
		@param num    how many corners
	 */
	public void setCcCornerStatus(int num) {
		if (num > 3) num = 3;
		ccCorners.setSelectedIndex(num);
	}

	/** Sets the input device to reflect either what the Key file says of what the
		vision system says depending on if the key file exists.
		@param num how many corners
	 */
	public void setTCornerStatus(int num) {
		if (num > 3) num = 3;
		tCorners.setSelectedIndex(num);
	}

	/** The user has changed the value of this overlay so tell the learning system
		about it.  Based on the value set the goal type.
		@param sourceBox   the item that was set
	 */
	public void setBlueGoalOverlay(JComboBox sourceBox) {
		if (((String) sourceBox.getSelectedItem()).equals("Left Post")) {
			learn.setBlueGoal(GoalType.LEFT);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Right Post")) {
			learn.setBlueGoal(GoalType.RIGHT);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Both Posts")) {
			learn.setBlueGoal(GoalType.BOTH);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Unknown Post")) {
			learn.setBlueGoal(GoalType.UNSURE);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("No Blue Post")) {
			learn.setBlueGoal(GoalType.NO_POST);
		}
	}

	/** The user has changed the value of this overlay so tell the learning system
		about it.  Based on the value set the goal type.
		@param sourceBox   the item that was set
	 */
	public void setYellowGoalOverlay(JComboBox sourceBox) {
		if (((String) sourceBox.getSelectedItem()).equals("Left Post")) {
			learn.setYellowGoal(GoalType.LEFT);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Right Post")) {
			learn.setYellowGoal(GoalType.RIGHT);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Both Posts")) {
			learn.setYellowGoal(GoalType.BOTH);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Unknown Post")) {
			learn.setYellowGoal(GoalType.UNSURE);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("No Yellow Post")) {
			learn.setYellowGoal(GoalType.NO_POST);
		}
	}

	/** The user has changed the value of this overlay so tell the learning system
		about it.  Based on the value set the goal type.
		@param sourceBox   the item that was set
	 */
	public void setCrossOverlay(JComboBox sourceBox) {
		if (((String) sourceBox.getSelectedItem()).equals("Yellow Cross")) {
			learn.setCross(CrossType.YELLOW);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Blue Cross")) {
			learn.setCross(CrossType.BLUE);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Both Crosses")) {
			learn.setCross(CrossType.DOUBLE_CROSS);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Unknown Cross")) {
			learn.setCross(CrossType.UNKNOWN);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("No Cross")) {
			learn.setCross(CrossType.NO_CROSS);
		}
	}

	/** The user has changed the value of this overlay so tell the learning system
		about it.  Based on the value set the goal type.
		@param sourceBox   the item that was set
	 */
	public void setRedRobotOverlay(JComboBox sourceBox) {
		if (((String) sourceBox.getSelectedItem()).equals("No Red Robots")) {
			learn.setRedRobot(0);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("One")) {
			learn.setRedRobot(1);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Two")) {
			learn.setRedRobot(2);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Three")) {
			learn.setRedRobot(3);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Four")) {
			learn.setRedRobot(4);
		}
	}

	/** The user has changed the value of this overlay so tell the learning system
		about it.  Based on the value set the goal type.
		@param sourceBox   the item that was set
	 */
	public void setBlueRobotOverlay(JComboBox sourceBox) {
		if (((String) sourceBox.getSelectedItem()).equals("No Blue Robots")) {
			learn.setBlueRobot(0);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("One")) {
			learn.setBlueRobot(1);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Two")) {
			learn.setBlueRobot(2);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Three")) {
			learn.setBlueRobot(3);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Four")) {
			learn.setBlueRobot(4);
		}
	}

	/** The user has changed the value of this overlay so tell the learning system
		about it.  Based on the value set the goal type.
		@param sourceBox   the item that was set
	 */
	public void setLCornerOverlay(JComboBox sourceBox) {
		if (((String) sourceBox.getSelectedItem()).equals("No L Corners")) {
			learn.setLCorners(0);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("One")) {
			learn.setLCorners(1);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Two")) {
			learn.setLCorners(2);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Three")) {
			learn.setLCorners(3);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Four")) {
			learn.setLCorners(4);
		}
	}

	/** The user has changed the value of this overlay so tell the learning system
		about it.  Based on the value set the goal type.
		@param sourceBox   the item that was set
	 */
	public void setCcCornerOverlay(JComboBox sourceBox) {
		if (((String) sourceBox.getSelectedItem()).equals("No CC Corners")) {
			learn.setCcCorners(0);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("One")) {
			learn.setCcCorners(1);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Two")) {
			learn.setCcCorners(2);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Three")) {
			learn.setCcCorners(3);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Four")) {
			learn.setCcCorners(4);
		}
	}

	/** The user has changed the value of this overlay so tell the learning system
		about it.  Based on the value set the goal type.
		@param sourceBox   the item that was set
	 */
	public void setTCornerOverlay(JComboBox sourceBox) {
		if (((String) sourceBox.getSelectedItem()).equals("No T Corners")) {
			learn.setTCorners(0);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("One")) {
			learn.setTCorners(1);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Two")) {
			learn.setTCorners(2);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Three")) {
			learn.setTCorners(3);
		}
		else if (((String) sourceBox.getSelectedItem()).equals("Four")) {
			learn.setTCorners(4);
		}
	}

    /** Listens to the check boxes. This method can probably be dumped.
		@param e    the event to be processed.
	 */
    public void itemStateChanged(ItemEvent e) {
        Object source = e.getItemSelectable();

        if (source == ball) {
			learn.setBall(!learn.getBall());
        }
	}

}
