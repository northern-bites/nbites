package teamcomm.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.text.DecimalFormat;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.GroupLayout;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.OverlayLayout;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.border.TitledBorder;
import teamcomm.data.RobotState;
import teamcomm.data.event.RobotStateEvent;
import teamcomm.data.event.RobotStateEventListener;

/**
 * Class for the panel showing basic information about robots.
 *
 * @author Felix Thielke
 */
public class RobotPanel extends JPanel implements RobotStateEventListener {

    private static final long serialVersionUID = 6656251707032959704L;

    /**
     * Width of a robot panel.
     */
    public static final int PANEL_WIDTH = 175;

    /**
     * Height of a robot panel.
     */
    public static final int PANEL_HEIGHT = 105;

    private static final double MPS_LEGAL_THRESHOLD = 5.25;

    private final RobotState robot;
    private final RobotDetailFrame detailFrame;

    private final JPanel foregroundPanel = new JPanel();
    private final JLabel connectionStatus = new JLabel();
    private final JLabel teamLogo = new JLabel((Icon) null, SwingConstants.CENTER);

    private static final Color defaultColor = new JLabel("test").getForeground();
    private static final String ICONS_PATH = "config/icons/";
    private static final String ICON_ONLINE = "wlan_status_green.png";
    private static final String ICON_OFFLINE = "wlan_status_red.png";
    private static final String ICON_HIGH_LATENCY = "wlan_status_yellow.png";
    private static ImageIcon iconOnline;
    private static ImageIcon iconOffline;
    private static ImageIcon iconHighLatency;

    /**
     * Constructor.
     *
     * @param robot robot to create the panel for
     */
    public RobotPanel(final RobotState robot) {
        super();

        loadIcons();

        this.robot = robot;

        final RobotPanel robotPanel = this;
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(getForeground()), robot.getAddress(), TitledBorder.CENTER, TitledBorder.TOP));
                setLayout(new OverlayLayout(robotPanel));
                setPreferredSize(new Dimension(PANEL_WIDTH, PANEL_HEIGHT));
                setMaximumSize(new Dimension(PANEL_WIDTH, PANEL_HEIGHT));

                foregroundPanel.setLayout(new BoxLayout(foregroundPanel, BoxLayout.Y_AXIS));
                foregroundPanel.setOpaque(false);
                for (int i = 0; i < 4; i++) {
                    foregroundPanel.add(new JLabel(" ", JLabel.LEFT));
                }
                final JPanel foregroundContainer = new JPanel();
                foregroundContainer.setOpaque(false);
                final GroupLayout foregroundContainerLayout = new GroupLayout(foregroundContainer);
                foregroundContainer.setLayout(foregroundContainerLayout);
                foregroundContainerLayout.setHorizontalGroup(
                        foregroundContainerLayout.createParallelGroup(GroupLayout.Alignment.LEADING)
                        .addComponent(foregroundPanel, GroupLayout.Alignment.TRAILING, GroupLayout.DEFAULT_SIZE, PANEL_WIDTH, Short.MAX_VALUE)
                );
                foregroundContainerLayout.setVerticalGroup(
                        foregroundContainerLayout.createParallelGroup(GroupLayout.Alignment.LEADING)
                        .addComponent(foregroundPanel, GroupLayout.Alignment.TRAILING, GroupLayout.DEFAULT_SIZE, PANEL_HEIGHT, Short.MAX_VALUE)
                );
                add(foregroundContainer);

                connectionStatus.setIcon(iconOnline);
                final JPanel backgroundContainer = new JPanel();
                backgroundContainer.setOpaque(false);
                final GroupLayout backgroundContainerLayout = new GroupLayout(backgroundContainer);
                backgroundContainer.setLayout(backgroundContainerLayout);
                backgroundContainerLayout.setHorizontalGroup(
                        backgroundContainerLayout.createParallelGroup(GroupLayout.Alignment.LEADING)
                        .addGroup(GroupLayout.Alignment.TRAILING, backgroundContainerLayout.createSequentialGroup()
                                .addGap(0, 220, Short.MAX_VALUE)
                                .addComponent(connectionStatus))
                );
                backgroundContainerLayout.setVerticalGroup(
                        backgroundContainerLayout.createParallelGroup(GroupLayout.Alignment.LEADING)
                        .addGroup(backgroundContainerLayout.createSequentialGroup()
                                .addComponent(connectionStatus)
                                .addGap(0, 181, Short.MAX_VALUE))
                );
                add(backgroundContainer);

                final JPanel backbackgroundContainer = new JPanel();
                backbackgroundContainer.setOpaque(false);
                final GroupLayout backbackgroundContainerLayout = new GroupLayout(backbackgroundContainer);
                backbackgroundContainer.setLayout(backbackgroundContainerLayout);
                backbackgroundContainerLayout.setHorizontalGroup(
                        backbackgroundContainerLayout.createParallelGroup(GroupLayout.Alignment.LEADING)
                        .addComponent(teamLogo, GroupLayout.Alignment.TRAILING, GroupLayout.DEFAULT_SIZE, PANEL_WIDTH, Short.MAX_VALUE)
                );
                backbackgroundContainerLayout.setVerticalGroup(
                        backbackgroundContainerLayout.createParallelGroup(GroupLayout.Alignment.LEADING)
                        .addComponent(teamLogo, GroupLayout.Alignment.TRAILING, GroupLayout.DEFAULT_SIZE, PANEL_HEIGHT, Short.MAX_VALUE)
                );
                add(backbackgroundContainer);

                foregroundContainer.addComponentListener(new ComponentAdapter() {
                    @Override
                    public void componentResized(final ComponentEvent e) {
                        if (teamLogo.getIcon() != null) {
                            teamLogo.setIcon(TeamLogoLoader.getInstance().getScaledSemiTransparentTeamLogoIcon(robot.getTeamNumber(), Math.min((PANEL_WIDTH * 3) >> 2, foregroundPanel.getWidth()), Math.min((PANEL_HEIGHT * 3) >> 2, foregroundPanel.getHeight())));
                        }
                    }
                });

                update();
                robot.addListener(robotPanel);
            }
        });

        detailFrame = new RobotDetailFrame(robot, robotPanel);
    }

    @Override
    public void robotStateChanged(final RobotStateEvent e) {
        if (isVisible()) {
            update();
        }
    }

    @Override
    public void connectionStatusChanged(final RobotStateEvent e) {
        if (isVisible()) {
            switch (robot.getConnectionStatus()) {
                case HIGH_LATENCY:
                    connectionStatus.setIcon(iconHighLatency);
                    break;
                case OFFLINE:
                    connectionStatus.setIcon(iconOffline);
                    break;
                case ONLINE:
                    connectionStatus.setIcon(iconOnline);
            }
        }
    }

    /**
     * Updates the panel with information of the given robot.
     */
    private void update() {
        final DecimalFormat df = new DecimalFormat("#.#####");

        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                if (robot.getPlayerNumber() == null || (robot.getLastMessage() != null && !robot.getLastMessage().playerNumValid)) {
                    ((JLabel) foregroundPanel.getComponent(0)).setForeground(Color.red);
                    ((JLabel) foregroundPanel.getComponent(0)).setText("Player no: " + (robot.getLastMessage() != null ? robot.getLastMessage().playerNum : "invalid"));
                } else {
                    ((JLabel) foregroundPanel.getComponent(0)).setForeground(defaultColor);
                    ((JLabel) foregroundPanel.getComponent(0)).setText("Player no: " + robot.getPlayerNumber());
                }
                ((JLabel) foregroundPanel.getComponent(1)).setText("Messages: " + robot.getMessageCount());

                final double mps = robot.getMessagesPerSecond();
                if (mps >= MPS_LEGAL_THRESHOLD) {
                    ((JLabel) foregroundPanel.getComponent(2)).setForeground(Color.red);
                } else {
                    ((JLabel) foregroundPanel.getComponent(2)).setForeground(defaultColor);
                }
                ((JLabel) foregroundPanel.getComponent(2)).setText("Per second: " + df.format(mps));
                if (robot.getLastMessage() == null || !robot.getLastMessage().valid) {
                    ((JLabel) foregroundPanel.getComponent(3)).setForeground(Color.red);
                } else {
                    ((JLabel) foregroundPanel.getComponent(3)).setForeground(defaultColor);
                }
                ((JLabel) foregroundPanel.getComponent(3)).setText("Illegal: " + robot.getIllegalMessageCount() + " (" + Math.round(robot.getIllegalMessageRatio() * 100.0) + "%)");
            }
        });
    }

    /**
     * Toggle if the team logo shall be displayed in the background of the
     * panel.
     *
     * @param toggle whether the team logo shall be displayeds
     */
    public void setTeamLogoVisible(final boolean toggle) {
        if (toggle) {
            SwingUtilities.invokeLater(new Runnable() {
                @Override
                public void run() {
                    teamLogo.setIcon(TeamLogoLoader.getInstance().getScaledSemiTransparentTeamLogoIcon(robot.getTeamNumber(), Math.min((PANEL_WIDTH * 3) >> 2, Math.max(PANEL_WIDTH >> 2, foregroundPanel.getWidth())), Math.min((PANEL_HEIGHT * 3) >> 2, Math.max(PANEL_HEIGHT >> 2, foregroundPanel.getHeight()))));
                }
            });
        } else {
            SwingUtilities.invokeLater(new Runnable() {
                @Override
                public void run() {
                    teamLogo.setIcon(null);
                }
            });
        }
    }

    /**
     * Releases resources of this panel.
     */
    public void dispose() {
        robot.removeListener(this);
        detailFrame.destroy();
    }

    /**
     * Returns the IP address of the robot associated with this panel.
     *
     * @return IP address
     */
    public String getRobotAddress() {
        return robot.getAddress();
    }

    private static void loadIcons() {
        if (iconOnline == null) {
            iconOnline = new ImageIcon(ICONS_PATH + ICON_ONLINE);
        }
        if (iconHighLatency == null) {
            iconHighLatency = new ImageIcon(ICONS_PATH + ICON_HIGH_LATENCY);
        }
        if (iconOffline == null) {
            iconOffline = new ImageIcon(ICONS_PATH + ICON_OFFLINE);
        }
    }
}
