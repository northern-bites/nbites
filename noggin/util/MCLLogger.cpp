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
// ./mclLogger ~/Documents/RoboCup/LocLogs/oldies/LOC1.LOG
int main(int argc, char* argv[])
{
    // Give default values
    team_color = "0";
    player_number = "3";

    // Iterate through the inputed files
    // We skip the first element of argv, as it is the command name
    for (int i =  1; i < argc; ++i) {
        //cout << "Reading file" << i << endl;
        // Read in the file
        fstream inputFile, outputFile;
        inputFile.open(argv[i], ios::in);
        string outfileName = "LOGx.mcl";//stringContents.str();
        outputFile.open(outfileName.c_str(), ios::out);

        // Initialize our localization system
        MCL *myLoc = new MCL;

        // process the file line by line
        string newLine;

        // Process information from the first line
        getline(inputFile, newLine);
        boost::char_separator<char> sep(" ");
        tokenizer tokens(newLine, sep);
        tokenizer::iterator tok_iter = tokens.begin();
        team_color = *tok_iter;
        player_number = *(++tok_iter);

        int k = 0;
        vector<Observation> v;
        MotionModel l;
        printOutLogLine(&outputFile, myLoc, v, l);
        while(!inputFile.eof()) {
            ++k;
            // Process the next line of the file
            getline(inputFile, newLine);
            // Ignore blank lines
            if(newLine == "") {
                continue;
            }
            cout << "Line number: " << k << endl;
            processLogLine(newLine, myLoc, &outputFile);
        }

        // Clean up
        inputFile.close();
        outputFile.close();
        delete myLoc;
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

    // Divide the sections with a colon
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

    // Divide the sections with a colon
    (*outputFile) << ":";

    // Output landmark infos
    for(unsigned int k = 0; k < sightings.size(); ++k) {
        (*outputFile) << sightings[k].getID() << " "
                      << sightings[k].getVisDist() << " "
                      << sightings[k].getVisBearing() << " ";
    }

    // Close the line
    (*outputFile) << endl;
}

////////////////////////
// CORE FUNCTIONS     //
////////////////////////

/**
 * Method handles the processing of the current log line
 *
 * @param current The line to be processed.
 * @param myLoc A pointer to the current MCL instance
 * @param outputFile A point to the current output file
 */
void processLogLine(string current, MCL* myLoc, fstream* outputFile)
{
    MotionModel lastOdo;
    vector<Observation> sightings;
    float tempDist;
    float tempBearing;

    // Setup our string tokenizer
    boost::char_separator<char> sep(" ");
    tokenizer tokens(current, sep);
    tokenizer::iterator tok_iter = tokens.begin();
    string time = *tok_iter;
    string frame = *(++tok_iter);
    lastOdo.deltaL = lexical_cast<float>(*(++tok_iter));
    lastOdo.deltaF = lexical_cast<float>(*(++tok_iter));
    lastOdo.deltaR = lexical_cast<float>(*(++tok_iter)) * DEG_TO_RAD;
    // cout << "deltaF " << lastOdo.deltaF << " deltaL: " << lastOdo.deltaL
    //      <<  " deltaR: " << lastOdo.deltaR << endl;

    // Skip the BY and YB data
    ++tok_iter;
    ++tok_iter;
    ++tok_iter;
    ++tok_iter;

    // YGLP
    tempDist = lexical_cast<float>(*(++tok_iter));
    tempBearing = lexical_cast<float>(*(++tok_iter));
    FieldObjects * yglp = new FieldObjects(YELLOW_GOAL_LEFT_POST);
    if (tempDist > 0.) {
        yglp->setDist(tempDist);
        yglp->setBearing(DEG_TO_RAD*tempBearing);
        sightings.push_back(*(new Observation(*yglp)));
        if (DEBUG_INPUT) {
            cout << "YGLP Dist:" << tempDist << " Bearing: " << tempBearing
                 << " ID : " << yglp->getID() << endl;
        }
    }

    // YGRP
    tempDist = lexical_cast<float>(*(++tok_iter));
    tempBearing = lexical_cast<float>(*(++tok_iter));
    FieldObjects * ygrp = new FieldObjects(YELLOW_GOAL_RIGHT_POST);
    if (tempDist > 0.) {
        ygrp->setDist(tempDist);
        ygrp->setBearing(DEG_TO_RAD*tempBearing);
        sightings.push_back(*(new Observation(*ygrp)));
        if (DEBUG_INPUT) {
            cout << "YGRP Dist:" << tempDist << " Bearing: " << tempBearing
                 <<  " ID : " << ygrp->getID() << endl;
        }
    }
    // BGLP
    tempDist = lexical_cast<float>(*(++tok_iter));
    tempBearing = lexical_cast<float>(*(++tok_iter));
    FieldObjects * bglp = new FieldObjects(BLUE_GOAL_LEFT_POST);
    if (tempDist > 0.) {
        bglp->setDist(tempDist);
        bglp->setBearing(DEG_TO_RAD*tempBearing);
        sightings.push_back(*(new Observation(*bglp)));
        if (DEBUG_INPUT) {
            cout << "BGLP Dist:" << tempDist << " Bearing: " << tempBearing
                 << " ID : " << bglp->getID() << endl;
        }
    }

    // BGRP
    tempDist = lexical_cast<float>(*(++tok_iter));
    tempBearing = lexical_cast<float>(*(++tok_iter));
    FieldObjects * bgrp = new FieldObjects(BLUE_GOAL_RIGHT_POST);
    if (tempDist > 0.) {
        bgrp->setDist(tempDist);
        bgrp->setBearing(DEG_TO_RAD*tempBearing);
        sightings.push_back(*(new Observation(*bgrp)));
        if (DEBUG_INPUT) {
            cout << "BGRP Dist:" << tempDist << " Bearing: " << tempBearing
                 << " ID : " << bgrp->getID() << endl;
        }

    }

    // Ball
    tempDist = lexical_cast<float>(*(++tok_iter));
    tempBearing = lexical_cast<float>(*(++tok_iter));
    if (DEBUG_INPUT  && tempDist > 0) {
        cout << "Ball Dist:" << tempDist << " Bearing: " << tempBearing << endl;
    }

    // Pwm...
    ++tok_iter; // Skip, AIBO specific

    // Head Yaw...
    ++tok_iter; // Skip, AIBO specific

    // Corners...
    ++tok_iter;
    int nextX, nextY;
    while(tok_iter != tokens.end()) {
        if((*tok_iter).compare("c") == 0) {
            tempDist = lexical_cast<float>(*(++tok_iter));
            tempBearing = lexical_cast<float>(*(++tok_iter));
            nextX = lexical_cast<int>(*(++tok_iter));
            nextY = lexical_cast<int>(*(++tok_iter));
            if (DEBUG_INPUT_CORNERS) {
                cout << "c dist: " << tempDist << " bearing: " << tempBearing
                     << " x: " << nextX << " y: " << nextY << endl;
            }
        } else if((*tok_iter).compare("a") == 0) {
            tempDist = lexical_cast<float>(*(++tok_iter));
            tempBearing = lexical_cast<float>(*(++tok_iter));
            if (DEBUG_INPUT_CORNERS) {
                cout << "a dist: " << tempDist << " bearing: " << tempBearing;
            }
            while(++tok_iter != tokens.end() &&
                  (*tok_iter != "a" &&  *tok_iter != "c")) {
                nextX = lexical_cast<int>(*(tok_iter));
                nextY = lexical_cast<int>(*(++tok_iter));
                if (DEBUG_INPUT_CORNERS) {
                    cout << " x: " << nextX << " y: " << nextY;
                }
            }
            if (DEBUG_INPUT_CORNERS) {
                cout << endl;
            }
        } else {
            ++tok_iter;
        }
    }

    myLoc->updateLocalization(lastOdo, sightings);
    printOutLogLine(outputFile, myLoc, sightings, lastOdo);

    // Clean up
    delete yglp;
    delete ygrp;
    delete bglp;
    delete bgrp;
}
