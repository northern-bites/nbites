package analyzer;

import java.io.File;
import java.io.FileWriter;
import java.util.LinkedList;

/**
 * @author Michel Bartsch
 * 
 * The log-analyzer-program starts in this class.
 * The main components are initialized here.
 */
public class LogAnalyzer
{
    /* Path the the log files to analyze. */
    public final static String PATH = "logs";
    /* Path where dropped logs will be moved to by the clean feature. */
    public final static String PATH_DROPPED = "logs/dropped";
    
    /* List of all logs */
    public static LinkedList<LogInfo> logs;
    /* The output file to write the statistics into.*/
    public static File stats;
    /* Use this to write into the output file. */
    public static FileWriter writer;
    
    /**
     * The program starts here.
     * 
     * @param args  This is ignored.
     */
    public static void main(String[] args)
    {
        load();
        new GUI();
    }
    
    /**
     * Loads all the logs, can be used at the beginning and to update
     * the list as well.
     */
    public static void load()
    {
        logs = new LinkedList<LogInfo>();
        File dir = new File(PATH);
        File[] files = dir.listFiles();
        for (File file: files) {
            if (file.isFile() && file.getName().startsWith("log_")) {
                logs.add(new LogInfo(file));
            }
        }
    }
}