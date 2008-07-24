package TOOL.PlayBookEditor;

import java.awt.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.undo.*;
import java.awt.event.*;
import java.util.Vector;
import TOOL.Image.PlayBookEditorViewer;

/**
 * Here are the components used for controlling the test mode in the PBE
 * @author Tucker Hermans
 */
public class PBTestModePanel extends PBModePanel {

    public static final int AIBO_MAX_ROBOTS = 4;
    public static final int NAO_MAX_ROBOTS = 3;

    protected JLabel activeRobotTitle, ballXLabel, ballYLabel;
    protected JComboBox activeRobotsBox;
    protected JButton drawBallButton, selectSubRoleButton;
    private boolean isBallDrawn;
    private SubRole curSubRole;
    private JSpinner ballXSpinner, ballYSpinner;
    private int MIN_BALL_X, MAX_BALL_X;
    private int MIN_BALL_Y, MAX_BALL_Y;
    public PBTestModePanel(PlayBookEditor theEditor)
    {
	super(theEditor);
	// Num active robots to draw
	activeRobotTitle = new JLabel("Num active robots: ");
	Vector<Integer> roboCountList = new Vector<Integer>();
	for (int i = AIBO_MAX_ROBOTS; i >= 1; i--) {
	    roboCountList.add(new Integer(i));
	}
	activeRobotsBox = new JComboBox(roboCountList);
	drawBallButton = new JButton("Draw Ball");
	selectSubRoleButton = new JButton("Select SubRole");
	isBallDrawn = false;

	getFieldLimits();
	ballXLabel = new JLabel("Ball X:");
	ballYLabel = new JLabel("Ball Y:");
	ballXSpinner = new JSpinner(new SpinnerNumberModel(250.0, MIN_BALL_X,
							   MAX_BALL_X, 10.));
	ballYSpinner = new JSpinner(new SpinnerNumberModel(375.0, MIN_BALL_Y,
							   MAX_BALL_Y, 10.));
	add(activeRobotTitle);
	add(activeRobotsBox);
	add(ballXLabel);
	add(ballXSpinner);
	add(ballYLabel);
	add(ballYSpinner);
	add(selectSubRoleButton);
	add(drawBallButton);

	setupListeners();
    }

    protected void setupListeners()
    {
	drawBallButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    if( isBallDrawn) {
			editor.setBallVisible(false);
			isBallDrawn = false;
			drawBallButton.setText("Draw Ball");
		    } else {
			editor.setBallVisible(true);
			isBallDrawn = true;
			drawBallButton.setText("Hide Ball");
		    }
		}
	    });
	selectSubRoleButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e)
		{
		    selectSubRole();
		}});

	ballXSpinner.addChangeListener(new ChangeListener() {
		//public void actionPerformed(ActionEvent e)
		public void stateChanged(ChangeEvent e)
		{
		    updateBallPos();
		}});
	ballYSpinner.addChangeListener(new ChangeListener() {
		//public void actionPerformed(ActionEvent e)
		public void stateChanged(ChangeEvent e)
		{
		    updateBallPos();
		}});

    }

    protected void selectSubRole()
    {
	curSubRole = (SubRole)editor.treePanel.subRoleView.getSelectedValue();
	editor.setEditingText(curSubRole.toString());
	updatePosition();
    }

    protected void updateBallPos()
    {
	Ball b = editor.getBall();
	SpinnerNumberModel xmod = (SpinnerNumberModel)ballXSpinner.getModel();
	SpinnerNumberModel ymod = (SpinnerNumberModel)ballYSpinner.getModel();
	b.setX(xmod.getNumber().intValue());
	b.setY(ymod.getNumber().intValue());
	updatePosition();
    }

    protected void updatePosition()
    {
	Point p = curSubRole.getPosition().getCurrentPos(editor.getBall());
	editor.getViewer().updateRobotPos(p, 0);
    }

    public void changeBallLoc(Point p)
    {
	p.x = clipBallX(p.x);
	p.y = clipBallY(p.y);
	ballXSpinner.setValue(p.x);
	ballYSpinner.setValue(p.y);
	updateBallPos();
    }

    private int clipBallX(int x)
    {
	if (x < MIN_BALL_X)
	    return MIN_BALL_X;

	if (x > MAX_BALL_X)
	    return MAX_BALL_X;

	return x;
    }

    private int clipBallY(int y)
    {
	if (y < MIN_BALL_Y)
	    return MIN_BALL_Y;

	if (y > MAX_BALL_Y)
	    return MAX_BALL_Y;

	return y;
    }

    public void getFieldLimits()
    {
	MIN_BALL_X = 0;
	MAX_BALL_X = (int)editor.getField().FIELD_WIDTH;
	MIN_BALL_Y = 0;
	MAX_BALL_Y = (int)editor.getField().FIELD_HEIGHT;
    }
}

