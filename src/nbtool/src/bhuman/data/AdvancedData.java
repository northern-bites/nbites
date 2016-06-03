package data;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.ArrayList;

import common.Log;
import controller.action.ActionBoard;

/**
 * @author Michel Bartsch
 *
 * This class extends the GameControlData that is send to the robots. It
 * contains all the additional informations the GameControler needs to
 * represent a state of the game, for example time in millis.
 * 
 * There are no synchronized get and set methods because in this architecture
 * only actions in their perform method are allowed to write into this and they
 * are all in the same thread. Look in the EventHandler for more information.
 */
public class AdvancedData extends GameControlData implements Cloneable
{
    private static final long serialVersionUID = 2720243434306304319L;

    /** This message is set when the data is put into the timeline */
    public String message = "";

    /** How much time summed up before the current state? (ms)*/
    public long timeBeforeCurrentGameState;
    
    /** When was switched to the current state? (ms) */
    public long whenCurrentGameStateBegan;
    
    /** When was the last drop-in? (ms, 0 = never) */
    public long whenDropIn;
    
    /** When was each player penalized last (ms, 0 = never)? */
    public long[][] whenPenalized = Rules.league.isCoachAvailable ? new long[2][Rules.league.teamSize+1] : new long[2][Rules.league.teamSize];

    /** How often was each robot penalized with each league-specific penalty? */
    public int[][][] penaltyCount = Rules.league.isCoachAvailable ? new int[2][Rules.league.teamSize + 1][Rules.league.penaltyTime.length]
                                                                  : new int[2][Rules.league.teamSize][Rules.league.penaltyTime.length];

    /** Which players were already ejected? */
    public boolean [][] ejected = Rules.league.isCoachAvailable ? new boolean[2][Rules.league.teamSize+1] : new boolean[2][Rules.league.teamSize];
    
    /** Pushing counters for each team, 0:left side, 1:right side. */
    public int[] pushes = {0, 0};
    
    /** If true, the referee set a timeout */
    public boolean refereeTimeout = false;

    /** If true, this team is currently taking a timeOut, 0:left side, 1:right side. */
    public boolean[] timeOutActive = {false, false};
    
    /** TimeOut counters for each team, 0:left side, 1:right side. */
    public boolean[] timeOutTaken = {false, false};
    
    /** If true, left side has the kickoff. */
    public boolean leftSideKickoff = true;
    
    /** If true, the colors change automatically. */
    public boolean colorChangeAuto;
    
    /** If true, the testmode has been activated. */
    public boolean testmode = false;

    /** If true, the clock has manually been paused in the testmode. */
    public boolean manPause = false;
    
    /** If true, the clock has manually been started in the testmode. */
    public boolean manPlay = false;
    
    /** When was the last manual intervention to the clock? */
    public long manWhenClockChanged;
    
    /** Time offset resulting from manually stopping the clock. */
    public long manTimeOffset;
    
    /** Time offset resulting from starting the clock when it should be stopped. */
    public long manRemainingGameTimeOffset;

    /** Used to backup the secondary game state during a timeout. */
    public byte previousSecGameState = STATE2_NORMAL;

    /** Keeps the penalties for the players if there are substituted */
    public ArrayList<ArrayList<PenaltyQueueData>> penaltyQueueForSubPlayers = new ArrayList<ArrayList<PenaltyQueueData>>();

    /** Keep the timestamp when a coach message was received*/
    public long timestampCoachPackage[] = {0, 0};

    /** Keep the coach messages*/
    public  ArrayList<SPLCoachMessage> splCoachMessageQueue = new ArrayList<SPLCoachMessage>();

    /**
     * Creates a new AdvancedData.
     */
    public AdvancedData()
    {
        if (Rules.league.startWithPenalty) {
            secGameState = GameControlData.STATE2_PENALTYSHOOT;
        }
        for (int i=0; i<2; i++) {
            for (int j=0; j < team[i].player.length; j++) {
                if (j >= Rules.league.robotsPlaying) {
                    team[i].player[j].penalty = PlayerInfo.PENALTY_SUBSTITUTE;
                }
            }
            penaltyQueueForSubPlayers.add(new ArrayList<PenaltyQueueData>());
        }
    }

    /**
     * Generically clone this object. Everything referenced must be Serializable.
     * @return A deep copy of this object.
     */
    public Object clone()
    {
        try {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            new ObjectOutputStream(out).writeObject(this);
            ByteArrayInputStream in = new ByteArrayInputStream(out.toByteArray());
            return new ObjectInputStream(in).readObject();
        } catch (ClassNotFoundException e) {
            System.out.println(e.getClass().getName() + ": " + e.getMessage());
        } catch (IOException e) {
            System.out.println(e.getClass().getName() + ": " + e.getMessage());
        }
        return null; // Should never be reached
    }
    
    /**
     * Returns the side on which a team plays. The team should be playing
     * via this GameController.
     * 
     * @param teamNumber    The unique teamNumber.
     * 
     * @return The side of the team, 0:left side, 1:right side.
     */
    public int getSide(short teamNumber)
    {
        return teamNumber == team[0].teamNumber ? 0 : 1;
    }
    
    /**
     * Returns the current time. Can be stopped in test mode.
     * @return The current time in ms. May become incompatible to
     *         the time delivered by System.currentTimeMillis().
     */
    public long getTime()
    {
        return manPause ? manWhenClockChanged : System.currentTimeMillis() + manTimeOffset;
    }
    
    /**
     * Returns the number of seconds since a certain timestamp.
     * @param millis The timestamp in ms.
     * @return The number of seconds since the timestamp.
     */
    public int getSecondsSince(long millis) {
        return millis == 0 ? 100000 : (int) (getTime() - millis) / 1000;
    }
    
    /**
     * The number of seconds until a certion duration is over. The time
     * already passed is specified as a timestamp when it began.
     * @param millis The timestamp in ms.
     * @param duration The full duration in s.
     * @param The number of seconds that still remain from the duration.
     *        Can be negative.
     */
    public int getRemainingSeconds(long millis, int durationInSeconds) {
        return durationInSeconds - getSecondsSince(millis);
    }

    /**
     * Update all durations in the GameControlData packet.
     */
    public void updateTimes()
    {
        secsRemaining = (short) getRemainingGameTime(false);
        dropInTime = whenDropIn == 0 ? -1 : (short) getSecondsSince(whenDropIn);
        Integer subT = getSecondaryTime(0);

        if (subT == null) {
            secondaryTime = 0;
        } else {
            secondaryTime = (short)(int)subT;
        }
        for (int side = 0; side < team.length; ++side) {
            for (int number = 0; number < team[side].player.length; ++number) {
                PlayerInfo player = team[side].player[number];
                player.secsTillUnpenalised = player.penalty == PlayerInfo.PENALTY_NONE
                        ? 0 : (byte) getRemainingPenaltyTime(side, number);
            }
        }
    }
    
    /**
     * Add the time passed in the current game state to the time that already passed before.
     * Is usually called during changes of the game state.
     */
    public void addTimeInCurrentState()
    {
        timeBeforeCurrentGameState += getTime() - whenCurrentGameStateBegan;
    }
    
    /**
     * Calculates the remaining game time in the current phase of the game.
     * This is what the primary clock will show.
     * @param real If true, the real time will be returned. If false, the first number of seconds in the playing state
     *             in play-off games will not be updated.
     * @return The remaining number of seconds.
     */
    public int getRemainingGameTime(boolean real)
    {
        int regularNumberOfPenaltyShots = (gameType == GAME_PLAYOFF) ? Rules.league.numberOfPenaltyShotsLong : Rules.league.numberOfPenaltyShotsShort;
        int duration = secGameState == STATE2_TIMEOUT ? secsRemaining : 
                secGameState == STATE2_NORMAL ? Rules.league.halfTime
                : secGameState == STATE2_OVERTIME ? Rules.league.overtimeTime
                : Math.max(team[0].penaltyShot, team[1].penaltyShot) > regularNumberOfPenaltyShots
                ? Rules.league.penaltyShotTimeSuddenDeath
                : Rules.league.penaltyShotTime;
        int timePlayed = gameState == STATE_INITIAL// during timeouts
                || (gameState == STATE_READY || gameState == STATE_SET)
                && ((gameType == GAME_PLAYOFF) && Rules.league.playOffTimeStop || timeBeforeCurrentGameState == 0)
                || gameState == STATE_FINISHED
                ? (int) ((timeBeforeCurrentGameState + manRemainingGameTimeOffset + (manPlay ? System.currentTimeMillis() - manWhenClockChanged : 0)) / 1000)
                : real || gameType != GAME_PLAYOFF || secGameState != STATE2_NORMAL || gameState != STATE_PLAYING
                || getSecondsSince(whenCurrentGameStateBegan) >= Rules.league.playOffDelayedSwitchToPlaying 
                ? getSecondsSince(whenCurrentGameStateBegan - timeBeforeCurrentGameState - manRemainingGameTimeOffset)
                : (int) ((timeBeforeCurrentGameState - manRemainingGameTimeOffset) / 1000);
        return duration - timePlayed;
    }
    
    /**
     * The method returns the remaining pause time.
     * @return The remaining number of seconds of the game pause or null if there currently is no pause.
     */
    public Integer getRemainingPauseTime()
    {
        if (Rules.league.dropInPlayerMode) {
            return null;
        } else if (secGameState == GameControlData.STATE2_NORMAL
                && (gameState == STATE_INITIAL && firstHalf != C_TRUE && !timeOutActive[0] && !timeOutActive[1]
                || gameState == STATE_FINISHED && firstHalf == C_TRUE)) {
            return getRemainingSeconds(whenCurrentGameStateBegan, Rules.league.pauseTime);
        } else if (Rules.league.pausePenaltyShootOutTime != 0 && (gameType == GAME_PLAYOFF) && team[0].score == team[1].score
                && (gameState == STATE_INITIAL && secGameState == STATE2_PENALTYSHOOT && !timeOutActive[0] && !timeOutActive[1]
                || gameState == STATE_FINISHED && firstHalf != C_TRUE)) {
            return getRemainingSeconds(whenCurrentGameStateBegan, Rules.league.pausePenaltyShootOutTime);
        } else {
            return null;
        }
    }
    
    /**
     * Resets the penalize time of all players to 0.
     * This does not unpenalize them.
     */
    public void resetPenaltyTimes()
    {
        for (long[] players : whenPenalized) {
            for (int i = 0; i < players.length; ++i) {
                players[i] = 0;
            }
        }
    }
    
    /**
     * Resets all penalties.
     */
    public void resetPenalties()
    {
        for (int i = 0; i < team.length; ++i) {
            pushes[i] = 0;
            for (int j = 0; j < Rules.league.teamSize; j++) {
                if (!ActionBoard.robot[i][j].isCoach(this) && team[i].player[j].penalty != PlayerInfo.PENALTY_SUBSTITUTE) {
                    team[i].player[j].penalty = PlayerInfo.PENALTY_NONE;
                    if (Rules.league.resetEjectedRobotsOnHalftime) {
                        ejected[i][j] = false;
                    }
                }
                if (Rules.league.resetPenaltyCountOnHalftime) {
                    for (int k = 0; k < Rules.league.penaltyTime.length; k++) {
                        penaltyCount[i][j][k] = 0;
                    }
                }
            }
        }
        resetPenaltyTimes();
        for (int i = 0; i < 2; i++) {
            penaltyQueueForSubPlayers.get(i).clear();
        }
    }
    
    /**
     * Calculates the remaining time a certain robot has to stay penalized.
     * @param side 0 or 1 depending on whether the robot's team is shown left or right.
     * @param number The robot's number starting with 0.
     * @return The number of seconds the robot has to stay penalized.
     */
    public int getRemainingPenaltyTime(int side, int number)
    {
        int penalty = team[side].player[number].penalty;
        int penaltyTime = -1;
        if (penalty != PlayerInfo.PENALTY_MANUAL && penalty != PlayerInfo.PENALTY_SUBSTITUTE) {
//            System.out.println("penalty: " + penalty);
//            System.out.println("penalty count: " + penaltyCount[side][number][penalty]);
            penaltyTime = Rules.league.penaltyTime[penalty][((penaltyCount[side][number][penalty] > Rules.league.penaltyTime[penalty].length)
                    ? Rules.league.penaltyTime[penalty].length
                    : penaltyCount[side][number][penalty]) - 1];
        }
//        System.out.println("penalty time: " + penaltyTime);
        assert penalty == PlayerInfo.PENALTY_MANUAL || penalty == PlayerInfo.PENALTY_SUBSTITUTE || penaltyTime != -1;
        return penalty == PlayerInfo.PENALTY_MANUAL || penalty == PlayerInfo.PENALTY_SUBSTITUTE ? 0
                : gameState == STATE_READY && Rules.league.returnRobotsInGameStoppages && whenPenalized[side][number] >= whenCurrentGameStateBegan
                ? Rules.league.readyTime - getSecondsSince(whenCurrentGameStateBegan)
                : Math.max(0, getRemainingSeconds(whenPenalized[side][number], penaltyTime));
    }
    
    /**
     * Calculates the Number of robots in play (not substitute) on one side
     * @param side 0 or 1 depending on whether the team is shown left or right.
     * @return The number of robots without substitute penalty on the side
     */
    public int getNumberOfRobotsInPlay(int side)
    {
        int count = 0;
        for (int i=0; i<team[side].player.length; i++) {
            if (team[side].player[i].penalty != PlayerInfo.PENALTY_SUBSTITUTE) {
                count++;
            }
        }
        return count;
    }
    
    /**
     * Determines the secondary time. Although this is a GUI feature, the secondary time
     * will also be encoded in the network packet.
     * @param timeKickOffBlockedOvertime In case the kickOffBlocked time is delivered, this
     *                                   parameter specified how long negative values will
     *                                   be returned before the time is switched off.
     * @return The secondary time in seconds or null if there currently is none.
     */
    public Integer getSecondaryTime(int timeKickOffBlockedOvertime)
    {
        if(timeKickOffBlockedOvertime == 0 // preparing data packet
                && secGameState == STATE2_NORMAL && gameState == STATE_PLAYING
                && getSecondsSince(whenCurrentGameStateBegan) < Rules.league.playOffDelayedSwitchToPlaying) {
            return null;
        }
        int timeKickOffBlocked = getRemainingSeconds(whenCurrentGameStateBegan, Rules.league.kickoffTime);
        if (kickOffTeam == DROPBALL) {
            timeKickOffBlocked = 0;
        }
        if (gameState == STATE_INITIAL && (timeOutActive[0] || timeOutActive[1])) {
            return getRemainingSeconds(whenCurrentGameStateBegan, Rules.league.timeOutTime);
        }
        else if (gameState == STATE_INITIAL && (refereeTimeout)) {
            return getRemainingSeconds(whenCurrentGameStateBegan, Rules.league.refereeTimeout);
        }
        else if (gameState == STATE_READY) {
            return getRemainingSeconds(whenCurrentGameStateBegan, Rules.league.readyTime);
        } else if (gameState == STATE_PLAYING && secGameState != STATE2_PENALTYSHOOT
                && timeKickOffBlocked >= -timeKickOffBlockedOvertime) {
            if (timeKickOffBlocked > 0) {
                return timeKickOffBlocked;
            } else {
                return null;
            }
        } else {
            return getRemainingPauseTime();
        }
    }

    /**
     * Dispatch the coach messages. Since coach messages are texts, the messages are zeroed
     * after the first zero character, to avoid the transport of information the
     * GameStateVisualizer would not show.
     */
    public void updateCoachMessages()
    {
        int i = 0;
        while (i < splCoachMessageQueue.size()) {
            if (splCoachMessageQueue.get(i).getRemainingTimeToSend() == 0) {
                for (int j = 0; j < 2; j++) {
                    if (team[j].teamNumber == splCoachMessageQueue.get(i).team) {
                        byte[] message = splCoachMessageQueue.get(i).message;
                        
                        // All chars after the first zero are zeroed, too
                        int k = 0;
                        while (k < message.length && message[k] != 0) {
                            k++;
                        }
                        while (k < message.length) {
                            message[k++] = 0;
                        }
                        
                        team[j].coachSequence = splCoachMessageQueue.get(i).sequence;
                        team[j].coachMessage = message;
                        Log.toFile("Coach Message Team "+  Rules.league.teamColorName[team[j].teamColor]+" "+ new String(message));
                        splCoachMessageQueue.remove(i);
                        break;
                    }
                }
            } else {
                i++;
            }
        }
    }
    
    public void updatePenalties() {
        if (secGameState == STATE2_NORMAL && gameState == STATE_PLAYING
                && getSecondsSince(whenCurrentGameStateBegan) >= Rules.league.playOffDelayedSwitchToPlaying) {
            for (TeamInfo t : team) {
                for (PlayerInfo p : t.player) {
                    if (p.penalty == PlayerInfo.PENALTY_SPL_ILLEGAL_MOTION_IN_SET) {
                        p.penalty = PlayerInfo.PENALTY_NONE;
                    }
                }
            }
        }
    }

    public class PenaltyQueueData  implements Serializable {
        private static final long serialVersionUID = 7536004813202642582L;

        public long whenPenalized;
        public byte penalty;

        public PenaltyQueueData(long whenPenalized, byte penalty) {
            this.whenPenalized = whenPenalized;
            this.penalty = penalty;
        }
    }

    public void addToPenaltyQueue(int side, long whenPenalized, byte penalty) {
        penaltyQueueForSubPlayers.get(side).add(new PenaltyQueueData(whenPenalized, penalty));
    }

}