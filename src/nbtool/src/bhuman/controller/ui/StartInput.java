package controller.ui;

import data.GameControlData;
import data.Rules;
import data.SPL;
import data.SPLDropIn;
import data.Teams;

import java.awt.BorderLayout;
import java.awt.Checkbox;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Graphics;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.util.Arrays;
import java.util.HashMap;
import java.io.Serializable;

import javax.swing.*;


/**
 * @author Michel Bartsch
 * 
 * This is only to be on starting the programm to get starting input.
 */
public class StartInput extends JFrame implements Serializable
{
    private static final long serialVersionUID = 1L;

    /**
     * Some constants defining this GUI`s appearance as their names say.
     * Feel free to change them and see what happens.
     */
    private static final String WINDOW_TITLE = "GameController";
    private static final int WINDOW_WIDTH = 600;
    private static final int WINDOW_HEIGHT = 450;
    private static final int STANDARD_SPACE = 10;
    private static final int TEAMS_HEIGHT = 300;
    private static final int IMAGE_SIZE = 250;
    private static final int OPTIONS_CONTAINER_HEIGHT = 80;
    private static final int OPTIONS_HEIGHT = 22;
    private static final int START_HEIGHT = 30;
    /** This is not what the name says ;) */
    private static final int FULLSCREEN_WIDTH = 160;
    private static final String ICONS_PATH = "config/icons/";
    private static final String[] BACKGROUND_PREFIX = {"robot_left_", "robot_right_"};
    private static final String BACKGROUND_EXT = ".png";
    private static final String FULLTIME_LABEL_NO = "Preliminaries Game";
    private static final String FULLTIME_LABEL_YES = "Play-off Game";
    private static final String FULLTIME_LABEL_HL_NO = "Normal Game";
    private static final String FULLTIME_LABEL_HL_YES = "Knock-Out Game";
    private static final String FULLSCREEN_LABEL = "Fullscreen";
    private static final String COLOR_CHANGE_LABEL = "Auto color change";
    private static final String START_LABEL = "Start";
    
    /** If true, this GUI has finished and offers it`s input. */
    public boolean finished = false;

    /** The inputs that can be read from this GUI when it has finished. */
    public int[] outTeam = {0, 0};
    public byte[] outTeamColor = new byte[2];
    public boolean outFulltime;
    public boolean outFullscreen;
    public boolean outAutoColorChange;

    /** All the components of this GUI. */
    private ImagePanel[] teamContainer = new ImagePanel[2];
    private ImageIcon[] teamIcon = new ImageIcon[2];
    private JLabel[] teamIconLabel = new JLabel[2];
    @SuppressWarnings("unchecked")
    private JComboBox<String>[] team = (JComboBox<String>[]) new JComboBox[2];
    private JPanel optionsLeft;
    private JPanel optionsRight;
    private JComboBox<String> league;
    private JRadioButton nofulltime;
    private JRadioButton fulltime;
    private ButtonGroup fulltimeGroup;
    private Checkbox fullscreen;
    private Checkbox autoColorChange;
    private JButton start;
    
    private HashMap<String, Image> images = new HashMap<String, Image>();

    /**
     * Creates a new StartInput.
     * @param args The parameters that the jar file was started with.
     */
    public StartInput(boolean fullscreenMode)
    {
        super(WINDOW_TITLE);

        GraphicsDevice gd = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();
        int width = gd.getDisplayMode().getWidth();
        int height = gd.getDisplayMode().getHeight();
        setLocation((width-WINDOW_WIDTH)/2, (height-WINDOW_HEIGHT)/2);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(false);
        setLayout(new FlowLayout(FlowLayout.CENTER, 0, STANDARD_SPACE));
        
        String[] teams = getShortTeams();
        for (int i=0; i<2; i++) {
            teamContainer[i] = new ImagePanel(getImage(i, i == 0 ? "blue" : "red"));
            teamContainer[i].setPreferredSize(new Dimension(WINDOW_WIDTH/2-STANDARD_SPACE, TEAMS_HEIGHT));
            teamContainer[i].setOpaque(true);
            teamContainer[i].setLayout(new BorderLayout());
            add(teamContainer[i]);
            setTeamIcon(i, 0);
            teamIconLabel[i] = new JLabel(teamIcon[i]);
            teamContainer[i].add(teamIconLabel[i], BorderLayout.CENTER);
            team[i] = new JComboBox<String>(teams);
            teamContainer[i].add(team[i], BorderLayout.SOUTH);
        }
        team[0].addActionListener(new ActionListener()
            {
            @Override
                public void actionPerformed(ActionEvent e)
                {
                    Object selected = team[0].getSelectedItem();
                    if (selected == null) {
                        return;
                    }
                    outTeam[0] = Integer.valueOf(((String)selected).split(" \\(")[1].split("\\)")[0]);
                    updateBackgrounds();
                    setTeamIcon(0, outTeam[0]);
                    teamIconLabel[0].setIcon(teamIcon[0]);
                    teamIconLabel[0].repaint();
                    teamIconLabel[1].repaint();
                    startEnabling();
                }
            }
        );
        team[1].addActionListener(new ActionListener()
            {
            @Override
                public void actionPerformed(ActionEvent e)
                {
                    Object selected = team[1].getSelectedItem();
                    if (selected == null){
                        return;
                    }
                    outTeam[1] = Integer.valueOf(((String)selected).split(" \\(")[1].split("\\)")[0]);
                    updateBackgrounds();
                    setTeamIcon(1, outTeam[1]);
                    teamIconLabel[1].setIcon(teamIcon[1]);
                    teamIconLabel[0].repaint();
                    teamIconLabel[1].repaint();
                    startEnabling();
                }
            }
        );

        optionsLeft = new JPanel();
        optionsLeft.setPreferredSize(new Dimension(WINDOW_WIDTH/2-2*STANDARD_SPACE, OPTIONS_CONTAINER_HEIGHT));
        optionsLeft.setLayout(new FlowLayout(FlowLayout.CENTER));
        add(optionsLeft);

        JPanel fullscreenPanel = new JPanel();
        fullscreenPanel.setLayout(new FlowLayout(FlowLayout.LEFT));
        optionsLeft.add(fullscreenPanel);
        JPanel autoColorChangePanel = new JPanel();
        autoColorChangePanel.setLayout(new FlowLayout(FlowLayout.LEFT));
        optionsLeft.add(autoColorChangePanel);

        fullscreen = new Checkbox(FULLSCREEN_LABEL);
        fullscreen.setPreferredSize(new Dimension(FULLSCREEN_WIDTH, OPTIONS_HEIGHT));
        fullscreen.setState(fullscreenMode);
        fullscreenPanel.add(fullscreen);
        
        autoColorChange = new Checkbox(COLOR_CHANGE_LABEL);
        autoColorChange.setPreferredSize(new Dimension(FULLSCREEN_WIDTH, OPTIONS_HEIGHT));
        autoColorChange.setState(Rules.league.colorChangeAuto);
        autoColorChangePanel.add(autoColorChange);
        autoColorChange.setState(Rules.league.colorChangeAuto);

        optionsRight = new JPanel();
        optionsRight.setPreferredSize(new Dimension(WINDOW_WIDTH/2-2*STANDARD_SPACE, OPTIONS_CONTAINER_HEIGHT));
        add(optionsRight);
        Dimension optionsDim = new Dimension(WINDOW_WIDTH/3-2*STANDARD_SPACE, OPTIONS_HEIGHT);
        league = new JComboBox<String>();
        for (int i=0; i < Rules.LEAGUES.length; i++) {
            league.addItem(Rules.LEAGUES[i].leagueName);
            if (Rules.LEAGUES[i] == Rules.league) {
                league.setSelectedIndex(i);
            }
        }
        league.setPreferredSize(optionsDim);
        league.addActionListener(new ActionListener()
            {
            @Override
                public void actionPerformed(ActionEvent e)
                {
                    if (e != null) { // not initial setup
                        for (int i=0; i < Rules.LEAGUES.length; i++) {
                            if (Rules.LEAGUES[i].leagueName.equals((String)league.getSelectedItem())) {
                                Rules.league = Rules.LEAGUES[i];
                                break;
                            }
                        }
                    }
                    if (Rules.league instanceof SPLDropIn) {
                        nofulltime.setVisible(false);
                        fulltime.setVisible(false);
                        autoColorChange.setVisible(false);
                    } else {
                        nofulltime.setVisible(true);
                        fulltime.setVisible(true);
                        if (Rules.league instanceof SPL) {
                            nofulltime.setText(FULLTIME_LABEL_NO);
                            fulltime.setText(FULLTIME_LABEL_YES);
                            autoColorChange.setVisible(false);
                        } else {
                            nofulltime.setText(FULLTIME_LABEL_HL_NO);
                            fulltime.setText(FULLTIME_LABEL_HL_YES);
                            autoColorChange.setState(Rules.league.colorChangeAuto);
                            autoColorChange.setVisible(true);
                        }
                    }
                    showAvailableTeams();
                    startEnabling();
                }
            }
        );
        optionsRight.add(league);
        nofulltime = new JRadioButton();
        nofulltime.setPreferredSize(optionsDim);
        fulltime = new JRadioButton();
        fulltime.setPreferredSize(optionsDim);
        fulltimeGroup = new ButtonGroup();
        fulltimeGroup.add(nofulltime);
        fulltimeGroup.add(fulltime);
        optionsRight.add(nofulltime);
        optionsRight.add(fulltime);
        nofulltime.addActionListener(new ActionListener() {
            @Override
                public void actionPerformed(ActionEvent e) {
                    startEnabling();
                }});
        fulltime.addActionListener(new ActionListener() {
            @Override
                public void actionPerformed(ActionEvent e) {
                    startEnabling();
                }});
        start = new JButton(START_LABEL);
        start.setPreferredSize(new Dimension(WINDOW_WIDTH/3-2*STANDARD_SPACE, START_HEIGHT));
        start.setEnabled(false);
        add(start);
        start.addActionListener(new ActionListener() {
            @Override
                public void actionPerformed(ActionEvent e) {
                    outFulltime = fulltime.isSelected() && fulltime.isVisible();
                    outFullscreen = fullscreen.getState();
                    outAutoColorChange = autoColorChange.getState();
                    finished = true;
                }});
                
        league.getActionListeners()[league.getActionListeners().length - 1].actionPerformed(null);

        getContentPane().setPreferredSize(new Dimension(WINDOW_WIDTH, WINDOW_HEIGHT));
        pack();
        setVisible(true);
    }
    /** Show in the combo box which teams are available for the selected league and competition*/
    private void showAvailableTeams() 
    {
        outTeam[0] = 0;
        outTeam[1] = 0;
        for (int i=0; i < 2; i++) {
            team[i].removeAllItems();
            String[] names = getShortTeams();
            if (Rules.league.dropInPlayerMode) {
                team[i].addItem(names[0]);
                team[i].addItem(names[i == 0 ?  1 : 2]);
            } else {
                for (int j=0; j < names.length; j++) {
                    team[i].addItem(names[j]);
                }
            }
            setTeamIcon(i, outTeam[i]);
            teamIconLabel[i].setIcon(teamIcon[i]);
            teamIconLabel[i].repaint();
        }
    }
    
    /**
     * Calculates an array that contains only the existing Teams of the
     * current league.
     * 
     * @return  Short teams array with numbers
     */ 
    private String[] getShortTeams()
    {
        String[] fullTeams = Teams.getNames(true);
        String[] out;
        int k = 0;
        for (int j=0; j<fullTeams.length; j++) {
            if (fullTeams[j] != null) {
                k++;
            }
        }
        out = new String[k];
        k = 0;
        for (int j=0; j<fullTeams.length; j++) {
            if (fullTeams[j] != null) {
                out[k++] = fullTeams[j];
            }
        }

        Arrays.sort(out, 1, out.length, String.CASE_INSENSITIVE_ORDER);

        return out;
    }
    
    /**
     * Sets the Team-Icon on the GUI.
     * 
     * @param side      The side (0=left, 1=right)
     * @param team      The number of the Team
     */ 
    private void setTeamIcon(int side, int team)
    {
        teamIcon[side] = new ImageIcon(Teams.getIcon(team));
        float scaleFactor;
        if (teamIcon[side].getImage().getWidth(null) > teamIcon[side].getImage().getHeight(null)) {
            scaleFactor = (float)IMAGE_SIZE/teamIcon[side].getImage().getWidth(null);
        } else {
            scaleFactor = (float)IMAGE_SIZE/teamIcon[side].getImage().getHeight(null);
        }

        // getScaledInstance/SCALE_SMOOTH does not work with all color models, so we need to convert image
        BufferedImage image = (BufferedImage) teamIcon[side].getImage();
        if (image.getType() != BufferedImage.TYPE_INT_ARGB) {
            BufferedImage temp = new BufferedImage(image.getWidth(), image.getHeight(), BufferedImage.TYPE_INT_ARGB);
            Graphics g = temp.createGraphics();
            g.drawImage(image, 0, 0, null);
            g.dispose();
            image = temp;
        }

        teamIcon[side].setImage(image.getScaledInstance(
                (int)(teamIcon[side].getImage().getWidth(null)*scaleFactor),
                (int)(teamIcon[side].getImage().getHeight(null)*scaleFactor),
                Image.SCALE_SMOOTH));
    }
    
    /**
     * Enables the start button, if the conditions are ok.
     */
    private void startEnabling()
    {
        start.setEnabled(outTeam[0] != outTeam[1] &&
                (fulltime.isSelected() || nofulltime.isSelected() || !fulltime.isVisible()));
    }
    
    private Image getImage(int side, String color)
    {
        String filename = ICONS_PATH + Rules.league.leagueDirectory + "/" + BACKGROUND_PREFIX[side] + color + BACKGROUND_EXT;
        if (images.get(filename) == null) {
            images.put(filename, new ImageIcon(filename).getImage());
        }
        return images.get(filename);
    }
    
    private void updateBackgrounds()
    {
        String[] colorNames = new String[2];
        if (Teams.getColors(outTeam[0]).length == 0 &&
                Teams.getColors(outTeam[1]).length > 0) {
            colorNames[1] = Teams.getColors(outTeam[1])[0];
            colorNames[0] = colorNames[1].equals("blue") ? "red" : "blue";
        } else {
            String[] colors = Teams.getColors(outTeam[0]);
            colorNames[0] = colors.length > 0 ? colors[0] : "blue";
            colors = Teams.getColors(outTeam[1]);
            colors = colors.length >= 2 ? colors : colors.length == 1 
                    ? new String[] {colors[0], "red"} : new String[]{"red", "blue"};
            colorNames[1] = colors[0].equals(colorNames[0]) ? colors[1] : colors[0];
        }
        teamContainer[0].setImage(getImage(0, colorNames[0]));
        teamContainer[1].setImage(getImage(1, colorNames[1]));
        for (int i = 0; i < 2; ++i) {
            if (colorNames[i].equals("blue")) {
                outTeamColor[i] = GameControlData.TEAM_BLUE;
            } else if (colorNames[i].equals("red")) {
                outTeamColor[i] = GameControlData.TEAM_RED;
            } else if (colorNames[i].equals("yellow")) {
                outTeamColor[i] = GameControlData.TEAM_YELLOW;
            } else {
                outTeamColor[i] = GameControlData.TEAM_BLACK;
            }
        }
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
            g.drawImage(image, (getWidth()-image.getWidth(null))/2, 0, image.getWidth(null), image.getHeight(null), null);
        }
    }
}
