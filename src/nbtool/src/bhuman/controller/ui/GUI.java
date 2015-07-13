package controller.ui;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;
import java.awt.GridLayout;
import java.awt.Image;
import java.awt.Insets;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.AbstractButton;
import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JRadioButton;
import javax.swing.JToggleButton;

import common.Log;
import common.TotalScaleLayout;
import controller.Clock;
import controller.EventHandler;
import controller.action.ActionBoard;
import controller.action.GCAction;
import controller.net.RobotOnlineStatus;
import controller.net.RobotWatcher;
import data.AdvancedData;
import data.GameControlData;
import data.HL;
import data.PlayerInfo;
import data.Rules;
import data.SPL;
import data.Teams;


/**
 * @author Michel Bartsch
 * 
 * This is the main GUI.
 * In this class you will find the whole graphical output and the bindings
 * of buttons to their actions, nothing less and nothing more.
 */
public class GUI extends JFrame implements GCGUI
{
    private static final boolean IS_OSX = System.getProperty("os.name").contains("OS X");
    private static final boolean IS_APPLE_JAVA = IS_OSX && System.getProperty("java.version").compareTo("1.7") < 0;
    private static final Insets insets = IS_APPLE_JAVA ? new Insets (2, -30, 2, -30) : null;
    private static final String BUTTON_MASK = IS_APPLE_JAVA
            ? "<html><div style=\"padding: 0px 12px\"><center>%s</center></div></html>"
            : "<html><center>%s</center></html>";
    
    /** Fix button centering for Apple Java. */
    private class Button extends JButton
    {
        private static final long serialVersionUID = -1533689100759569853L;

        public Button()
        {
            setMargin(insets);
        }
        
        public Button(String text)
        {
            setMargin(insets);
            setText(text);
        }
        
        public void setText(String text)
        {
            super.setText(String.format(BUTTON_MASK, text));
        }
    }
    
    /** Fix button centering for Apple Java. */
    private class ToggleButton extends JToggleButton
    {
        private static final long serialVersionUID = -7733709666734108610L;

        public ToggleButton()
        {
            setMargin(insets);
        }
        
        public ToggleButton(String text)
        {
            setMargin(insets);
            setText(text);
        }
        
        public void setText(String text)
        {
            super.setText(String.format(BUTTON_MASK, text));
        }
    }
    
    private static final long serialVersionUID = 1L;
    
    /**
     * Some constants defining this GUI`s appearance as their names say.
     * Feel free to change them and see what happens.
     */
    private static final int WINDOW_WIDTH = 1024;
    private static final int WINDOW_HEIGHT = 768;
    private static final int STANDARD_FONT_SIZE = 17;
    private static final int TITLE_FONT_SIZE = 24;
    private static final String STANDARD_FONT = "Helvetica";
    private static final int GOALS_FONT_SIZE = 60;
    private static final int TIME_FONT_SIZE = 50;
    private static final int TIME_SUB_FONT_SIZE = 40;
    private static final int TIMEOUT_FONT_SIZE = 14;
    private static final int STATE_FONT_SIZE = 12;
    private static final String WINDOW_TITLE = "GameController";
    private static final String ICONS_PATH = "config/icons/";
    private static final String[][] BACKGROUND_SIDE = {{"robot_left_blue.png",
                                                        "robot_left_red.png",
                                                        "robot_left_yellow.png",
                                                        "robot_left_black.png"},
                                                       {"robot_right_blue.png",
                                                        "robot_right_red.png",
                                                        "robot_right_yellow.png",
                                                        "robot_right_black.png"}};
    private static final String BACKGROUND_MID = "field.png";
    private static final String BACKGROUND_CLOCK_SMALL = "time_ground_small.png";
    private static final String BACKGROUND_CLOCK = "time_ground.png";
    private static final String KICKOFF = "Kickoff";
    private static final String KICKOFF_PENALTY_SHOOTOUT = "P.-taker";
    private static final String PUSHES = "Pushes";
    private static final String SHOT = "Shot";
    private static final String SHOTS = "Shots";
    private static final String EJECTED = "Ejected";
    private static final String ONLINE = "wlan_status_green.png";
    private static final String OFFLINE = "wlan_status_red.png";
    private static final String HIGH_LATENCY = "wlan_status_yellow.png";
    private static final String UNKNOWN_ONLINE_STATUS = "wlan_status_grey.png";
    private static final String TIMEOUT = "Timeout";
    private static final String REFEREE_TIMEOUT = "Referee<br/>Timeout";
    private static final String STUCK = "Global <br/> Game <br/> Stuck";
    private static final String OUT = "Out";
    private static final String STATE_INITIAL = "Initial";
    private static final String STATE_READY = "Ready";
    private static final String STATE_SET = "Set";
    private static final String STATE_PLAY = "Play";
    private static final String STATE_FINISH = "Finish";
    private static final String CLOCK_RESET = "reset.png";
    private static final String CLOCK_PAUSE = "pause.png";
    private static final String CLOCK_PLAY = "play.png";
    private static final String CLOCK_PLUS = "plus.png";
    private static final String FIRST_HALF = "First Half";
    private static final String SECOND_HALF = "Second Half";
    private static final String FIRST_HALF_SHORT = "1st Half";
    private static final String SECOND_HALF_SHORT = "2nd Half";
    private static final String FIRST_HALF_OVERTIME = "1st Extra";
    private static final String SECOND_HALF_OVERTIME = "2nd Extra";
    private static final String PENALTY_SHOOT = "Penalty Shots";
    private static final String PENALTY_SHOOT_SHORT = "Penalty";
    private static final String PEN_PUSHING = "Pushing";
    private static final String PEN_LEAVING = "Leaving the Field";
    private static final String PEN_MOTION_IN_SET = "Motion in Set";
    private static final String PEN_MOTION_IN_SET_SHORT = "Motion";
    private static final String PEN_INACTIVE = "Fallen / Inactive / Local Game Stuck";
    private static final String PEN_DEFENDER = "Illegal Defender";
    private static final String PEN_BALL_CONTACT = "Ball Holding / Hands";
    private static final String PEN_KICK_OFF_GOAL = "Kickoff Goal";
    private static final String PEN_COACH_MOTION = "Coach Motion";
    private static final String PEN_PICKUP = "Pick-Up";
    private static final String PEN_MANIPULATION = "Ball Manipulation";
    private static final String PEN_PHYSICAL = "Physical Contact";
    private static final String PEN_DEFENSE = "Illegal Defense";
    private static final String PEN_ATTACK = "Illegal Attack";
    private static final String PEN_PICKUP_INCAPABLE = "Pickup/Incapable";
    private static final String PEN_SERVICE = "Service";
    private static final String PEN_SUBSTITUTE = "Substitute";
    private static final String PEN_SUBSTITUTE_SHORT = "Sub";
    private static final String DROP_BALL = "Dropped Ball";
    private static final String TEAMMATE_PUSHING = "Teammate Pushing";
    private static final String CANCEL = "Cancel";
    private static final String COACH = "Coach";
    private static final String BACKGROUND_BOTTOM = "timeline_ground.png";
    private static final Color COLOR_HIGHLIGHT = Color.YELLOW;
    private static final Color COLOR_STANDARD = (new JButton()).getBackground();
    private static final int UNPEN_HIGHLIGHT_SECONDS = 10;
    private static final int TIMEOUT_HIGHLIGHT_SECONDS = 10;
    private static final int FINISH_HIGHLIGHT_SECONDS = 10;
    private static final int KICKOFF_BLOCKED_HIGHLIGHT_SECONDS = 3;

    /** Some attributes used in the GUI components. */
    private double lastSize = 0;
    private Font standardFont;
    private Font titleFont;
    private Font goalsFont;
    private Font timeFont;
    private Font timeSubFont;
    private Font timeoutFont;
    private Font stateFont;
    private ImageIcon clockImgReset;
    private ImageIcon clockImgPlay;
    private ImageIcon clockImgPause;
    private ImageIcon clockImgPlus;
    private ImageIcon lanOnline;
    private ImageIcon lanHighLatency;
    private ImageIcon lanOffline;
    private ImageIcon lanUnknown;
    private ImageIcon[][] backgroundSide;
    
    /** All the components of this GUI. */
    private ImagePanel[] side;
    private JLabel[] name;
    private JButton[] goalDec;
    private JButton[] goalInc;
    private JLabel[] goals;
    private JRadioButton[] kickOff;
    private ButtonGroup kickOffGroup;
    private JLabel[] pushes;
    private JPanel[] robots;
    private JButton[][] robot;
    private JLabel[][] robotLabel;
    private ImageIcon[][] lanIcon;
    private JProgressBar[][] robotTime;
    private JToggleButton refereeTimeout;
    private JToggleButton[] timeOut;
    private JButton[] stuck;
    private JButton[] out;
    private JPanel mid;
    private JToggleButton initial;
    private JToggleButton ready;
    private JToggleButton set;
    private JToggleButton play;
    private JToggleButton finish;
    private ButtonGroup stateGroup;
    private ImageButton clockReset;
    private ImagePanel clockContainer;
    private JLabel clock;
    private JLabel clockSub;
    private ImageButton incGameClock;
    private ImageButton clockPause;
    private JToggleButton firstHalf;
    private JToggleButton secondHalf;
    private JToggleButton firstHalfOvertime;
    private JToggleButton secondHalfOvertime;
    private JToggleButton penaltyShoot;
    private ButtonGroup halfGroup;
    private JToggleButton[] pen;
    private JButton dropBall;
    private ImagePanel bottom;
    private JPanel log;
    private JToggleButton[] undo;
    private JButton cancelUndo;
  
    
    /**
     * Creates a new GUI.
     * 
     * @param fullscreen    If true, the GUI tries to start using the full
     *                      size of the screen. Actually this means changing
     *                      the display`s resolution to the GUI`s size.
     * @param data      The starting data.
     */
    public GUI(boolean fullscreen, GameControlData data)
    {
        super(WINDOW_TITLE);
        setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        setResizable(true);

        GraphicsDevice gd = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();
        int width = gd.getDisplayMode().getWidth();
        int height = gd.getDisplayMode().getHeight();
        setLocation((width-WINDOW_WIDTH)/2, (height-WINDOW_HEIGHT)/2);
        
        addWindowListener(new WindowAdapter()
        {
            @Override
            public void windowClosing(WindowEvent e) {
                Clock.getInstance().interrupt();
                dispose();
            }
        });
        
        clockImgReset = new ImageIcon(ICONS_PATH+CLOCK_RESET);
        clockImgPlay = new ImageIcon(ICONS_PATH+CLOCK_PLAY);
        clockImgPause = new ImageIcon(ICONS_PATH+CLOCK_PAUSE);
        clockImgPlus = new ImageIcon(ICONS_PATH+CLOCK_PLUS);
        lanOnline = new ImageIcon(ICONS_PATH+ONLINE);
        lanHighLatency = new ImageIcon(ICONS_PATH+HIGH_LATENCY);
        lanOffline = new ImageIcon(ICONS_PATH+OFFLINE);
        lanUnknown = new ImageIcon(ICONS_PATH+UNKNOWN_ONLINE_STATUS);
        
        backgroundSide = new ImageIcon[2][4];
        for (int i=0; i<BACKGROUND_SIDE.length; i++) {
            for (int j=0; j<BACKGROUND_SIDE[i].length; j++) {
                backgroundSide[i][j] = new ImageIcon(ICONS_PATH+Rules.league.leagueDirectory+"/"+BACKGROUND_SIDE[i][j]);
            }
        }
        
        
        //Components
        side = new ImagePanel[2];
        for (int i=0; i<2; i++) {
            side[i] = new ImagePanel(backgroundSide[i][i].getImage());
            side[i].setOpaque(true);
        }
        mid = new ImagePanel(new ImageIcon(ICONS_PATH+BACKGROUND_MID).getImage());
        bottom = new ImagePanel(new ImageIcon(ICONS_PATH+BACKGROUND_BOTTOM).getImage());
        
        //--side--
        //  score
        name = new JLabel[2];
        goalDec = new JButton[2];
        goalInc = new JButton[2];
        goals = new JLabel[2];
        kickOff = new JRadioButton[3];
        kickOffGroup = new ButtonGroup();
        pushes = new JLabel[2];
        for (int i=0; i<2; i++) {
            name[i] = new JLabel(Teams.getNames(false)[data.team[i].teamNumber]);
            name[i].setHorizontalAlignment(JLabel.CENTER);
            name[i].setForeground(Rules.league.teamColor[data.team[i].teamColor]);
            goalInc[i] = new Button("+");
            goalDec[i] = new Button("-");
            kickOff[i] = new JRadioButton(KICKOFF);
            kickOff[i].setOpaque(false);
            kickOff[i].setHorizontalAlignment(JLabel.CENTER);
            kickOffGroup.add(kickOff[i]);
            goals[i] = new JLabel("0");
            goals[i].setHorizontalAlignment(JLabel.CENTER);
            pushes[i] = new JLabel("0");
            pushes[i].setHorizontalAlignment(JLabel.CENTER);
        }
        kickOff[2] = new JRadioButton();
        kickOffGroup.add(kickOff[2]);
        
        //  robots
        robots = new JPanel[2];
        if (Rules.league.isCoachAvailable) {
            robot = new JButton[2][Rules.league.teamSize+1];
            robotLabel = new JLabel[2][Rules.league.teamSize+1];
            lanIcon = new ImageIcon[2][Rules.league.teamSize+1];
            robotTime = new JProgressBar[2][Rules.league.teamSize+1];
        } else {
            robot = new JButton[2][Rules.league.teamSize];
            robotLabel = new JLabel[2][Rules.league.teamSize];
            lanIcon = new ImageIcon[2][Rules.league.teamSize];
            robotTime = new JProgressBar[2][Rules.league.teamSize];
        }
        for (int i=0; i<2; i++) {
            robots[i] = new JPanel();
            robots[i].setLayout(new GridLayout(robot[i].length, 1, 0, 10));
            robots[i].setOpaque(false);
            
            for (int j=0; j<robot[i].length; j++) {
                robot[i][j] = new Button();
                robotLabel[i][j] = new JLabel();
                robotLabel[i][j].setHorizontalAlignment(JLabel.CENTER);
                lanIcon[i][j] = lanUnknown;
                robotLabel[i][j].setIcon(lanIcon[i][j]);
                robotTime[i][j] = new JProgressBar();
                robotTime[i][j].setMaximum(1000);
                robotTime[i][j].setVisible(false);
                TotalScaleLayout robotLayout = new TotalScaleLayout(robot[i][j]);
                robot[i][j].setLayout(robotLayout);
                robotLayout.add(.1, .1, .8, .5, robotLabel[i][j]);
                robotLayout.add(.1, .7, .8, .2, robotTime[i][j]);
                robots[i].add(robot[i][j]);
            }
        }
        //  team
        timeOut = new JToggleButton[2];
        out = new JButton[2];
        for (int i=0; i<2; i++) {
            timeOut[i] = new ToggleButton(TIMEOUT);
            out[i] = new JButton(OUT);
        }
        if (Rules.league instanceof SPL) {
            stuck = new Button[2];
            for (int i=0; i<2; i++) {
                stuck[i] = new Button();
            }
        }
        
        //--mid--
        //  time
        clockReset = new ImageButton(clockImgReset.getImage());
        clockReset.setOpaque(false);
        clockReset.setBorder(null);
        if (Rules.league.lostTime) {
            clockContainer = new ImagePanel(new ImageIcon(ICONS_PATH+BACKGROUND_CLOCK_SMALL).getImage());
        } else {
            clockContainer = new ImagePanel(new ImageIcon(ICONS_PATH+BACKGROUND_CLOCK).getImage());
        }
        clockContainer.setOpaque(false);
        clock = new JLabel("10:00");
        clock.setForeground(Color.WHITE);
        clock.setHorizontalAlignment(JLabel.CENTER);
        clockPause = new ImageButton(clockImgReset.getImage());
        clockPause.setOpaque(false);
        clockPause.setBorder(null);
        clockSub = new JLabel("0:00");
        clockSub.setHorizontalAlignment(JLabel.CENTER);
        incGameClock = new ImageButton(clockImgPlus.getImage());
        incGameClock.setOpaque(false);
        incGameClock.setBorder(null);
        if (!Rules.league.overtime) {
            firstHalf = new ToggleButton(FIRST_HALF);
            firstHalf.setSelected(true);
            secondHalf = new ToggleButton(SECOND_HALF);
            penaltyShoot = new ToggleButton(PENALTY_SHOOT);
            refereeTimeout = new ToggleButton(REFEREE_TIMEOUT);
            halfGroup = new ButtonGroup();
            halfGroup.add(firstHalf);
            halfGroup.add(secondHalf);
            halfGroup.add(penaltyShoot);
            
            if (Rules.league.isRefereeTimeoutAvailable) {
                halfGroup.add(refereeTimeout);
            }
        } else {
            firstHalf = new ToggleButton(FIRST_HALF_SHORT);
            firstHalf.setSelected(true);
            secondHalf = new ToggleButton(SECOND_HALF_SHORT);
            firstHalfOvertime = new ToggleButton(FIRST_HALF_OVERTIME);
            secondHalfOvertime = new ToggleButton(SECOND_HALF_OVERTIME);
            penaltyShoot = new ToggleButton(PENALTY_SHOOT_SHORT);
            refereeTimeout = new ToggleButton(REFEREE_TIMEOUT);
            halfGroup = new ButtonGroup();
            halfGroup.add(firstHalf);
            halfGroup.add(secondHalf);
            halfGroup.add(firstHalfOvertime);
            halfGroup.add(secondHalfOvertime);
            halfGroup.add(penaltyShoot);
            
            if (Rules.league.isRefereeTimeoutAvailable) {
                halfGroup.add(refereeTimeout);
            }
        }
        //  state
        initial = new ToggleButton(STATE_INITIAL);
        initial.setSelected(true);
        ready = new ToggleButton(STATE_READY);
        set = new ToggleButton(STATE_SET);
        play = new ToggleButton(STATE_PLAY);
        finish = new ToggleButton(STATE_FINISH);
        stateGroup = new ButtonGroup();
        stateGroup.add(initial);
        stateGroup.add(ready);
        stateGroup.add(set);
        stateGroup.add(play);
        stateGroup.add(finish);
        //  penalties
        if (Rules.league instanceof SPL) {
            pen = new JToggleButton[10];
            
            pen[0] = new ToggleButton(PEN_PUSHING);
            pen[1] = new ToggleButton(PEN_LEAVING);
            pen[2] = new ToggleButton(PEN_INACTIVE);
            pen[3] = new ToggleButton(PEN_DEFENDER);
            pen[4] = new ToggleButton(PEN_MOTION_IN_SET);
            pen[5] = new ToggleButton(PEN_KICK_OFF_GOAL);
            pen[6] = new ToggleButton(PEN_BALL_CONTACT);
            pen[7] = new ToggleButton(PEN_PICKUP);
            pen[8] = new ToggleButton(Rules.league.dropInPlayerMode ? TEAMMATE_PUSHING : PEN_COACH_MOTION);
            pen[9] = new ToggleButton(PEN_SUBSTITUTE);
        } else if (Rules.league instanceof HL) {
            pen = new JToggleButton[7];
            pen[0] = new ToggleButton(PEN_MANIPULATION);
            pen[1] = new ToggleButton(PEN_PHYSICAL);
            pen[2] = new ToggleButton(PEN_ATTACK);
            pen[3] = new ToggleButton(PEN_DEFENSE);
            pen[4] = new ToggleButton(PEN_PICKUP_INCAPABLE);
            pen[5] = new ToggleButton(PEN_SERVICE);
            pen[6] = new ToggleButton(PEN_SUBSTITUTE);
            dropBall = new Button(DROP_BALL);
        }
        //--bottom--
        //  log
        log = new JPanel();
        log.setOpaque(false);
        log.setLayout(new GridLayout(1, ActionBoard.MAX_NUM_UNDOS_AT_ONCE-1, 10, 0));
        undo = new JToggleButton[ActionBoard.MAX_NUM_UNDOS_AT_ONCE-1];
        for (int i=undo.length-1; i>=0; i--) {
            undo[i] = new ToggleButton();
            undo[i].setVisible(false);
            log.add(undo[i]);
        }
        cancelUndo = new Button(CANCEL);
        cancelUndo.setVisible(false);
      
        //--layout--
        TotalScaleLayout layout = new TotalScaleLayout(this);
        setLayout(layout);
        
        layout.add(0, 0, .3, .04, name[0]);
        layout.add(.7, 0, .3, .04, name[1]);
        layout.add(.01, .05, .08, .07, goalInc[0]);
        layout.add(.91, .05, .08, .07, goalInc[1]);
        layout.add(.01, .13, .08, .06, goalDec[0]);
        layout.add(.91, .13, .08, .06, goalDec[1]);
        layout.add(.17, .05, .12, .04, kickOff[0]);
        layout.add(.71, .05, .12, .04, kickOff[1]);
        layout.add(.21, .09, .08, .07, goals[0]);
        layout.add(.71, .09, .08, .07, goals[1]);
        layout.add(.21, .16, .08, .04, pushes[0]);
        layout.add(.71, .16, .08, .04, pushes[1]);
        layout.add(.01, .21, .28, .55, robots[0]);
        layout.add(.71, .21, .28, .55, robots[1]);
        if (Rules.league instanceof SPL && !Rules.league.dropInPlayerMode) {
            layout.add(.01, .77, .09, .09, timeOut[0]);
            layout.add(.9, .77, .09, .09, timeOut[1]);
            layout.add(.11, .77, .08, .09, stuck[0]);
            layout.add(.81, .77, .08, .09, stuck[1]);
            layout.add(.20, .77, .09, .09, out[0]);
            layout.add(.71, .77, .09, .09, out[1]);
        } else {
            if (Rules.league instanceof SPL) {
                layout.add(.01, .77, .135, .09, stuck[0]);
                layout.add(.855, .77, .135, .09, stuck[1]);
            } else {
                layout.add(.01, .77, .135, .09, timeOut[0]);
                layout.add(.855, .77, .135, .09, timeOut[1]);
            }
            layout.add(.155, .77, .135, .09, out[0]);
            layout.add(.71, .77, .135, .09, out[1]);
        }
        layout.add(.31, .0, .08, .11, clockReset);
        layout.add(.4, .012, .195, .10, clock);
        layout.add(.61, .0, .08, .11, clockPause);
        layout.add(.4, .11, .2, .07, clockSub);
        if (Rules.league.lostTime) {
            layout.add(.590, .0, .03, .11, incGameClock);
            layout.add(.4, .0, .195, .11, clockContainer);
        }
        else{
            layout.add(.4, .0, .2, .11, clockContainer);
        }
        if (!Rules.league.overtime) {
            if (Rules.league.isRefereeTimeoutAvailable && !Rules.league.dropInPlayerMode) {
                layout.add(.31, .19, .09, .06, firstHalf);
                layout.add(.407, .19, .09, .06, secondHalf);
                layout.add(.503, .19, .09, .06, penaltyShoot);
                layout.add(.60, .19, .09, .06, refereeTimeout);
            } else { // no referee timeout in dropInPlayerMode is not supported!
                layout.add(.31, .19, .12, .06, firstHalf);
                layout.add(.44, .19, .12, .06, secondHalf);
                layout.add(.57, .19, .12, .06, Rules.league.dropInPlayerMode ? refereeTimeout : penaltyShoot);
            }
        } else {
            if (Rules.league.isRefereeTimeoutAvailable) {
                layout.add(.31, .19, .06, .06, firstHalf);
                layout.add(.375, .19, .06, .06, secondHalf);
                layout.add(.439, .19, .06, .06, firstHalfOvertime);
                layout.add(.501, .19, .06, .06, secondHalfOvertime);
                layout.add(.565, .19, .06, .06, penaltyShoot);
                layout.add(.63, .19, .06, .06, refereeTimeout);
            } else {
                layout.add(.31, .19, .07, .06, firstHalf);
                layout.add(.3875, .19, .07, .06, secondHalf);
                layout.add(.465, .19, .07, .06, firstHalfOvertime);
                layout.add(.5425, .19, .07, .06, secondHalfOvertime);
                layout.add(.62, .19, .07, .06, penaltyShoot);
            }
        }
        layout.add(.31, .26, .07, .08, initial);
        layout.add(.3875, .26, .07, .08, ready);
        layout.add(.465, .26, .07, .08, set);
        layout.add(.5425, .26, .07, .08, play);
        layout.add(.62, .26, .07, .08, finish);
        if (Rules.league instanceof SPL) {
            layout.add(.31, .37, .185, .08, pen[0]);
            layout.add(.505, .37, .185, .08, pen[1]);
            layout.add(.31, .47, .185, .08, pen[2]);
            layout.add(.505, .47, .185, .08, pen[3]);
            layout.add(.31, .57, .185, .08, pen[4]);
            layout.add(.505, .57, .185, .08, pen[5]);
            layout.add(.31, .67, .185, .08, pen[6]);
            layout.add(.505, .67, .185, .08, pen[7]);
            layout.add(.31, .77, .185, .08, pen[8]);
            if (Rules.league.teamSize > Rules.league.robotsPlaying) {
                layout.add(.505, .77, .185, .08, pen[9]);
            }
        } else if (Rules.league instanceof HL) {
            layout.add(.31,  .38, .185, .08, pen[0]);
            layout.add(.505, .38, .185, .08, pen[1]);
            layout.add(.31,  .48, .185, .08, pen[2]);
            layout.add(.505, .48, .185, .08, pen[3]);
            layout.add(.31,  .58, .185, .08, pen[4]);
            layout.add(.505, .58, .185, .08, pen[5]);
            layout.add(.31,  .68, .185, .08, pen[6]);
            layout.add(.31,  .78, .38,  .08, dropBall);
        }
        layout.add(.08, .88, .84, .11, log);
        layout.add(.925, .88, .07, .11, cancelUndo);
        layout.add(0, 0, .3, .87, side[0]);
        layout.add(.3, 0, .4, .87, mid);
        layout.add(.7, 0, .3, .87, side[1]);
        layout.add(0, .87, 1, .132, bottom);
        
        //--listener--
        for (int i=0; i<2; i++) {
            goalDec[i].addActionListener(ActionBoard.goalDec[i]);
            goalInc[i].addActionListener(ActionBoard.goalInc[i]);
            kickOff[i].addActionListener(ActionBoard.kickOff[i]);
            for (int j=0; j<robot[i].length; j++) {
                robot[i][j].addActionListener(ActionBoard.robot[i][j]);
            }
            timeOut[i].addActionListener(ActionBoard.timeOut[i]);
            out[i].addActionListener(ActionBoard.out[i]);
            if (Rules.league instanceof SPL) {
                stuck[i].addActionListener(ActionBoard.stuck[i]);
            }
        }
        refereeTimeout.addActionListener(ActionBoard.refereeTimeout);
        initial.addActionListener(ActionBoard.initial);
        ready.addActionListener(ActionBoard.ready);
        set.addActionListener(ActionBoard.set);
        play.addActionListener(ActionBoard.play);
        finish.addActionListener(ActionBoard.finish);
        clockReset.addActionListener(ActionBoard.clockReset);
        clockPause.addActionListener(ActionBoard.clockPause);
        if (Rules.league.lostTime) {
            incGameClock.addActionListener(ActionBoard.incGameClock);
        }
        firstHalf.addActionListener(ActionBoard.firstHalf);
        secondHalf.addActionListener(ActionBoard.secondHalf);
        if (Rules.league.overtime) {
            firstHalfOvertime.addActionListener(ActionBoard.firstHalfOvertime);
            secondHalfOvertime.addActionListener(ActionBoard.secondHalfOvertime);
        }
        penaltyShoot.addActionListener(ActionBoard.penaltyShoot);
        if (Rules.league instanceof SPL) {
            pen[0].addActionListener(ActionBoard.pushing);
            pen[1].addActionListener(ActionBoard.leaving);
            pen[2].addActionListener(ActionBoard.inactive);
            pen[3].addActionListener(ActionBoard.defender);
            pen[4].addActionListener(ActionBoard.motionInSet);
            pen[5].addActionListener(ActionBoard.kickOffGoal);
            pen[6].addActionListener(ActionBoard.ballContact);
            pen[7].addActionListener(ActionBoard.pickUp);
            pen[8].addActionListener(Rules.league.dropInPlayerMode ? ActionBoard.teammatePushing : ActionBoard.coachMotion);
            pen[9].addActionListener(ActionBoard.substitute);
        } else if (Rules.league instanceof HL) {
            pen[0].addActionListener(ActionBoard.ballManipulation);
            pen[1].addActionListener(ActionBoard.pushing);
            pen[2].addActionListener(ActionBoard.attack);
            pen[3].addActionListener(ActionBoard.defense);
            pen[4].addActionListener(ActionBoard.pickUpHL);
            pen[5].addActionListener(ActionBoard.serviceHL);
            pen[6].addActionListener(ActionBoard.substitute);
            dropBall.addActionListener(ActionBoard.dropBall);
        }
        for (int i=0; i<undo.length; i++) {
            undo[i].addActionListener(ActionBoard.undo[i+1]);
        }
        cancelUndo.addActionListener(ActionBoard.cancelUndo);
      
        //fullscreen
        if (fullscreen) {
            setUndecorated(true);
            GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice().setFullScreenWindow(this);
            if (IS_APPLE_JAVA) {
                setVisible(false); // without this, keyboard input is missing on OS X
            }
        }
        
        setVisible(true);
    }
    
    /**
     * @author Michel Bartsch
     * 
     * This is a normal JPanel, but it has a background image.
     */
    class ImagePanel extends JPanel
    {
        private static final long serialVersionUID = 1L;
        
        
        /** The image that is shown in the background. */
        private Image image;

        /**
         * Creates a new ImagePanel.
         * 
         * @param image     The Image to be shown in the background.
         */
        public ImagePanel(Image image)
        {
            this.image = image;
        }
        
        /**
         * Changes the background image.
         * 
         * @param image     Changes the image to this one.
         */
        public void setImage(Image image)
        {
            this.image = image;
        }
        
        /**
         * Paints this Component, should be called automatically.
         * 
         * @param g     This components graphical content.
         */
        @Override
        public void paintComponent(Graphics g)
        {
            if (super.isOpaque()) {
                g.setColor(Color.WHITE);
                g.fillRect(0, 0, getWidth(), getHeight());
            }
            g.drawImage(image, 0, 0, getWidth(), getHeight(), null);
        }
    }
    
    /**
     * @author Michel Bartsch
     * 
     * This is a normal JButton, but it has a background image.
     */
    class ImageButton extends JButton
    {
        private static final long serialVersionUID = 1L;
        
        
        /** The image that is shown in the background. */
        private Image image;

        /**
         * Creates a new ImageButton.
         * 
         * @param image     The Image to be shown in the background.
         */
        public ImageButton(Image image)
        {
            this.image = image;
        }
        
        /**
         * Changes the background image.
         * 
         * @param image     Changes the image to this one.
         */
        public void setImage(Image image)
        {
            this.image = image;
        }
        
        /**
         * Paints this Component, should be called automatically.
         * 
         * @param g     This components graphical content.
         */
        @Override
        public void paintComponent(Graphics g)
        {
            if (super.isOpaque()) {
                g.clearRect(0, 0, getWidth(), getHeight());
            }
            g.drawImage(image, 0, 0, getWidth(), getHeight(), null);
        }
    }

    /**
     * This is called by the EventHandler after an action has been performed.
     * Here the GUI should update it`s view based on the data parameter.
     * There are three additional sources of information that can be used here:
     *  1. The RobotWatcher, you can ask him for the robots online-status.
     *  2. The last events from the EventHandler, but you should try to avoid
     *     this for less dependencies between actions and GUI (view and control).
     *  3. The actions isLegal method to enable or disable buttons.
     * This method should never have other effects than updating the view!
     * 
     * @param data     The current data (model) the GUI should view.
     */
    @Override
    public void update(AdvancedData data)
    {
        updateClock(data);
        updateHalf(data);
        updateColor(data);
        updateState(data);
        updateGoal(data);
        updateKickoff(data);
        updatePushes(data);
        updateTimeOut(data);
        updateRefereeTimeout(data);
        updateOut(data);
        
        if (Rules.league instanceof SPL) {
            updateGlobalStuck(data);
            updatePenaltiesSPL(data);
        } else if (Rules.league instanceof HL) {
            updatePenaltiesHL(data);
            updateDropBall(data);
        }
        updateRobots(data);
        updateUndo(data);
        repaint();
    }

    /**
     * Always update fonts before drawing.
     *
     * @param g     The graphics context to draw to.
     */
    @Override
    public void paint(Graphics g)
    {
        updateFonts();
        super.paint(g);
    }

    /**
     * Updates the clock.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updateClock(AdvancedData data)
    {
        clock.setText(formatTime(data.getRemainingGameTime(true)));
        Integer secondaryTime = data.getSecondaryTime(KICKOFF_BLOCKED_HIGHLIGHT_SECONDS - 1);
        if (secondaryTime != null) {
            if (data.gameState == GameControlData.STATE_PLAYING) {
                clockSub.setText(formatTime(Math.max(0, secondaryTime)));
                clockSub.setForeground(secondaryTime <= 0
                        && clockSub.getForeground() != COLOR_HIGHLIGHT ? COLOR_HIGHLIGHT : Color.BLACK);
            } else {
                clockSub.setText(formatTime(secondaryTime));
                clockSub.setForeground(Color.BLACK);
            }
        } else {
            clockSub.setText("");
            clockSub.setForeground(Color.BLACK);
        }
        
        ImageIcon tmp;
        if (ActionBoard.clock.isClockRunning(data)) {
            tmp = clockImgPause;
        } else {
            tmp = clockImgPlay;
        }
        clockPause.setImage(tmp.getImage());
        clockReset.setVisible(ActionBoard.clockReset.isLegal(data));
        clockPause.setVisible(ActionBoard.clockPause.isLegal(data));
        if (Rules.league.lostTime) {
            incGameClock.setEnabled(ActionBoard.incGameClock.isLegal(data));
        }
    }
    
    /**
     * Updates the half.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updateHalf(AdvancedData data)
    {
        for (int i=0; i<2; i++) {
            name[i].setText(Teams.getNames(false)[data.team[i].teamNumber]);
        }
        firstHalf.setEnabled(ActionBoard.firstHalf.isLegal(data));
        secondHalf.setEnabled(ActionBoard.secondHalf.isLegal(data));
        if (Rules.league.overtime) {
            firstHalfOvertime.setEnabled(ActionBoard.firstHalfOvertime.isLegal(data));
            secondHalfOvertime.setEnabled(ActionBoard.secondHalfOvertime.isLegal(data));
        }
        penaltyShoot.setEnabled(ActionBoard.penaltyShoot.isLegal(data));
        firstHalf.setSelected((data.secGameState == GameControlData.STATE2_NORMAL)
                            && (data.firstHalf == GameControlData.C_TRUE));
        secondHalf.setSelected((data.secGameState == GameControlData.STATE2_NORMAL)
                            && (data.firstHalf != GameControlData.C_TRUE));
        if (Rules.league.overtime) {
           firstHalfOvertime.setSelected((data.secGameState == GameControlData.STATE2_OVERTIME)
                            && (data.firstHalf == GameControlData.C_TRUE));
           secondHalfOvertime.setSelected((data.secGameState == GameControlData.STATE2_OVERTIME)
                            && (data.firstHalf != GameControlData.C_TRUE)); 
        }
        penaltyShoot.setSelected(data.secGameState == GameControlData.STATE2_PENALTYSHOOT || data.previousSecGameState == GameControlData.STATE2_PENALTYSHOOT);
    }
    
    /**
     * Updates left and right background picture.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updateColor(AdvancedData data)
    {
        for (int i=0; i<2; i++) {
            name[i].setForeground(Rules.league.teamColor[data.team[i].teamColor]);
            side[i].setImage(backgroundSide[i][data.team[i].teamColor].getImage());
        }
    }
    
    /**
     * Updates the state.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updateState(AdvancedData data)
    {
        initial.setEnabled(ActionBoard.initial.isLegal(data));
        ready.setEnabled(ActionBoard.ready.isLegal(data));
        set.setEnabled(ActionBoard.set.isLegal(data));
        play.setEnabled(ActionBoard.play.isLegal(data));
        finish.setEnabled(ActionBoard.finish.isLegal(data));
        switch (data.gameState) {
            case GameControlData.STATE_INITIAL:
                initial.setSelected(true);
                break;
            case GameControlData.STATE_READY:
                ready.setSelected(true);
                break;
            case GameControlData.STATE_SET:
                set.setSelected(true);
                break;
            case GameControlData.STATE_PLAYING:
                play.setSelected(true);
                break;
            case GameControlData.STATE_FINISHED:
                finish.setSelected(true);
                break;
        }
        highlight(finish, (data.gameState != GameControlData.STATE_FINISHED)
                && (data.getRemainingGameTime(true) <= FINISH_HIGHLIGHT_SECONDS)
                && (finish.getBackground() != COLOR_HIGHLIGHT));
    }
    
    /**
     * Updates the goal.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updateGoal(AdvancedData data)
    {
        for (int i=0; i<2; i++) {
            goals[i].setText(""+data.team[i].score);
            goalInc[i].setEnabled(ActionBoard.goalInc[i].isLegal(data));
            goalDec[i].setVisible(ActionBoard.goalDec[i].isLegal(data));
        }
    }
    
    /**
     * Updates the kickoff.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updateKickoff(AdvancedData data)
    {
        if (data.kickOffTeam == GameControlData.DROPBALL) {
            kickOff[2].setSelected(true);
        } else {
            kickOff[data.team[0].teamNumber == data.kickOffTeam ? 0 : 1].setSelected(true);
        }
        for (int i=0; i<2; i++) {
            kickOff[i].setEnabled(ActionBoard.kickOff[i].isLegal(data));
            if (data.secGameState != GameControlData.STATE2_PENALTYSHOOT
                && data.previousSecGameState != GameControlData.STATE2_PENALTYSHOOT) {
                kickOff[i].setText(KICKOFF);
            } else {
                kickOff[i].setText(KICKOFF_PENALTY_SHOOTOUT);
            }
        }
    }
    
    /**
     * Updates the pushes.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updatePushes(AdvancedData data)
    {
        for (int i=0; i<2; i++) {
            if (data.secGameState != GameControlData.STATE2_PENALTYSHOOT && data.previousSecGameState != GameControlData.STATE2_PENALTYSHOOT) {
                if (Rules.league.pushesToEjection == null || Rules.league.pushesToEjection.length == 0) {
                    pushes[i].setText("");
                } else {
                    pushes[i].setText(PUSHES+": "+data.pushes[i]);
                }
            } else {
                pushes[i].setText((i == 0 && (data.gameState == GameControlData.STATE_SET
                        || data.gameState == GameControlData.STATE_PLAYING) ? SHOT : SHOTS)+": "+data.team[i].penaltyShot);
            }
        }
    }
    
    /**
     * Updates the robots.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updateRobots(AdvancedData data)
    {
        RobotOnlineStatus[][] onlineStatus = RobotWatcher.updateRobotOnlineStatus();
        for (int i=0; i<robot.length; i++) {
            for (int j=0; j<robot[i].length; j++) {
                if (ActionBoard.robot[i][j].isCoach(data)) {
                   if (data.team[i].coach.penalty == PlayerInfo.PENALTY_SPL_COACH_MOTION) {
                      robot[i][j].setEnabled(false);
                      robotLabel[i][j].setText(EJECTED);
                  } else {
                      robotLabel[i][j].setText(Rules.league.teamColorName[data.team[i].teamColor]+" "+COACH);
                  }
                }
                else {
                    if (data.team[i].player[j].penalty != PlayerInfo.PENALTY_NONE) {
                        if (!data.ejected[i][j]) {
                            int seconds = data.getRemainingPenaltyTime(i, j);
                            boolean pickup = ((Rules.league instanceof SPL &&
                                        data.team[i].player[j].penalty == PlayerInfo.PENALTY_SPL_REQUEST_FOR_PICKUP)
                                   || (Rules.league instanceof HL &&
                                       ( data.team[i].player[j].penalty == PlayerInfo.PENALTY_HL_PICKUP_OR_INCAPABLE
                                      || data.team[i].player[j].penalty == PlayerInfo.PENALTY_HL_SERVICE ))
                                    );
                            boolean illegalMotion = Rules.league instanceof SPL
                                    && data.team[i].player[j].penalty == PlayerInfo.PENALTY_SPL_ILLEGAL_MOTION_IN_SET;
                            if (seconds == 0) {
                                if (pickup) {
                                    robotLabel[i][j].setText(Rules.league.teamColorName[data.team[i].teamColor]+" "+(j+1)+" ("+PEN_PICKUP+")");
                                    highlight(robot[i][j], true);
                                } else if (illegalMotion) {
                                    robotLabel[i][j].setText(Rules.league.teamColorName[data.team[i].teamColor]+" "+(j+1)+" ("+PEN_MOTION_IN_SET_SHORT+")");
                                    highlight(robot[i][j], true);
                                } else if (data.team[i].player[j].penalty == PlayerInfo.PENALTY_SUBSTITUTE) {
                                    robotLabel[i][j].setText(Rules.league.teamColorName[data.team[i].teamColor]+" "+(j+1)+" ("+PEN_SUBSTITUTE_SHORT+")");
                                    highlight(robot[i][j], false);
                                } else if (!(Rules.league instanceof SPL) ||
                                        !(data.team[i].player[j].penalty == PlayerInfo.PENALTY_SPL_COACH_MOTION)) {
                                    robotLabel[i][j].setText(Rules.league.teamColorName[data.team[i].teamColor]+" "+(j+1)+": "+formatTime(seconds));
                                    highlight(robot[i][j], seconds <= UNPEN_HIGHLIGHT_SECONDS && robot[i][j].getBackground() != COLOR_HIGHLIGHT);
                                }
                            }  else {
                                robotLabel[i][j].setText(Rules.league.teamColorName[data.team[i].teamColor]+" "+(j+1)+": "+formatTime(seconds)+(pickup ? " (P)" : ""));
                                highlight(robot[i][j], seconds <= UNPEN_HIGHLIGHT_SECONDS && robot[i][j].getBackground() != COLOR_HIGHLIGHT);
                            }
                            int penTime = (seconds + data.getSecondsSince(data.whenPenalized[i][j]));
                            if (seconds != 0) {
                                robotTime[i][j].setValue(1000 * seconds / penTime);
                            }
                            robotTime[i][j].setVisible(seconds != 0);
                        } else {
                            robotLabel[i][j].setText(EJECTED);
                            robotTime[i][j].setVisible(false);
                            highlight(robot[i][j], false);
                        }
                    } else {
                        robotLabel[i][j].setText(Rules.league.teamColorName[data.team[i].teamColor]+" "+(j+1));
                        robotTime[i][j].setVisible(false);
                        highlight(robot[i][j], false);
                    }
                }    
                
                robot[i][j].setEnabled(ActionBoard.robot[i][j].isLegal(data));
                
                ImageIcon currentLanIcon;
                if (onlineStatus[i][j] == RobotOnlineStatus.ONLINE) {
                    currentLanIcon = lanOnline;
                } else if (onlineStatus[i][j] == RobotOnlineStatus.HIGH_LATENCY) {
                    currentLanIcon = lanHighLatency;
                } else if (onlineStatus[i][j] == RobotOnlineStatus.OFFLINE) {
                    currentLanIcon = lanOffline;
                } else {
                    currentLanIcon = lanUnknown;
                }
                robotLabel[i][j].setIcon(currentLanIcon);
            }
        }
    }
    
    /**
     * Updates the time-out.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updateTimeOut(AdvancedData data)
    {
        for (int i=0; i<2; i++) {
            if (!data.timeOutActive[i]) {
                timeOut[i].setSelected(false);
                highlight(timeOut[i], false);
            } else {
                boolean shouldHighlight = (data.getRemainingSeconds(data.whenCurrentGameStateBegan, Rules.league.timeOutTime) < TIMEOUT_HIGHLIGHT_SECONDS)
                        && (timeOut[i].getBackground() != COLOR_HIGHLIGHT);
                timeOut[i].setSelected(!IS_OSX || !shouldHighlight);
                highlight(timeOut[i], shouldHighlight);
            }
            timeOut[i].setEnabled(ActionBoard.timeOut[i].isLegal(data));
        }
    }
    
    private void updateRefereeTimeout(AdvancedData data) {
        refereeTimeout.setSelected(data.refereeTimeout);
        refereeTimeout.setEnabled(ActionBoard.refereeTimeout.isLegal(data));
    }
    
    /**
     * Updates the global game stuck.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updateGlobalStuck(AdvancedData data)
    {
        for (int i=0; i<2; i++) {
            if (data.gameState == GameControlData.STATE_PLAYING
                    && data.getRemainingSeconds(data.whenCurrentGameStateBegan, Rules.league.kickoffTime + Rules.league.minDurationBeforeStuck) > 0)
            {
                stuck[i].setEnabled(false);
                stuck[i].setText("<font color=#808080>"+STUCK);
            } else {
                stuck[i].setEnabled(ActionBoard.stuck[i].isLegal(data));
                stuck[i].setText((ActionBoard.stuck[i].isLegal(data) ? "<font color=#000000>" : "<font color=#808080>")+STUCK);
            }
        }
    }
    
    /**
     * Updates the dropped-ball button.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updateDropBall(AdvancedData data)
    {
        dropBall.setEnabled(ActionBoard.dropBall.isLegal(data));
    }
    
    /**
     * Updates the out.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updateOut(AdvancedData data)
    {
        for (int i=0; i<2; i++) {
            out[i].setEnabled(ActionBoard.out[i].isLegal(data));
        }
    }

    /**
     * Updates the SPL penalties.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updatePenaltiesSPL(AdvancedData data)
    {
        pen[0].setEnabled(ActionBoard.pushing.isLegal(data));
        pen[1].setEnabled(ActionBoard.leaving.isLegal(data));
        pen[2].setEnabled(ActionBoard.inactive.isLegal(data));
        pen[2].setText("<html><center>"
                +(ActionBoard.inactive.isLegal(data) ? "<font color=#000000>" : "<font color=#808080>")
                +PEN_INACTIVE);
        pen[3].setEnabled(ActionBoard.defender.isLegal(data));
        pen[4].setEnabled(ActionBoard.motionInSet.isLegal(data));
        pen[5].setEnabled(ActionBoard.kickOffGoal.isLegal(data));
        pen[6].setEnabled(ActionBoard.ballContact.isLegal(data));
        pen[7].setEnabled(ActionBoard.pickUp.isLegal(data));
        pen[8].setEnabled(Rules.league.dropInPlayerMode ? ActionBoard.teammatePushing.isLegal(data)
                : ActionBoard.coachMotion.isLegal(data));
        pen[9].setEnabled(ActionBoard.substitute.isLegal(data));
        
        GCAction hightlightEvent = EventHandler.getInstance().lastUIEvent;
        pen[0].setSelected(hightlightEvent == ActionBoard.pushing);
        pen[1].setSelected(hightlightEvent == ActionBoard.leaving);
        pen[2].setSelected(hightlightEvent == ActionBoard.inactive);
        pen[3].setSelected(hightlightEvent == ActionBoard.defender);
        pen[5].setSelected(hightlightEvent == ActionBoard.kickOffGoal);
        pen[6].setSelected(hightlightEvent == ActionBoard.ballContact);
        pen[7].setSelected(hightlightEvent == ActionBoard.pickUp);
        pen[8].setSelected(Rules.league.dropInPlayerMode ? hightlightEvent == ActionBoard.teammatePushing
                : hightlightEvent == ActionBoard.coachMotion);
        pen[9].setSelected(hightlightEvent == ActionBoard.substitute);

        // Handle quick select for ILLEGAL_MOTION_IN_SET
        if (pen[4].isEnabled()) {
            boolean otherButtonSelected = false;
            for (JToggleButton button : pen) {
                otherButtonSelected |= button != pen[4] && button.isSelected();
            }
            for (JToggleButton button : undo) {
                otherButtonSelected |= button.isSelected();
            }
            pen[4].setSelected(!otherButtonSelected);
            if (!otherButtonSelected) {
                EventHandler.getInstance().lastUIEvent = ActionBoard.motionInSet;
            }
        } else {
            pen[4].setSelected(EventHandler.getInstance().lastUIEvent == ActionBoard.motionInSet);
        }
    }
    
    /**
     * Updates the HL penalties.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updatePenaltiesHL(AdvancedData data)
    {
        pen[0].setEnabled(ActionBoard.ballManipulation.isLegal(data));
        pen[1].setEnabled(ActionBoard.pushing.isLegal(data));
        pen[2].setEnabled(ActionBoard.attack.isLegal(data));
        pen[3].setEnabled(ActionBoard.defense.isLegal(data));
        pen[4].setEnabled(ActionBoard.pickUpHL.isLegal(data));
        pen[5].setEnabled(ActionBoard.serviceHL.isLegal(data));
        pen[6].setEnabled(ActionBoard.substitute.isLegal(data));

        GCAction hightlightEvent = EventHandler.getInstance().lastUIEvent;
        pen[0].setSelected(hightlightEvent == ActionBoard.ballManipulation);
        pen[1].setSelected(hightlightEvent == ActionBoard.pushing);
        pen[2].setSelected(hightlightEvent == ActionBoard.attack);
        pen[3].setSelected(hightlightEvent == ActionBoard.defense);
        pen[4].setSelected(hightlightEvent == ActionBoard.pickUpHL);
        pen[5].setSelected(hightlightEvent == ActionBoard.serviceHL);
        pen[6].setSelected(hightlightEvent == ActionBoard.substitute);
    }
    
    /**
     * Updates the timeline/undo.
     * 
     * @param data     The current data (model) the GUI should view.
     */
    private void updateUndo(AdvancedData data)
    {
        GCAction highlightEvent = EventHandler.getInstance().lastUIEvent;
        String[] undos = Log.getLast(ActionBoard.MAX_NUM_UNDOS_AT_ONCE);
        boolean undoFromHere = false;
        for (int i=undo.length - 1; i >= 0; i--) {
            undo[i].setVisible(!undos[i].equals(""));
            undo[i].setEnabled(!undos[i].contains(" vs "));
            if ((highlightEvent == ActionBoard.undo[i+1]) && (!ActionBoard.undo[i+1].executed)) {
                undoFromHere = true;
            }
            if (undoFromHere) {
                undo[i].setText("<html><center>Undo '"+undos[i] + "\'?");
                undo[i].setSelected(true);
            } else {
                undo[i].setText("<html><center>"+undos[i]);
                undo[i].setSelected(false);
            }
        }
        cancelUndo.setVisible(undoFromHere);
    }
    
    private void updateFonts()
    {
        double size = Math.min((getWidth()/(double)WINDOW_WIDTH), (getHeight()/(double)WINDOW_HEIGHT));
        
        if (size == lastSize) {
            return;
        }
        lastSize = size;
        
        titleFont = new Font(STANDARD_FONT, Font.PLAIN, (int)(TITLE_FONT_SIZE*(size)));
        standardFont = new Font(STANDARD_FONT, Font.PLAIN, (int)(STANDARD_FONT_SIZE*(size)));
        goalsFont = new Font(STANDARD_FONT, Font.PLAIN, (int)(GOALS_FONT_SIZE*(size)));
        timeFont = new Font(STANDARD_FONT, Font.PLAIN, (int)(TIME_FONT_SIZE*(size)));
        timeSubFont = new Font(STANDARD_FONT, Font.PLAIN, (int)(TIME_SUB_FONT_SIZE*(size)));
        timeoutFont = new Font(STANDARD_FONT, Font.PLAIN, (int)(TIMEOUT_FONT_SIZE*(size)));
        stateFont = new Font(STANDARD_FONT, Font.PLAIN, (int)(STATE_FONT_SIZE*(size)));

        for (int i=0; i<=1; i++) {
            name[i].setFont(titleFont);
            goalInc[i].setFont(standardFont);
            goalDec[i].setFont(standardFont);
            kickOff[i].setFont(standardFont);
            goals[i].setFont(goalsFont);
            pushes[i].setFont(standardFont);
            for (int j=0; j<robot[i].length; j++) {
                robotLabel[i][j].setFont(titleFont);
            }
            timeOut[i].setFont(timeoutFont);
            out[i].setFont(timeoutFont);
            if (Rules.league instanceof SPL) {
                stuck[i].setFont(timeoutFont);
            }
        }
        clock.setFont(timeFont);
        clockSub.setFont(timeSubFont);
        
        firstHalf.setFont(timeoutFont);
        secondHalf.setFont(timeoutFont);
        if (Rules.league.overtime) {
            firstHalfOvertime.setFont(timeoutFont);
            secondHalfOvertime.setFont(timeoutFont);
        }
        penaltyShoot.setFont(timeoutFont);
        if (Rules.league.isRefereeTimeoutAvailable) {
            refereeTimeout.setFont(timeoutFont);
        }

        initial.setFont(stateFont);
        ready.setFont(stateFont);
        set.setFont(stateFont);
        play.setFont(stateFont);
        finish.setFont(stateFont);
        for (int i=0; i<pen.length; i++) {
            pen[i].setFont(standardFont);
        }
        if (dropBall != null) {
            dropBall.setFont(standardFont);
        }
        for (int i=0; i<undo.length; i++) {
            undo[i].setFont(timeoutFont);
        }
        cancelUndo.setFont(standardFont);
    }
    
    /**
     * Set the given button highlighted or normal.
     * 
     * @param button        The button to highlight.
     * @param highlight     If the button should be highlighted.
     */
    private void highlight(AbstractButton button, boolean highlight)
    {
        button.setBackground(highlight ? COLOR_HIGHLIGHT : COLOR_STANDARD);
        if (IS_OSX) {
            button.setOpaque(highlight);
            button.setBorderPainted(!highlight);
        }
    }

    private String formatTime(int seconds) {
        int displaySeconds = Math.abs(seconds) % 60;
        int displayMinutes = Math.abs(seconds) / 60;
        return (seconds < 0 ? "-" : "") + String.format("%02d:%02d", displayMinutes, displaySeconds);
    }
}