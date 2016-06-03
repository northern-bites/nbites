package data;

import java.awt.Color;

/**
 *
 * @author Michel-Zen
 * 
 * This class sets attributes given by the spl rules.
 */
public class SPL extends Rules
{
    SPL()
    {
        /** The league´s name this rules are for. */
        leagueName = "SPL";
        /** The league´s directory name with it´s teams and icons. */
        leagueDirectory = "spl";
        /** How many robots are in a team. */
        teamSize = 6; // 5 players + 1 sub
        /** How many robots of each team may play at one time. */
        robotsPlaying = 5;
        /** The Java Colors the left and the right team starts with. */
        teamColor = new Color[] {Color.BLUE, Color.RED, new Color(224, 200, 0), Color.BLACK};
        /** The name of the colors. */
        teamColorName = new String[] {"Blue", "Red", "Yellow", "Black"};
        /** If the colors change automatically. */
        colorChangeAuto = false;
        /** If the clock may stop in certain states (Ready, Set) in a play-off game. */
        playOffTimeStop = true;
        /** Time in seconds one half is long. */
        halfTime = 10*60;
        /** Time in seconds the ready state is long. */
        readyTime = 45;
        /** Time in seconds between first and second half. */
        pauseTime = 10*60;
        /** If left and right side may both have the first kickoff. */
        kickoffChoice = false;
        /** Time in seconds the ball is blocked after kickoff. */
        kickoffTime = 10;
        /** Time in seconds before a global game stuck can be called. */
        minDurationBeforeStuck = 15;
        /** The number of seconds switching to Playing is delayed in a play-off game. */
        playOffDelayedSwitchToPlaying = 15;
        /** If there is an overtime before penalty-shoot in a play-off game. */
        overtime = false;
        /** Time in seconds one overtime half is long. */
        overtimeTime = 0;
        /** If the game starts with penalty-shoots. */
        startWithPenalty = false;
       /** Time in seconds between second half and penalty shoot. */
        pausePenaltyShootOutTime = 5*60;
        /** Time in seconds one penalty shoot is long. */
        penaltyShotTime = 1*60;
        /** If there can be a penalty-shoot retry. */
        penaltyShotRetries = false;
        /** If there is a sudden-death. */
        suddenDeath = true;
        /** Time in seconds one penalty shoot is long in sudden-death. */
        penaltyShotTimeSuddenDeath = 2*60;
        /** Number of penalty-shoots for each team when a half has 10minutes. */
        numberOfPenaltyShotsShort = 3;
        /** Number of penalty-shoots for each team after full 10minutes playing. */
        numberOfPenaltyShotsLong = 5;
        /** Time in seconds for each kind of penalty (-1 = should not be used). */
        penaltyTime = new int[][] {{-1}, {45, 90, 135, 2 * halfTime}, {45, 90, 135, 2 * halfTime}, {0}, {45, 90, 135, 2 * halfTime}, {45, 90, 135, 2 * halfTime}, {45, 90, 135, 2 * halfTime}, {45, 90, 135, 2 * halfTime}, {45}, {2 * halfTime}};
        /** Whether the penalty count is reset on halftime */
        resetPenaltyCountOnHalftime = true;
        /** Whether the ejected robots are reset on halftime */
        resetEjectedRobotsOnHalftime = true;
        /** Whether penalties can be removed before the penalty time has passed. */
        allowEarlyPenaltyRemoval = false;
        /** Penalty that players get when they substitute another player. */
        substitutePenalty = PlayerInfo.PENALTY_SPL_REQUEST_FOR_PICKUP;
        /** if robots should return from penalties when the game state changes. */
        returnRobotsInGameStoppages = true;
        /** Time in seconds one team has as timeOut. */
        timeOutTime = 5*60;
        /** Time in seconds of a referee timeout */
        refereeTimeout = 10*60;
        /** Defines if the option for a referee timeout is available */
        isRefereeTimeoutAvailable = true;
        /** One time-out per half? */
        timeOutPerHalf = false;
        /** On how many pushings is a robot ejected. */
        pushesToEjection = new int[] {4, 6, 8, 10, 12};
        /** Defines if coach is available */
        isCoachAvailable = true;
        /** Allowed to compensate for lost time? */
        lostTime = true;
        /** Whether compatibility mode (version 7) is supported */
        compatibilityToVersion7 = false;
        /** If true, the drop-in player competition is active */
        dropInPlayerMode = false;
        /** If true, the game controller should drop broadcast-messages */
        dropBroadcastMessages = true;
    }
}