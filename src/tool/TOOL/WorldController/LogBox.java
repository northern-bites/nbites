package TOOL.WorldController;

//import WorldController;

import javax.swing.*;
import javax.swing.event.*;
import java.awt.event.*;
import java.awt.*;
import java.awt.GridLayout;

public class LogBox extends JFrame implements ActionListener, ChangeListener
{

    // button strings
    public static final String PLAY_STRING = "Play";
    public static final String PAUSE_STRING = "Pause";
    public static final String FRAME_STRING = "Frame:";
    public static final String OUT_OF_STRING = "of";
    public static final String LAST_FRAME_STRING = "Last Frame";
    public static final String NEXT_FRAME_STRING = "Next Frame";
    public static final String START_FRAME_STRING = "Start Frame";
    public static final String END_FRAME_STRING = "End Frame";
    // button action strings
    public static final String PLAY_PAUSE_ACTION = "playpause";
    public static final String LAST_FRAME_ACTION = "lastframe";
    public static final String NEXT_FRAME_ACTION = "nextframe";
    public static final String START_FRAME_ACTION = "startframe";
    public static final String END_FRAME_ACTION = "endframe";
    public static final String SLIDE_ACTION = "slideframe";

    // variable declarations
    public JButton play_pause, last_frame, next_frame;
    public JButton start_frame, end_frame;
    private JLabel frameLabel1, frameLabel2;
    public JLabel frameNumber, frameTotal;
    public JPanel buttons, labels;
    public JSlider slide;
    public JSlider fps_slide;
    private LogHandler log;
    private WorldControllerPainter painter;

    public LogBox(LogHandler _log, WorldControllerPainter _painter, int x,
                  int y)
    {
        super("Log Box");
        // store WorldControll instance locally
        log = _log;
        painter = _painter;

        JFrame.setDefaultLookAndFeelDecorated(true);
        // set layout
        setLayout(new BoxLayout(this.getContentPane(), BoxLayout.PAGE_AXIS));

        // init the buttons
        play_pause = new JButton(PLAY_STRING);
        play_pause.setActionCommand(PLAY_PAUSE_ACTION);
        play_pause.addActionListener(this);

        last_frame = new JButton(LAST_FRAME_STRING);
        last_frame.setActionCommand(LAST_FRAME_ACTION);
        last_frame.addActionListener(this);

        next_frame = new JButton(NEXT_FRAME_STRING);
        next_frame.setActionCommand(NEXT_FRAME_ACTION);
        next_frame.addActionListener(this);

        start_frame = new JButton(START_FRAME_STRING);
        start_frame.setActionCommand(START_FRAME_ACTION);
        start_frame.addActionListener(this);

        end_frame = new JButton(END_FRAME_STRING);
        end_frame.setActionCommand(END_FRAME_ACTION);
        end_frame.addActionListener(this);

        // init the labels
        frameLabel1 = new JLabel(FRAME_STRING, JLabel.CENTER);
        frameLabel2 = new JLabel(OUT_OF_STRING, JLabel.CENTER);
        frameNumber = new JLabel("1", JLabel.CENTER);
        frameTotal = new JLabel("1", JLabel.CENTER);

        slide = new JSlider(JSlider.HORIZONTAL, 1, 2, 1);
        slide.addChangeListener(this);

        //slide.setMajorTickSpacing(1);

        buttons = new JPanel();
        labels = new JPanel();

        buttons.add(start_frame);
        buttons.add(last_frame);
        buttons.add(play_pause);
        buttons.add(next_frame);
        buttons.add(end_frame);
        labels.add(frameLabel1);
        labels.add(frameNumber);
        labels.add(frameLabel2);
        labels.add(frameTotal);

        // CALIBRATE BUTTONS LAYOUT

        getContentPane().add("North", labels);
        getContentPane().add("Center",buttons);
        getContentPane().add("South", slide);

        setLocation(x,y);
        pack();
        setVisible(false);
    }

    public void actionPerformed(ActionEvent e)
    {
        String command = e.getActionCommand();

        if (command.equals(PLAY_PAUSE_ACTION)) {
            if (log.getPaused()) {
                play_pause.setText(PAUSE_STRING);
                log.logPlay();
            } else {
                play_pause.setText(PLAY_STRING);
                log.logPause();
            }
        } else if (command.equals(START_FRAME_ACTION)) {
            log.logStartFrame();
        } else if (command.equals(END_FRAME_ACTION)) {
            log.logEndFrame();
        } else if (command.equals(NEXT_FRAME_ACTION)) {
            log.logNextFrame();
        } else if (command.equals(LAST_FRAME_ACTION)) {
            log.logLastFrame();
        }
    }

    /** Listen to the slider. */
    public void stateChanged(ChangeEvent e)
    {
        JSlider source = (JSlider)e.getSource();
        if (!source.getValueIsAdjusting()) {
            int thresh = (int)source.getValue();
            // clear simulation history if the slider jumps more than
            // one frame in either direction
            if (Math.abs(log.getLogMarker()-thresh) > 1) {
                painter.clearSimulationHistory();
            }
            log.setLogMarker(thresh);
        }
    }
}
