package teamcomm.gui;

import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.border.EmptyBorder;
import teamcomm.net.logging.LogReplayEvent;
import teamcomm.net.logging.LogReplayEventListener;
import teamcomm.net.logging.LogReplayer;

/**
 * Class for an always-on-top window containing controls for replaying the
 * contents of a log file.
 *
 * @author Felix Thielke
 */
public class LogReplayFrame extends JFrame implements LogReplayEventListener {

    private static final long serialVersionUID = -2837554836011688982L;

    private static final float MAX_REPLAY_SPEED = 128;

    private final JFrame parent;

    private final JLabel stateLabel = new JLabel("Paused");
    private final JLabel timeLabel = new JLabel("00:00");

    private final JButton fastRewindButton = new JButton("<<");
    private final JButton rewindButton = new JButton("<");
    private final JButton pauseButton = new JButton("||");
    private final JButton playButton = new JButton(">");
    private final JButton fastForwardButton = new JButton(">>");

    private float lastSpeed = 0;

    /**
     * Constructor.
     *
     * @param parent a window in the center of which this window is initially
     * positioned
     */
    public LogReplayFrame(final JFrame parent) {
        super("Replay log file");

        this.parent = parent;
        final LogReplayFrame frame = this;

        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                setDefaultCloseOperation(HIDE_ON_CLOSE);
                addWindowListener(new WindowAdapter() {
                    @Override
                    public void windowClosing(WindowEvent e) {
                        LogReplayer.getInstance().close();
                    }
                });

                final JPanel contentPane = new JPanel();
                contentPane.setLayout(new BoxLayout(contentPane, BoxLayout.Y_AXIS));
                contentPane.setBorder(new EmptyBorder(5, 5, 10, 5));
                setContentPane(contentPane);

                final JPanel infoPanel = new JPanel();
                infoPanel.setLayout(new BoxLayout(infoPanel, BoxLayout.X_AXIS));
                infoPanel.setBorder(new EmptyBorder(5, 8, 5, 8));
                stateLabel.setHorizontalAlignment(SwingConstants.LEFT);
                infoPanel.add(stateLabel);
                infoPanel.add(new Box.Filler(new Dimension(), new Dimension(), new Dimension(32767, 0)));
                timeLabel.setHorizontalAlignment(SwingConstants.RIGHT);
                infoPanel.add(timeLabel);
                contentPane.add(infoPanel);

                contentPane.add(new Box.Filler(new Dimension(), new Dimension(), new Dimension(0, 32767)));

                final JPanel controlsPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 5, 0));
                fastRewindButton.addActionListener(new ActionListener() {
                    @Override
                    public void actionPerformed(ActionEvent e) {
                        if (lastSpeed < 0) {
                            LogReplayer.getInstance().setPlaybackSpeed(Math.max(lastSpeed * 2, -MAX_REPLAY_SPEED));
                        } else {
                            LogReplayer.getInstance().setPlaybackSpeed(-2);
                        }
                    }
                });
                controlsPanel.add(fastRewindButton);
                controlsPanel.add(new Box.Filler(new Dimension(), new Dimension(), new Dimension(32767, 0)));
                rewindButton.addActionListener(new ActionListener() {
                    @Override
                    public void actionPerformed(ActionEvent e) {
                        LogReplayer.getInstance().setPlaybackSpeed(-1);
                    }
                });
                controlsPanel.add(rewindButton);
                controlsPanel.add(new Box.Filler(new Dimension(), new Dimension(), new Dimension(32767, 0)));
                pauseButton.addActionListener(new ActionListener() {
                    @Override
                    public void actionPerformed(ActionEvent e) {
                        LogReplayer.getInstance().setPlaybackSpeed(0);
                    }
                });
                controlsPanel.add(pauseButton);
                controlsPanel.add(new Box.Filler(new Dimension(), new Dimension(), new Dimension(32767, 0)));
                playButton.addActionListener(new ActionListener() {
                    @Override
                    public void actionPerformed(ActionEvent e) {
                        LogReplayer.getInstance().setPlaybackSpeed(1);
                    }
                });
                controlsPanel.add(playButton);
                controlsPanel.add(new Box.Filler(new Dimension(), new Dimension(), new Dimension(32767, 0)));
                fastForwardButton.addActionListener(new ActionListener() {
                    @Override
                    public void actionPerformed(ActionEvent e) {
                        if (lastSpeed > 0) {
                            LogReplayer.getInstance().setPlaybackSpeed(Math.min(lastSpeed * 2, MAX_REPLAY_SPEED));
                        } else {
                            LogReplayer.getInstance().setPlaybackSpeed(2);
                        }
                    }
                });
                controlsPanel.add(fastForwardButton);
                contentPane.add(controlsPanel);

                setAlwaysOnTop(true);
                setResizable(false);
                pack();

                LogReplayer.getInstance().addListener(frame);
            }
        });
    }

    @Override
    public void logReplayStatus(final LogReplayEvent e) {
        if (e.atBeginning) {
            fastRewindButton.setEnabled(false);
            rewindButton.setEnabled(false);
        } else {
            fastRewindButton.setEnabled(true);
            if (e.playbackSpeed <= -2) {
                if (e.playbackSpeed <= -MAX_REPLAY_SPEED) {
                    fastRewindButton.setEnabled(false);
                }
                rewindButton.setEnabled(true);
                stateLabel.setText("Fast rewind " + e.playbackSpeed + "x");
            } else {
                if (e.playbackSpeed < 0) {
                    rewindButton.setEnabled(false);
                    stateLabel.setText("Rewinding");
                } else {
                    rewindButton.setEnabled(true);
                }
            }
        }
        if (e.atEnd) {
            playButton.setEnabled(false);
            fastForwardButton.setEnabled(false);
        } else {
            fastForwardButton.setEnabled(true);
            if (e.playbackSpeed >= 2) {
                if (e.playbackSpeed >= MAX_REPLAY_SPEED) {
                    fastForwardButton.setEnabled(false);
                }
                playButton.setEnabled(true);
                stateLabel.setText("Fast forward " + e.playbackSpeed + "x");
            } else {
                if (e.playbackSpeed > 0) {
                    playButton.setEnabled(false);
                    stateLabel.setText("Playing");
                } else {
                    playButton.setEnabled(true);
                }
            }
        }
        if (e.playbackSpeed == 0) {
            pauseButton.setEnabled(false);
            final StringBuilder text = new StringBuilder("Paused");
            if (e.atBeginning) {
                if (e.atEnd) {
                    text.append(" (no data)");
                } else {
                    text.append(" (beginning)");
                }
            } else if (e.atEnd) {
                text.append(" (end)");
            }
            stateLabel.setText(text.toString());
        } else {
            pauseButton.setEnabled(true);
        }

        final int minutes = (int) (e.timePosition / 60000);
        final byte seconds = (byte) ((e.timePosition / 1000) % 60);
        timeLabel.setText((minutes < 10 ? "0" : "") + minutes + ":" + (seconds < 10 ? "0" : "") + seconds);

        lastSpeed = e.playbackSpeed;
    }

    @Override
    public void logReplayStarted() {
        fastRewindButton.setEnabled(false);
        rewindButton.setEnabled(false);
        pauseButton.setEnabled(false);
        playButton.setEnabled(true);
        fastForwardButton.setEnabled(true);
        stateLabel.setText("Paused");
        timeLabel.setText("00:00");

        setLocationRelativeTo(parent);
        setVisible(true);
    }

    @Override
    public void logReplayEnded() {
        setVisible(false);
    }
}
