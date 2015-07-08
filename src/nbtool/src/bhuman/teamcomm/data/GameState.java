package teamcomm.data;

import data.GameControlData;
import data.Rules;
import data.SPLStandardMessage;
import data.TeamInfo;
import data.Teams;
import java.awt.image.BufferedImage;
import java.util.Collection;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.TreeSet;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import javax.swing.event.EventListenerList;
import teamcomm.PluginLoader;
import teamcomm.data.event.TeamEvent;
import teamcomm.data.event.TeamEventListener;
import teamcomm.net.logging.LogReplayer;
import teamcomm.net.logging.Logger;

/**
 * Singleton class managing the known information about communicating robots.
 *
 * @author Felix Thielke
 */
public class GameState {

    /**
     * Index of the team playing on the left side of the field.
     */
    public static final int TEAM_LEFT = 0;
    /**
     * Index of the team playing on the right side of the field.
     */
    public static final int TEAM_RIGHT = 1;
    /**
     * Index of the virtual team containing illegally communicating robots.
     */
    public static final int TEAM_OTHER = 2;

    private static final int CHANGED_LEFT = 1;
    private static final int CHANGED_RIGHT = 2;
    private static final int CHANGED_OTHER = 4;

    private static final GameState instance = new GameState();

    private GameControlData lastGameControlData;

    private final int[] teamNumbers = new int[]{0, 0};
    private final Map<Integer, Integer> teamColors = new HashMap<Integer, Integer>();

    private boolean mirrored = false;

    private final Map<Integer, Collection<RobotState>> robots = new HashMap<Integer, Collection<RobotState>>();

    private static final Comparator<RobotState> playerNumberComparator = new Comparator<RobotState>() {
        @Override
        public int compare(RobotState o1, RobotState o2) {
            if (o1.getPlayerNumber() == null) {
                if (o2.getPlayerNumber() == null) {
                    return o1.hashCode() - o2.hashCode();
                }
                return -1;
            } else if (o2.getPlayerNumber() == null) {
                return 1;
            }
            return o1.getPlayerNumber() - o2.getPlayerNumber();
        }
    };

    private final HashMap<String, RobotState> robotsByAddress = new HashMap<String, RobotState>();

    private final EventListenerList listeners = new EventListenerList();

    private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
    private final ScheduledFuture<?> taskHandle;

    /**
     * Returns the only instance of the RobotData class.
     *
     * @return instance
     */
    public static GameState getInstance() {
        return instance;
    }

    private GameState() {
        taskHandle = scheduler.scheduleAtFixedRate(new Runnable() {
            @Override
            public void run() {
                if (!(LogReplayer.getInstance().isReplaying() && LogReplayer.getInstance().isPaused())) {
                    int changed = 0;

                    synchronized (robotsByAddress) {
                        final Iterator<RobotState> iter = robotsByAddress.values().iterator();
                        while (iter.hasNext()) {
                            final RobotState r = iter.next();
                            if (r.isInactive()) {
                                iter.remove();

                                final Collection<RobotState> team = robots.get(r.getTeamNumber());
                                team.remove(r);

                                synchronized (teamNumbers) {
                                    if (r.getTeamNumber() == teamNumbers[TEAM_LEFT]) {
                                        changed |= CHANGED_LEFT;
                                        if (team.isEmpty() && lastGameControlData == null) {
                                            teamNumbers[TEAM_LEFT] = 0;
                                        }
                                    } else if (r.getTeamNumber() == teamNumbers[TEAM_RIGHT]) {
                                        changed |= CHANGED_RIGHT;
                                        if (team.isEmpty() && lastGameControlData == null) {
                                            teamNumbers[TEAM_RIGHT] = 0;
                                        }
                                    } else {
                                        changed |= CHANGED_OTHER;
                                    }
                                }
                            }
                        }
                    }

                    sendEvents(changed);
                }
            }
        }, RobotState.MILLISECONDS_UNTIL_INACTIVE * 2, RobotState.MILLISECONDS_UNTIL_INACTIVE / 2, TimeUnit.MILLISECONDS);
    }

    /**
     * Shuts down the thread which removes inactive robots. To be called before
     * the program exits.
     */
    public void shutdown() {
        taskHandle.cancel(false);
    }

    /**
     * Resets all information about robots and teams.
     */
    public void reset() {
        lastGameControlData = null;
        synchronized (teamNumbers) {
            teamNumbers[0] = 0;
            teamNumbers[1] = 0;
        }
        synchronized (robotsByAddress) {
            robots.clear();
            robotsByAddress.clear();
        }
        sendEvents(CHANGED_LEFT | CHANGED_RIGHT | CHANGED_OTHER);
    }

    /**
     * Updates info about the game with a message from the GameController.
     *
     * @param data data sent by the GameController
     */
    public void updateGameData(final GameControlData data) {
        int changed = 0;

        if (data == null) {
            if (lastGameControlData != null) {
                synchronized (teamNumbers) {
                    teamNumbers[TEAM_LEFT] = 0;
                    teamNumbers[TEAM_RIGHT] = 0;
                    int s = 0;
                    for (final Entry<Integer, Collection<RobotState>> entry : robots.entrySet()) {
                        if (!entry.getValue().isEmpty()) {
                            teamNumbers[s++] = entry.getKey();
                            if (s == 2) {
                                break;
                            }
                        }
                    }
                }
                changed = CHANGED_LEFT | CHANGED_RIGHT | CHANGED_OTHER;
                Logger.getInstance().createLogfile();
            }
        } else {
            if (lastGameControlData == null) {
                synchronized (teamNumbers) {
                    teamNumbers[TEAM_LEFT] = data.team[0].teamNumber;
                    teamNumbers[TEAM_RIGHT] = data.team[1].teamNumber;
                }
                changed = CHANGED_LEFT | CHANGED_RIGHT | CHANGED_OTHER;
            } else {
                synchronized (teamNumbers) {
                    if (data.team[0].teamNumber != teamNumbers[TEAM_LEFT]) {
                        teamNumbers[TEAM_LEFT] = data.team[0].teamNumber;
                        changed = CHANGED_LEFT | CHANGED_OTHER;
                    }
                    if (data.team[1].teamNumber != teamNumbers[TEAM_RIGHT]) {
                        teamNumbers[TEAM_RIGHT] = data.team[1].teamNumber;
                        changed = CHANGED_RIGHT | CHANGED_OTHER;
                    }
                }
            }

            teamColors.put((int) data.team[0].teamNumber, (int) data.team[0].teamColor);
            teamColors.put((int) data.team[1].teamNumber, (int) data.team[1].teamColor);

            // Update penalties
            for (final TeamInfo team : data.team) {
                final Collection<RobotState> teamRobots = robots.get((int) team.teamNumber);
                if (teamRobots != null) {
                    for (final RobotState r : teamRobots) {
                        if (r.getPlayerNumber() != null && r.getPlayerNumber() <= team.player.length) {
                            r.setPenalty(team.player[r.getPlayerNumber() - 1].penalty);
                        }
                    }
                }
            }

            // Open a new logfile for the current GameController state if the
            // state changed from or to initial/finished
            final StringBuilder logfileName;
            if ((data.team[0].teamNumber == 98 || data.team[0].teamNumber == 99) && (data.team[1].teamNumber == 98 || data.team[1].teamNumber == 99)) {
                logfileName = new StringBuilder("Drop-in_");
                if (data.firstHalf == GameControlData.C_TRUE) {
                    logfileName.append("1st");
                } else {
                    logfileName.append("2nd");
                }
                logfileName.append("Half");
            } else {
                if (data.firstHalf == GameControlData.C_TRUE) {
                    logfileName = new StringBuilder(getTeamName((int) data.team[0].teamNumber, false, false)).append("_").append(getTeamName((int) data.team[1].teamNumber, false, false)).append("_1st");
                } else {
                    logfileName = new StringBuilder(getTeamName((int) data.team[1].teamNumber, false, false)).append("_").append(getTeamName((int) data.team[0].teamNumber, false, false)).append("_2nd");
                }
                logfileName.append("Half");
            }
            if (data.gameState == GameControlData.STATE_READY && (lastGameControlData == null || lastGameControlData.gameState == GameControlData.STATE_INITIAL)) {
                Logger.getInstance().createLogfile(logfileName.toString());
            } else if (data.gameState == GameControlData.STATE_INITIAL && (lastGameControlData == null || lastGameControlData.gameState != GameControlData.STATE_INITIAL)) {
                Logger.getInstance().createLogfile(logfileName.append("_initial").toString());
            } else if (data.gameState == GameControlData.STATE_FINISHED && (lastGameControlData == null || lastGameControlData.gameState != GameControlData.STATE_FINISHED)) {
                Logger.getInstance().createLogfile(logfileName.append("_finished").toString());
            }

            if (changed != 0) {
                // (re)load plugins
                PluginLoader.getInstance().update((int) data.team[0].teamNumber, (int) data.team[1].teamNumber);
            }
        }
        lastGameControlData = data;

        // Log the GameController data
        if (data != null || changed != 0) {
            Logger.getInstance().log(data);
        }

        // send events
        sendEvents(changed);
    }

    /**
     * Handles a message that was received from a robot.
     *
     * @param address IP address of the sender
     * @param teamNumber team number belonging to the port on which the message
     * was received
     * @param message received message
     */
    public void receiveMessage(final String address, final int teamNumber, final SPLStandardMessage message) {
        int changed = 0;

        // update the team info if no GameController info is available
        if (lastGameControlData == null) {
            synchronized (teamNumbers) {
                for (int i = 0; i < 2; i++) {
                    if (teamNumbers[i] == 0) {
                        teamNumbers[i] = teamNumber;

                        // (re)load plugins
                        PluginLoader.getInstance().update(teamNumber);

                        changed = i + 1 | CHANGED_OTHER;
                        break;
                    } else if (teamNumbers[i] == teamNumber) {
                        break;
                    }
                }
            }
        }

        // create the robot state if it does not yet exist
        RobotState r;
        synchronized (robotsByAddress) {
            r = robotsByAddress.get(address);
            if (r == null) {
                r = new RobotState(address, teamNumber);

                robotsByAddress.put(address, r);
            }

            Collection<RobotState> set = robots.get(teamNumber);
            if (set == null) {
                set = new HashSet<RobotState>();
                robots.put(teamNumber, set);
            }
            if (set.add(r)) {
                if (teamNumbers[TEAM_LEFT] == teamNumber) {
                    changed |= CHANGED_LEFT;
                } else if (teamNumbers[TEAM_RIGHT] == teamNumber) {
                    changed |= CHANGED_RIGHT;
                }
            }
        }

        // let the robot state handle the message
        r.registerMessage(message);

        // send events
        sendEvents(changed);
    }

    private void sendEvents(final int changed) {
        boolean leftSent = false;
        boolean rightSent = false;

        if ((changed & CHANGED_OTHER) != 0) {
            final Collection<RobotState> rs = new TreeSet<RobotState>(playerNumberComparator);
            synchronized (robotsByAddress) {
                for (final Entry<Integer, Collection<RobotState>> entry : robots.entrySet()) {
                    if (entry.getKey() == teamNumbers[TEAM_LEFT]) {
                        if ((changed & CHANGED_LEFT) != 0) {
                            final Collection<RobotState> list = new TreeSet<RobotState>(playerNumberComparator);
                            for (final RobotState r : entry.getValue()) {
                                list.add(r);
                            }
                            fireEvent(new TeamEvent(this, outputSide(TEAM_LEFT), teamNumbers[TEAM_LEFT], list));
                            leftSent = true;
                        }
                    } else if (entry.getKey() == teamNumbers[TEAM_RIGHT]) {
                        if ((changed & CHANGED_RIGHT) != 0) {
                            final Collection<RobotState> list = new TreeSet<RobotState>(playerNumberComparator);
                            for (final RobotState r : entry.getValue()) {
                                list.add(r);
                            }
                            fireEvent(new TeamEvent(this, outputSide(TEAM_RIGHT), teamNumbers[TEAM_RIGHT], list));
                            rightSent = true;
                        }
                    } else {
                        for (final RobotState r : entry.getValue()) {
                            rs.add(r);
                        }
                    }
                }
            }
            fireEvent(new TeamEvent(this, TEAM_OTHER, 0, rs));
        }

        if (!leftSent && (changed & CHANGED_LEFT) != 0) {
            final Collection<RobotState> rs;
            synchronized (robotsByAddress) {
                rs = robots.get(teamNumbers[TEAM_LEFT]);
            }
            final Collection<RobotState> list = new TreeSet<RobotState>(playerNumberComparator);
            if (rs != null) {
                for (final RobotState r : rs) {
                    list.add(r);
                }
            }
            fireEvent(new TeamEvent(this, outputSide(TEAM_LEFT), teamNumbers[TEAM_LEFT], list));
        }

        if (!rightSent && (changed & CHANGED_RIGHT) != 0) {
            final Collection<RobotState> rs;
            synchronized (robotsByAddress) {
                rs = robots.get(teamNumbers[TEAM_RIGHT]);
            }
            final Collection<RobotState> list = new TreeSet<RobotState>(playerNumberComparator);
            if (rs != null) {
                for (final RobotState r : rs) {
                    list.add(r);
                }
            }
            fireEvent(new TeamEvent(this, outputSide(TEAM_RIGHT), teamNumbers[TEAM_RIGHT], list));
        }
    }

    private void fireEvent(final TeamEvent e) {
        for (final TeamEventListener listener : listeners.getListeners(TeamEventListener.class)) {
            listener.teamChanged(e);
        }
    }

    /**
     * Returns the team color of the given team. The team color is either sent
     * by the game controller or given by the GameController configuration.
     *
     * @param teamNumber number of the team
     * @return the team color
     * @see TeamInfo#teamColor
     */
    public int getTeamColor(final int teamNumber) {
        Integer color = teamColors.get(teamNumber);
        if (color == null) {
            String[] colorStrings = null;
            try {
                if (teamNumber == 98 || teamNumber == 99) {
                    Rules.league = Rules.LEAGUES[1];
                } else {
                    Rules.league = Rules.LEAGUES[0];
                }
                colorStrings = Teams.getColors(teamNumber);
            } catch (NullPointerException e) {
            } catch (ArrayIndexOutOfBoundsException e) {
            }
            if (colorStrings == null || colorStrings.length < 1) {
                if (teamNumber == teamNumbers[TEAM_RIGHT]) {
                    return GameControlData.TEAM_RED;
                } else {
                    return GameControlData.TEAM_BLUE;
                }
            } else if (colorStrings[0].equals("blue")) {
                return GameControlData.TEAM_BLUE;
            } else if (colorStrings[0].equals("red")) {
                return GameControlData.TEAM_RED;
            } else if (colorStrings[0].equals("yellow")) {
                return GameControlData.TEAM_YELLOW;
            } else {
                return GameControlData.TEAM_BLACK;
            }
        }

        return color;
    }

    /**
     * Returns the most recently received GameControlData.
     *
     * @return GameControlData of null if none was received recently
     */
    public GameControlData getLastGameControlData() {
        return lastGameControlData;
    }

    /**
     * Returns the team name of the given team.
     *
     * @param teamNumber number of the team
     * @param withNumber whether the team number should be in the returned
     * string
     * @param withPrefix whether the pre- or suffix "Team" should be included
     * @return the team name
     */
    public String getTeamName(final Integer teamNumber, final boolean withNumber, final boolean withPrefix) {
        if (teamNumber == 98 || teamNumber == 99) {
            Rules.league = Rules.LEAGUES[1];
        } else {
            Rules.league = Rules.LEAGUES[0];
        }
        final String[] teamNames = Teams.getNames(withNumber);
        if (teamNumber != null) {
            if (teamNumber < teamNames.length && teamNames[teamNumber] != null) {
                return ((withPrefix ? "Team " : "") + teamNames[teamNumber]);
            } else {
                return ("Unknown" + (withPrefix ? " Team" : "") + (withNumber ? " (" + teamNumber + ")" : ""));
            }
        } else {
            return "Unknown" + (withPrefix ? " Team" : "");
        }
    }

    /**
     * Returns the icon of the given team.
     *
     * @param teamNumber number of the team
     * @return icon as a BufferedImage
     */
    public BufferedImage getTeamIcon(final int teamNumber) {
        if (teamNumber == 98 || teamNumber == 99) {
            Rules.league = Rules.LEAGUES[1];
        } else {
            Rules.league = Rules.LEAGUES[0];
        }
        return Teams.getIcon(teamNumber);
    }

    /**
     * Returns whether the team sides are mirrored.
     *
     * @return boolean
     */
    public boolean isMirrored() {
        return mirrored;
    }

    /**
     * Sets whether the team sides are mirrored.
     *
     * @param mirrored boolean
     */
    public void setMirrored(final boolean mirrored) {
        if (mirrored != this.mirrored) {
            this.mirrored = mirrored;
            sendEvents(CHANGED_LEFT | CHANGED_RIGHT);
        }
    }

    private int outputSide(final int side) {
        return mirrored ? (side == 0 ? 1 : (side == 1 ? 0 : side)) : side;
    }

    /**
     * Registeres a GUI component as a listener receiving events about team
     * changes.
     *
     * @param listener component
     */
    public void addListener(final TeamEventListener listener) {
        listeners.add(TeamEventListener.class, listener);
        sendEvents(CHANGED_LEFT | CHANGED_RIGHT | CHANGED_OTHER);
    }

}
