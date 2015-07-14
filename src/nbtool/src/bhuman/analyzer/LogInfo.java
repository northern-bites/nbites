package analyzer;

import common.Log;
import data.Rules;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Date;
import java.util.LinkedList;

/**
 * @author Michel Bartsch
 * 
 * Instances of this class represent a log file. It can analyze some basic
 * information of it´s log to guess, if it was made by a real game.
 */
public class LogInfo
{
    /* A log should be written in this charset. */
    private final static String CHARSET = "UTF-8";
    /* A team´s name which indicates, that this is not a real game´s log. */
    private final static String NOT_A_REAL_TEAM = "Invisibles";
    /* Minimal duration in seconds between the first ready and the last
     Finish a real game should have.*/
    private final static int MIN_DURATION = 18*60;
    /* Number of log entries with general information at the beginning of
     * every log, used to count the number of real actions. */
    private final static int NUM_OF_INFO_ENTRIES = 6;
    
    /* The log file this instance belongs to. */
    public File file;
    /* The version information found in the log. */
    public String version;
    /* The league this log´s game was layed in. */
    public Rules league;
    /* If the teams have kept there team colors during the game. */
    public boolean keepColors = false;
    /* The teams's names. */
    public String[] team = new String[2];
    /* The teams's jersey colors. */
    public String[] color = new String[2];
    /* Time of the first ready. */
    public Date start;
    /* Duration of the game in seconds (first ready to last finish) */
    public int duration;
    /* All lines from the log file. */
    public LinkedList<String> lines = new LinkedList<String>();
    /* If something odd happens while parsing this log, it will be written
     * into this string. */
    public String parseErrors = "";
    
    
    /**
     * Creates a new LogInfo.
     * It will read the whole log into a list of strings and launch a parsing
     * method to collect some basic information.
     * 
     * @param log   The log file to read and represent.
     */
    public LogInfo(File log)
    {
        file = log;
        BufferedReader br = null;
        try {
            InputStream inStream = new FileInputStream(log);
            br = new BufferedReader(new InputStreamReader(inStream, CHARSET));
            String currentLine;
            while ((currentLine = br.readLine()) != null) {
                lines.add(currentLine);
            }
        } catch (IOException e) {
            Log.error("cannot load "+log);
        }
        finally {
            if (br != null) {
                try {
                    br.close();
                } catch (Exception e) {}
            }
        }
        Parser.info(this);
    }
    
    /**
     * Makes a guess if this is a real game´s log based on all basic
     * information.
     * 
     * @return  True, if this looks like a real game´s log or false, if not.
     */
    public boolean isRealLog()
    {
        return isRealVersion()
                && isRealLeague()
                && isRealTeam(true)
                && isRealTeam(false)
                && isRealDuration();
    }
    
    /**
     * Makes a guess if this is a real game´s log based on it´s version.
     * 
     * @return  True, if this looks like a real game´s log or false, if not.
     */
    private boolean isRealVersion()
    {
        return version == null ? false : version.equals(controller.GameController.version);
    }
    
    /**
     * Makes a guess if this is a real game´s log based on it´s league.
     * 
     * @return  True, if this looks like a real game´s log or false, if not.
     */
    private boolean isRealLeague()
    {
        return league != null;
    }
    
    /**
     * Makes a guess if this is a real game´s log based on a team name.
     * 
     * @param firstTeam     True, if you want to check the first team´s name,
     *                      or false for the second.
     * @return      True, if this looks like a real game´s log or false, if not.
     */
    private boolean isRealTeam(boolean firstTeam)
    {
        return team[firstTeam ? 0 : 1] == null ? false : !team[firstTeam ? 0 : 1].equals(NOT_A_REAL_TEAM);
    }
    
    /**
     * Makes a guess if this is a real game´s log based on it´s duration.
     * 
     * @return  True, if this looks like a real game´s log or false, if not.
     */
    private boolean isRealDuration()
    {
        return duration > MIN_DURATION;
    }
    
    /**
     * This method writes all basic information into a string.
     * 
     * @return  Nice looking string with all basic information.
     */
    public String getInfo()
    {
        return GUI.HTML
                + (isRealVersion() ? version : GUI.HTML_RED + version + GUI.HTML_END) + GUI.HTML_LF
                + (isRealLeague() ? league.leagueName : GUI.HTML_RED + league + GUI.HTML_END) + GUI.HTML_LF
                + (isRealTeam(true) ? team[0] : GUI.HTML_RED + team[0] + GUI.HTML_END)
                + " vs "
                + (isRealTeam(false) ? team[1] : GUI.HTML_RED + team[1] + GUI.HTML_END) + GUI.HTML_LF
                + (keepColors ? "No Color Change" : "Color Change") + GUI.HTML_LF
                + (start != null ? start : GUI.HTML_RED + start + GUI.HTML_END) + " starting" + GUI.HTML_LF
                + (isRealDuration() ? duration : GUI.HTML_RED + duration + GUI.HTML_END) + " seconds" + GUI.HTML_LF
                + (lines.size()-NUM_OF_INFO_ENTRIES) + " actions" + GUI.HTML_LF
                + GUI.HTML_RED + parseErrors;
    }
    
    @Override
    public String toString()
    {
        return GUI.HTML + (isRealLog() ? team[0] + " vs " + team[1] : GUI.HTML_RED + team[0] + " vs " + team[1] + GUI.HTML_END);
    }
}