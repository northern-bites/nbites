package teamcomm.gui;

import common.Log;
import data.SPLStandardMessage;
import java.awt.Color;
import java.awt.GridLayout;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.text.DecimalFormat;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import javax.swing.border.TitledBorder;
import teamcomm.data.AdvancedMessage;
import teamcomm.data.GameState;
import teamcomm.data.RobotState;
import teamcomm.data.event.RobotStateEvent;
import teamcomm.data.event.RobotStateEventListener;

/**
 * Class for the windows showing detailed information about robots.
 *
 * @author Felix Thielke
 */
public class RobotDetailFrame extends JFrame implements RobotStateEventListener {

    private static final long serialVersionUID = 4709653396291218508L;

    private final RobotState robot;
    private final JPanel leftPanel = new JPanel();
    private final JPanel rightPanel = new JPanel();
    private final Color defaultColor = new JLabel("test").getForeground();

    /**
     * Constructor.
     *
     * @param robot robot to create the frame for
     * @param anchor panel which triggers the frame on doubleclick
     */
    public RobotDetailFrame(final RobotState robot, final JPanel anchor) {
        super(robot.getAddress());

        this.robot = robot;

        final RobotStateEventListener listener = this;

        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
                anchor.addMouseListener(new MouseAdapter() {
                    @Override
                    public void mouseClicked(MouseEvent e) {
                        if (e.getClickCount() == 2) {
                            if (!isVisible()) {
                                setLocationRelativeTo(anchor);
                            }
                            setVisible(true);
                            update();
                            repaint();
                        }
                    }
                });

                final JPanel contentPane = new JPanel();
                setContentPane(contentPane);

                contentPane.setLayout(new GridLayout(1, 2, 0, 5));
                contentPane.add(leftPanel);

                contentPane.setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(defaultColor), robot.getAddress(), TitledBorder.CENTER, TitledBorder.TOP));

                leftPanel.setLayout(new BoxLayout(leftPanel, BoxLayout.Y_AXIS));
                rightPanel.setLayout(new BoxLayout(rightPanel, BoxLayout.Y_AXIS));

                for (int i = 0; i < 23; i++) {
                    leftPanel.add(new JLabel(" ", JLabel.LEFT));
                }

                update();

                pack();
                setResizable(false);

                robot.addListener(listener);
            }
        });
    }

    @Override
    public void robotStateChanged(final RobotStateEvent e) {
        if (isVisible()) {
            SwingUtilities.invokeLater(new Runnable() {
                @Override
                public void run() {
                    update();
                    repaint();
                }
            });
        }
    }

    /**
     * Releases resources of this frame.
     */
    public void destroy() {
        setVisible(false);
        robot.removeListener(this);
    }

    @Override
    public void dispose() {
        super.dispose();
        robot.removeListener(this);
    }

    /**
     * Updates the frame with information of the given robot.
     */
    private void update() {
        final SPLStandardMessage msg = robot.getLastMessage();
        if (msg != null) {
            final DecimalFormat df = new DecimalFormat("#.#####");
            if (!msg.teamNumValid || msg.teamNum != robot.getTeamNumber()) {
                ((JLabel) leftPanel.getComponent(0)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(0)).setText("Invalid team no: " + msg.teamNum);
            } else {
                ((JLabel) leftPanel.getComponent(0)).setForeground(defaultColor);
                ((JLabel) leftPanel.getComponent(0)).setText(GameState.getInstance().getTeamName(robot.getTeamNumber(), true, true));
            }
            if (robot.getPlayerNumber() == null || !msg.playerNumValid) {
                ((JLabel) leftPanel.getComponent(1)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(1)).setText("Player no: " + msg.playerNum);
            } else {
                ((JLabel) leftPanel.getComponent(1)).setForeground(defaultColor);
                ((JLabel) leftPanel.getComponent(1)).setText("Player no: " + robot.getPlayerNumber());
            }
            ((JLabel) leftPanel.getComponent(2)).setText("Messages: " + robot.getMessageCount());
            ((JLabel) leftPanel.getComponent(3)).setText("Per second: " + df.format(robot.getMessagesPerSecond()));
            if (!msg.valid) {
                ((JLabel) leftPanel.getComponent(4)).setForeground(Color.red);
            }
            ((JLabel) leftPanel.getComponent(4)).setText("Illegal: " + robot.getIllegalMessageCount() + " (" + Math.round(robot.getIllegalMessageRatio() * 100.0) + "%)");
            if (msg.fallenValid) {
                ((JLabel) leftPanel.getComponent(6)).setForeground(defaultColor);
                ((JLabel) leftPanel.getComponent(6)).setText(msg.fallen ? "fallen" : "upright");
            } else {
                ((JLabel) leftPanel.getComponent(6)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(6)).setText("unknown state");
            }
            if (msg.intentionValid) {
                ((JLabel) leftPanel.getComponent(7)).setForeground(defaultColor);
                ((JLabel) leftPanel.getComponent(7)).setText("Activity: " + msg.intention.toString());
            } else {
                ((JLabel) leftPanel.getComponent(7)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(7)).setText("Activity: ?");
            }
            ((JLabel) leftPanel.getComponent(9)).setText("Confidence:");
            if (msg.currentPositionConfidenceValid) {
                ((JLabel) leftPanel.getComponent(10)).setForeground(defaultColor);
                ((JLabel) leftPanel.getComponent(10)).setText("Position: " + msg.currentPositionConfidence + "%");
            } else {
                ((JLabel) leftPanel.getComponent(10)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(10)).setText("Position: " + msg.currentPositionConfidence);
            }
            if (msg.currentSideConfidenceValid) {
                ((JLabel) leftPanel.getComponent(11)).setForeground(defaultColor);
                ((JLabel) leftPanel.getComponent(11)).setText("Side: " + msg.currentSideConfidence + "%");
            } else {
                ((JLabel) leftPanel.getComponent(11)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(11)).setText("Side: " + msg.currentSideConfidence);
            }
            if (msg.averageWalkSpeedValid) {
                ((JLabel) leftPanel.getComponent(13)).setForeground(defaultColor);
                ((JLabel) leftPanel.getComponent(13)).setText("Avg. walk speed: " + msg.averageWalkSpeed + "mm/s");
            } else {
                ((JLabel) leftPanel.getComponent(13)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(13)).setText("Avg. walk speed: " + msg.averageWalkSpeed);
            }
            if (msg.maxKickDistanceValid) {
                ((JLabel) leftPanel.getComponent(14)).setForeground(defaultColor);
                ((JLabel) leftPanel.getComponent(14)).setText("Max. kick distance: " + msg.maxKickDistance + "mm");
            } else {
                ((JLabel) leftPanel.getComponent(14)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(14)).setText("Max. kick distance: " + msg.maxKickDistance);
            }

            for (int i = 0; i < 5; i++) {
                if (msg.suggestionValid[i]) {
                    ((JLabel) leftPanel.getComponent(16 + i)).setForeground(defaultColor);
                    ((JLabel) leftPanel.getComponent(16 + i)).setText("Suggestion " + (i + 1) + ": " + msg.suggestion[i].toString());
                } else {
                    ((JLabel) leftPanel.getComponent(16 + i)).setForeground(Color.red);
                    ((JLabel) leftPanel.getComponent(16 + i)).setText("Suggestion " + (i + 1) + ": ?");
                }
            }

            if (msg.dataValid) {
                ((JLabel) leftPanel.getComponent(22)).setForeground(defaultColor);
                ((JLabel) leftPanel.getComponent(22)).setText("Additional data: " + msg.data.length + "B (" + (msg.data.length * 100 / SPLStandardMessage.SPL_STANDARD_MESSAGE_DATA_SIZE) + "%)");
            } else {
                ((JLabel) leftPanel.getComponent(22)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(22)).setText("Additional data: " + msg.nominalDataBytes + "B");
            }

            if (!msg.poseValid) {
                while (leftPanel.getComponentCount() < 24) {
                    leftPanel.add(new JLabel(" ", JLabel.LEFT));
                }
                ((JLabel) leftPanel.getComponent(23)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(23)).setText("Invalid pose: x" + df.format(msg.pose[0]) + " y" + df.format(msg.pose[1]) + " t" + df.format(msg.pose[2]));
            }

            if (!msg.walkingToValid) {
                while (leftPanel.getComponentCount() < 25) {
                    leftPanel.add(new JLabel(" ", JLabel.LEFT));
                }
                ((JLabel) leftPanel.getComponent(24)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(24)).setText("Invalid walking target: x" + df.format(msg.walkingTo[0]) + " y" + df.format(msg.walkingTo[1]));
            }

            if (!msg.shootingToValid) {
                while (leftPanel.getComponentCount() < 26) {
                    leftPanel.add(new JLabel(" ", JLabel.LEFT));
                }
                ((JLabel) leftPanel.getComponent(25)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(25)).setText("Invalid shooting target: x" + df.format(msg.shootingTo[0]) + " y" + df.format(msg.shootingTo[1]));
            }

            if (!msg.ballValid) {
                while (leftPanel.getComponentCount() < 26) {
                    leftPanel.add(new JLabel(" ", JLabel.LEFT));
                }
                ((JLabel) leftPanel.getComponent(25)).setForeground(Color.red);
                ((JLabel) leftPanel.getComponent(25)).setText("Invalid ball data: age " + msg.ballAge + " x" + msg.ball[0] + "/" + msg.ballVel[0] + " y" + msg.ball[1] + "/" + msg.ballVel[1]);
            }
        }

        if (msg instanceof AdvancedMessage) {
            final String[] data;
            try {
                data = ((AdvancedMessage) msg).display();
            } catch (final Throwable e) {
                Log.error(e.getClass().getSimpleName() + " was thrown while displaying custom message data from " + msg.getClass().getSimpleName() + ": " + e.getMessage());
                return;
            }
            if (data != null && data.length != 0) {
                while (rightPanel.getComponentCount() < data.length + 6) {
                    rightPanel.add(new JLabel(" ", JLabel.LEFT));
                }
                while (rightPanel.getComponentCount() > data.length + 6) {
                    rightPanel.remove(rightPanel.getComponentCount() - 1);
                }

                for (int i = 0; i < data.length; i++) {
                    if (data[i] != null) {
                        if (data[i].isEmpty()) {
                            ((JLabel) rightPanel.getComponent(i + 6)).setText(" ");
                        } else {
                            ((JLabel) rightPanel.getComponent(i + 6)).setText(data[i]);
                        }
                    }
                }

                if (getContentPane().getComponentCount() == 1) {
                    getContentPane().add(rightPanel);
                    pack();
                }
            } else if (getContentPane().getComponentCount() == 2) {
                getContentPane().remove(rightPanel);
                pack();
            }
        } else if (getContentPane().getComponentCount() == 2) {
            getContentPane().remove(rightPanel);
            pack();
        }
    }

    @Override
    public void connectionStatusChanged(final RobotStateEvent e) {

    }
}
