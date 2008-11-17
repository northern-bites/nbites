/**
 * MCLLogger.cpp
 *
 * @author Tucker Hermans
 * @date 17.11.08
 * @version %I%
 */
#include "MCLLogger.h"

/**
 * Control method to control the file readin, processing, and output control
 */

int main(int argc, char* argv[])
{
    // Iterate through the inputed files
    // We skip the first element of argv, as it is the command name
    for (int i =  1; i < argc; ++i) {
        // Read in the file
        fstream inputFile;
        inputFile.open(argv[i], ios::in);

        // Initialize our localization system
        MCL *myLoc = new MCL;

        // process the file line by line
        string newLine;
        while(!inputFile.eof()) {
            getline(inputFile, newLine);

            // Process the lines of the file
            processLogLine(newLine, myLoc);
        }
        // Print the log to file
        printMCLLog(i);
    }
}

////////////////////////
// FILE I/O           //
////////////////////////

/**
 * Read in the raw data file
 */
void readRawFile(const string fileName)
{
}

/**
 * Prints the input to a log file to be read by the TOOL
 */
void printMCLLog(int logNum)
{
    //sstream fileName =  MCL_LOG_PREFIX << logNum << MCL_LOG_SUFFIX;
}

////////////////////////
// CORE FUNCTIONS     //
////////////////////////

/**
 * Method handles the processing of the current log line
 */
void processLogLine(string current, MCL* myLoc)
{
}
