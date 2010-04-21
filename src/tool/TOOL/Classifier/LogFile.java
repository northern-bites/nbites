package TOOL.Classifier;

import java.io.*;
import java.util.regex.*;

/**
 * @author Harrison Chapman
 * A system to manage the log files for the vision human classifier.
 * The objects return their logfile data with the method String toLog()
 * and (when implemented) interpret data in void fromLog() (or something)
 * The format of a log file is as follows:
 *
 *
 * int numFrames, String robotName
 * 
 * FILENUM.FRM {
 * [Ball int occluded (int id, int upperLeftCornerX, int upperLeftCornerY,
 * 					  int width, int height)]
 * [Goal int occluded (int id, int x1, int y1, int x2, int y2, int x3, int y3, 
 *      			  int x4, int y4)]
 * [Beacon int occluded (int id, int x1, int y1, int x2, int y2, int x3, int y3, 
 *      				int x4, int y4)]
 * [Line int occluded (int id, int x1, int y1, int x1, int x2)]
 * [CenterCircle int occluded (int id, int upperLeftCornerX, int upperLeftCornerY, 
 *        					  int width, int height)]
 * [Corner int occluded (int type (L or T), int id, int x1, int y1, int x1, int x2)]
 * [CornerArc int occluded (int id, int endpoint1.x, int endpoint1.y, int endpoint2.x,
 * 						   int endpoint2.y, int radius.x, int radius.y)]}
 * 
 * and so on and so forth
 * 
 * Nick: "Whenever the system is expanded (for instance, to allow classifying of
 * robots maybe?) you should ALWAYS add the logging information to the END.
 * That way we can easily make the conversion later on, both in C and in Java."
 * 
 * 
 * --- Right now, this only deals with the writing of a log file.
 */

public class LogFile {
    private RandomAccessFile logFile;
	
    public LogFile (String fName)
    {
        try {
            logFile = new RandomAccessFile(fName, "rw");
        } catch (Exception e) {
            System.err.println(e);
            System.exit(1);
        }
    }
	
    public void writeLogFile (ClassifierObject[][] log)
    {
    	for (int i = 0; i < log.length; i++)
            for (int j = 0; j < log[i].length; j++)
                if( log[i][j] != null ) System.out.println(log[i][j].toLog());
    }
    
    public void readLogFile ()
    {
        // Do stuff
    }
    
}
