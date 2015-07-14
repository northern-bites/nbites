package teamcomm.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.text.DecimalFormat;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
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

    private final RobotState robot;
    private final RobotDetailFrame detailFrame;

    private final Color defaultColor = new JLabel("test").getForeground();

    /**
     * Constructor.
     *
     * @param robot robot to create the panel for
     */
    public RobotPanel(final RobotState robot) {
        super();

        this.robot = robot;

        final JPanel panel = this;
        final RobotStateEventListener listener = this;

        setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(getForeground()), robot.getAddress(), TitledBorder.CENTER, TitledBorder.TOP));
        setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
        setPreferredSize(new Dimension(PANEL_WIDTH, PANEL_HEIGHT));

        for (int i = 0; i < 5; i++) {
            add(new JLabel(" ", JLabel.LEFT));
        }

        update();
        robot.addListener(listener);

        detailFrame = new RobotDetailFrame(robot, this);
    }

    @Override
    public void robotStateChanged(final RobotStateEvent e) {
        if (isVisible()) {
            update();
        }
    }

    /**
     * Updates the panel with information of the given robot.
     */
    private void update() {
        final DecimalFormat df = new DecimalFormat("#.#####");

        synchronized (getTreeLock()) {
            if (robot.getPlayerNumber() == null || (robot.getLastMessage() != null && !robot.getLastMessage().playerNumValid)) {
                ((JLabel) getComponent(0)).setForeground(Color.red);
                ((JLabel) getComponent(0)).setText("Player no: " + (robot.getLastMessage() != null ? robot.getLastMessage().playerNum : "invalid"));
            } else {
                ((JLabel) getComponent(0)).setForeground(defaultColor);
                ((JLabel) getComponent(0)).setText("Player no: " + robot.getPlayerNumber());
            }
            ((JLabel) getComponent(1)).setText("Messages: " + robot.getMessageCount());
            ((JLabel) getComponent(2)).setText("Current mps: " + df.format(robot.getRecentMessageCount()));
            ((JLabel) getComponent(3)).setText("Average mps: " + df.format(robot.getMessagesPerSecond()));
            if (robot.getLastMessage() == null || !robot.getLastMessage().valid) {
                ((JLabel) getComponent(4)).setForeground(Color.red);
            } else {
                ((JLabel) getComponent(4)).setForeground(defaultColor);
            }
            ((JLabel) getComponent(4)).setText("Illegal: " + robot.getIllegalMessageCount() + " (" + Math.round(robot.getIllegalMessageRatio() * 100.0) + "%)");
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

}
