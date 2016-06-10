package teamcomm.gui;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.InputEvent;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.KeyStroke;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.border.EmptyBorder;
import teamcomm.TeamCommunicationMonitor;
import teamcomm.data.GameState;
import teamcomm.data.RobotState;
import teamcomm.data.event.TeamEvent;
import teamcomm.data.event.TeamEventListener;
import teamcomm.net.logging.LogReplayer;

/**
 * Class for the main window of the application.
 *
 * @author Felix Thielke
 */
public class MainWindow extends JFrame implements TeamEventListener {

    private static final long serialVersionUID = 6549981924840180076L;

    private static final Map<Integer, ImageIcon> logos = new HashMap<>();

    private final View3D fieldView = new View3D();
    private final JPanel[] teamPanels = new JPanel[]{new JPanel(), new JPanel(), new JPanel()};
    private final JLabel[] teamLogos = new JLabel[]{new JLabel((Icon) null, SwingConstants.CENTER), new JLabel((Icon) null, SwingConstants.CENTER)};
    private final Map<String, RobotPanel> robotPanels = new HashMap<>();

    @SuppressWarnings("unused")
    private final LogReplayFrame logReplayFrame = new LogReplayFrame(this);

    /**
     * Constructor.
     */
    public MainWindow() {
        super("TeamCommunicationMonitor");

        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                // Initialize
                initialize();
            }
        });
    }

    private void initialize() {
        // Setup window
        setLocationByPlatform(true);
        setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosed(WindowEvent e) {
                TeamCommunicationMonitor.shutdown();
            }
        });

        // Setup team panels
        final Box left = new Box(BoxLayout.Y_AXIS);
        teamPanels[0].setLayout(new GridLayout(10, 1, 0, 5));
        left.add(teamPanels[0]);
        left.add(new Box.Filler(new Dimension(RobotPanel.PANEL_WIDTH, RobotPanel.PANEL_HEIGHT), new Dimension(RobotPanel.PANEL_WIDTH, RobotPanel.PANEL_HEIGHT), new Dimension(RobotPanel.PANEL_WIDTH, 1000)));
        final Box right = new Box(BoxLayout.Y_AXIS);
        teamPanels[1].setLayout(new GridLayout(10, 1, 0, 5));
        right.add(teamPanels[1]);
        right.add(new Box.Filler(new Dimension(RobotPanel.PANEL_WIDTH, RobotPanel.PANEL_HEIGHT), new Dimension(RobotPanel.PANEL_WIDTH, RobotPanel.PANEL_HEIGHT), new Dimension(RobotPanel.PANEL_WIDTH, 1000)));
        final Box bottom = new Box(BoxLayout.X_AXIS);
        bottom.setBorder(new EmptyBorder(0, RobotPanel.PANEL_WIDTH, 0, RobotPanel.PANEL_WIDTH));
        teamPanels[2].setLayout(new BoxLayout(teamPanels[2], BoxLayout.LINE_AXIS));
        bottom.add(teamPanels[2]);

        // Setup team logos
        teamPanels[0].add(teamLogos[0]);
        teamPanels[1].add(teamLogos[1]);

        // Setup content pane
        final JPanel contentPane = new JPanel(new BorderLayout());
        contentPane.add(left, BorderLayout.WEST);
        contentPane.add(right, BorderLayout.EAST);
        contentPane.add(bottom, BorderLayout.SOUTH);
        contentPane.add(fieldView.getCanvas(), BorderLayout.CENTER);
        setContentPane(contentPane);

        // Add menu bar
        final JMenuBar mb = new JMenuBar();
        mb.add(createFileMenu());
        mb.add(createViewMenu());
        setJMenuBar(mb);

        // Display window
        setPreferredSize(new Dimension(1442, 720));
        pack();
        setVisible(true);

        // Listen for robot events
        GameState.getInstance().addListener(this);
    }

    private JMenu createFileMenu() {
        final JMenu fileMenu = new JMenu("File");
        final JMenuItem resetItem = new JMenuItem("Reset");
        resetItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                GameState.getInstance().reset();
            }
        });
        resetItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F5, 0));
        fileMenu.add(resetItem);

        final JMenuItem replayItem = new JMenuItem("Replay log file");
        fileMenu.add(replayItem);
        replayItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(final ActionEvent e) {
                final JFileChooser fc = new JFileChooser(new File(new File(".").getAbsoluteFile(), "logs_teamcomm"));
                if (fc.showOpenDialog(MainWindow.this) == JFileChooser.APPROVE_OPTION) {
                    try {
                        LogReplayer.getInstance().open(fc.getSelectedFile());
                    } catch (IOException ex) {
                        JOptionPane.showMessageDialog(null,
                                "Error opening log file.",
                                ex.getClass().getSimpleName(),
                                JOptionPane.ERROR_MESSAGE);
                    }
                }
            }
        });
        replayItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, InputEvent.CTRL_DOWN_MASK));

        final JMenuItem exitItem = new JMenuItem("Exit");
        exitItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                TeamCommunicationMonitor.shutdown();
                setVisible(false);
            }
        });
        fileMenu.add(exitItem);

        return fileMenu;
    }

    private JMenu createViewMenu() {
        final JMenu viewMenu = new JMenu("View");

        // Mirroring
        final JCheckBoxMenuItem mirrorOption = new JCheckBoxMenuItem("Mirror", GameState.getInstance().isMirrored());
        mirrorOption.addItemListener(new ItemListener() {
            @Override
            public void itemStateChanged(final ItemEvent e) {
                GameState.getInstance().setMirrored(e.getStateChange() == ItemEvent.SELECTED);
            }
        });
        viewMenu.add(mirrorOption);

        // Drawings
        viewMenu.add(fieldView.getDrawingsMenu());

        return viewMenu;
    }

    /**
     * Terminates the view.
     */
    public void terminate() {
        fieldView.terminate();
    }

    @Override
    public void teamChanged(final TeamEvent e) {
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                if (e.side != GameState.TEAM_OTHER) {
                    teamLogos[e.side].setIcon(TeamLogoLoader.getInstance().getTeamLogoPanelIcon(e.teamNumber));
                }

                int i = 0;
                for (final RobotState r : e.players) {
                    RobotPanel panel = robotPanels.get(r.getAddress());
                    if (panel == null) {
                        panel = new RobotPanel(r);
                        robotPanels.put(r.getAddress(), panel);
                    }

                    if (teamPanels[e.side].getComponentCount() <= i + (e.side < 2 ? 1 : 0)) {
                        teamPanels[e.side].add(panel);
                        panel.revalidate();
                    } else if (panel != teamPanels[e.side].getComponent(i + (e.side < 2 ? 1 : 0))) {
                        teamPanels[e.side].remove(panel);
                        teamPanels[e.side].add(panel, i + (e.side < 2 ? 1 : 0));
                        panel.revalidate();
                    }

                    panel.setTeamLogoVisible(e.side == GameState.TEAM_OTHER);

                    i++;
                }

                boolean teamPanelChanged = false;
                while (e.players.size() < teamPanels[e.side].getComponentCount() - (e.side < 2 ? 1 : 0)) {
                    teamPanelChanged = true;
                    final RobotPanel panel = (RobotPanel) teamPanels[e.side].getComponent(teamPanels[e.side].getComponentCount() - 1);
                    teamPanels[e.side].remove(teamPanels[e.side].getComponentCount() - 1);
                    robotPanels.remove(panel.getRobotAddress());
                    panel.dispose();
                }
                if (teamPanelChanged) {
                    teamPanels[e.side].repaint();
                }
            }
        });
    }
}
