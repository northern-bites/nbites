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
        fstream inputFile, outputFile;
        inputFile.open(argv[i], ios::in);
        stringstream stringContents;
        stringContents << MCL_LOG_PREFIX << argv[i] << MCL_LOG_SUFFIX;
        string outfileName = stringContents.str();
        outputFile.open(outfileName.c_str(), ios::out);

        // Initialize our localization system
        MCL *myLoc = new MCL;

        // process the file line by line
        string newLine;
        while(!inputFile.eof()) {
            // Process the next line of the file
            getline(inputFile, newLine);
            processLogLine(newLine, myLoc);
            printOutLogLine(outputFile, myLoc);
        }
        inputFile.close();
        outputFile.close();

        // Cleanup
        delete myLoc;
    }
}

////////////////////////
// FILE I/O           //
////////////////////////

/**
 * Prints the input to a log file to be read by the TOOL
 */
void printOutLogLine(fstream outputFile, MCL* myLoc)
{
    // Output particle infos
    vector<Particle> particles = myLoc->getParticles();
    for(unsigned int j = 0; j < particles.size(); ++j) {
        Particle p = particles[j];
        outputfile << p.pose.x << " " << p.pose.y << " " << p.pose.h << " " <<
            p.weight << " ";
    }

    fstream << ":";

    // Output standard infos
    fstream << team_color << " " << player_number << " " << myLoc->getXEst() <<
        myLoc->getYEst() << " " << myLoc->getHEstDeg() << " " <<
        myLoc->getXUncert() << " " << myLoc->getYUncert() << " " <<
        myLoc->getHUncertDeg() << "0.0" << " " << "0.0" << " " << "0.0" <<
        " " << "0.0" << " " << "0.0" << " " << "0.0" << " " << "0.0" << " " <<
        "0.0" << " " << lastOdo.x << " " << lastOdo.y << " " << lastOdo.h;

    fstream << ":";

    // Output landmark infos
}

////////////////////////
// CORE FUNCTIONS     //
////////////////////////

/**
 * Method handles the processing of the current log line
 */
void processLogLine(string current, MCL* myLoc)
{
    MotionModel lastOdo;
    vector<Observation> sightings;
    myLoc->updateLocalization(lastOdo, sightings);
}
