package data;

import java.awt.Color;


/**
 * @author Michel Bartsch
 * 
 * This class holds attributes defining rules.
 */
public abstract class Rules
{   
    /** Note all league´s rules here to have them available. */
    public static final Rules[] LEAGUES = {
        new SPL(),
        new SPLDropIn(),
        new HL(),
        new HLTeen(),
        new HLAdult()
    };
    
    /** The rules of the league playing. */
    public static Rules league = LEAGUES[0];
    /** The league´s name this rules are for. */
    public String leagueName;
    /** The league´s directory name with it´s teams and icons. */
    public String leagueDirectory;
    /** How many robots are in a team. */
    public int teamSize;
    /** How many robots of each team may play at one time. */
    public int robotsPlaying;
    /** The Java Colors the left and the right team starts with. */
    public Color[] teamColor;
    /** The name of the colors. */
    public String[] teamColorName;
    /** If the colors change automatically. */
    public boolean colorChangeAuto;
    /** If the clock may stop in certain states (Ready, Set) in a play-off game. */
    public boolean playOffTimeStop;
    /** Time in seconds one half is long. */
    public int halfTime;
    /** Time in seconds the ready state is long. */
    public int readyTime;
    /** Time in seconds between first and second half. */
    public int pauseTime;
    /** If left and right side may both have the first kickoff. */
    public boolean kickoffChoice;
    /** Time in seconds the ball is blocked after kickoff. */
    public int kickoffTime;
    /** Time in seconds before a global game stuck can be called. */
    public int minDurationBeforeStuck;
    /** The number of seconds switching to Playing is delayed in a play-off game. */
    public int playOffDelayedSwitchToPlaying;
    /** If there is an overtime before the penalty shoot-out in a play-off game. */
    public boolean overtime;
    /** Time in seconds one overtime half is long. */
    public int overtimeTime;
    /** If the game starts with penalty-shots. */
    public boolean startWithPenalty;
    /** Time in seconds between second half and penalty shoot-out. */
    public int pausePenaltyShootOutTime;
    /** Time in seconds one penalty shoot is long. */
    public int penaltyShotTime;
    /** If there can be a penalty-shot retry. */
    public boolean penaltyShotRetries;
    /** If there is a sudden-death. */
    public boolean suddenDeath;
    /** Time in seconds one penalty shoot is long in sudden-death. */
    public int penaltyShotTimeSuddenDeath;
    /** Number of penalty-shots for each team when a half has 10minutes. */
    public int numberOfPenaltyShotsShort;
    /** Number of penalty-shots for each team after full 10minutes playing. */
    public int numberOfPenaltyShotsLong;
    /** Time in seconds for each kind of penalty. */
    public int[] penaltyTime;
    /** if robots should return from penalties when the game state changes. */
    public boolean returnRobotsInGameStoppages;
    /** Time in seconds one team has as timeOut. */
    public int timeOutTime;
    /** Time in seconds of a referee timeout*/
    public int refereeTimeout;
    /** Defines if the option for a referee timeout is available. */ 
    public boolean isRefereeTimeoutAvailable;
    /** One time-out per half? */
    public boolean timeOutPerHalf;
    /** On how many pushings is a robot ejected. */
    public int[] pushesToEjection;
    /** Defines if coach is available **/
    public boolean isCoachAvailable;
    /** Allowed to compensate for lost time? */
    public boolean lostTime;
    /** Whether compatibility mode (version 7) is supported **/
    public boolean compatibilityToVersion7;
    /** If true, the drop-in player competition is active*/
    public boolean dropInPlayerMode;
}