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
    // Give default values
    team_color = "0";
    player_number = "3";

    // Iterate through the inputed files
    // We skip the first element of argv, as it is the command name
    for (int i =  1; i < argc; ++i) {
        cout << "Reading file" << i << endl;
        // Read in the file
        fstream inputFile, outputFile;
        inputFile.open(argv[i], ios::in);
        stringstream stringContents;
        //stringContents << MCL_LOG_PREFIX << argv[i] << MCL_LOG_SUFFIX;
        string outfileName = "LOGx.mcl";//stringContents.str();
        outputFile.open(outfileName.c_str(), ios::out);

        // Initialize our localization system
        MCL *myLoc = new MCL;

        // process the file line by line
        string newLine;
        vector<Observation>* sightings;

        // Process information from the first line
        getline(inputFile, newLine);
        char delims[] = " ";
        char* firstLine = new char[newLine.size()+1];
        strcpy(firstLine, newLine.c_str());
        team_color = strtok(firstLine, delims);
        player_number = strtok(NULL, delims);

        while(!inputFile.eof()) {
            // Process the next line of the file
            getline(inputFile, newLine);
            processLogLine(newLine, myLoc, &outputFile);
        }

        // Clean up
        inputFile.close();
        outputFile.close();
        delete myLoc;
        delete firstLine;
    }
}

////////////////////////
// FILE I/O           //
////////////////////////

/**
 * Prints the input to a log file to be read by the TOOL
 */
void printOutLogLine(fstream* outputFile, MCL* myLoc, vector<Observation>
                     sightings, MotionModel lastOdo)
{
    // Output particle infos
    vector<Particle> particles = myLoc->getParticles();
    for(unsigned int j = 0; j < particles.size(); ++j) {
        Particle p = particles[j];
        (*outputFile) << p.pose.x << " " << p.pose.y << " " << p.pose.h << " "
                      << p.weight << " ";
    }

    (*outputFile) << ":";

    // Output standard infos
    (*outputFile) << team_color<< " " << player_number << " " <<myLoc->getXEst()
                  << " " << myLoc->getYEst() << " " << myLoc->getHEstDeg()
                  << " " << myLoc->getXUncert() << " " << myLoc->getYUncert()
                  << " " << myLoc->getHUncertDeg() << " " << "0.0"
                  << " " << "0.0" << " " << "0.0" << " " << "0.0"
                  << " " << "0.0" << " " << "0.0" << " " << "0.0"
                  << " " << "0.0" << " " << lastOdo.deltaL
                  << " " << lastOdo.deltaF << " " << lastOdo.deltaR;

    (*outputFile) << ":";

    // Output landmark infos
    for(unsigned int k = 0; k < sightings.size(); ++k) {
        (*outputFile) << sightings[k].getID() << " "
                      << sightings[k].getVisDist() << " "
                      << sightings[k].getVisBearing();
    }
    (*outputFile) << endl;
}

////////////////////////
// CORE FUNCTIONS     //
////////////////////////

/**
 * Method handles the processing of the current log line
 */
void processLogLine(string current, MCL* myLoc, fstream* outputFile)
{
    MotionModel lastOdo;
    lastOdo.deltaL = 0.0;
    lastOdo.deltaF = 0.0;
    lastOdo.deltaR = 0.0;
    vector<Observation> sightings;

    // Setup our string tokenizer
    char delims[] = " ";
    char* newLine = new char[current.size()+1];
    strcpy(newLine, current.c_str());
    char* token = NULL;
    token = strtok(newLine, delims);


    myLoc->updateLocalization(lastOdo, sightings);
    printOutLogLine(outputFile, myLoc, sightings, lastOdo);

    delete newLine;
}
