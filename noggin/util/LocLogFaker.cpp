#include "LocLogFaker.h"

int main()
{
    // TODO: Make this read a text file
    // Make navPath
    NavPath letsGo;
    letsGo.startPos = PoseEst(100.1f,100.2f,-0.03f);
    letsGo.myMoves.push_back(NavMove(MotionModel(2.0f,2.0f,-0.3f),
                                     20));
    letsGo.myMoves.push_back(NavMove(MotionModel(2.0f,2.0f,0.5f),
                                     10));
    letsGo.myMoves.push_back(NavMove(MotionModel(0.0f,0.0f,0.25f),
                                     10));

    // Information needed for the main method
    PoseEst currentPose = letsGo.startPos;
    MCL *myLoc = new MCL;
    vector<Observation> Z_t;
    team_color = "0";
    player_number = "3";

    // Setup output file
    fstream outputFile;
    string outfileName = "LOGx.mcl";
    outputFile.open(outfileName.c_str(), ios::out);

    // Iterate through the moves
    for(unsigned int i = 0; i < letsGo.myMoves.size(); ++i) {
        // Continue the move for as long as specified
        for (int j = 0; j < letsGo.myMoves[i].time; ++j) {
            currentPose += letsGo.myMoves[i].move;
            Z_t = determineObservedLandmarks(currentPose,0.0);
            myLoc->updateLocalization(letsGo.myMoves[i].move,Z_t);
            printOutLogLine(&outputFile, myLoc, Z_t, letsGo.myMoves[i].move);
        }
    }

    return 0;
}

vector<Observation> determineObservedLandmarks(PoseEst myPos, float neckYaw)
{
    vector<Observation> Z_t;
    // Get half of the nao FOV converted to radians
    float FOV_OFFSET = NAO_FOV_X_DEG * M_PI / 360.0f;

    // Check concrete field objects
    // required measurements for the added observation
    float visDist, visBearing;
    for(int i = 0; i < ConcreteFieldObject::NUM_FIELD_OBJECTS; ++i) {
       const ConcreteFieldObject* toView = ConcreteFieldObject::
           concreteFieldObjectList[i];
        float deltaX = toView->getFieldX() - myPos.x;
        float deltaY = toView->getFieldY() - myPos.y;
        visDist = hypot(deltaX, deltaY);
        visBearing = subPIAngle(atan2(deltaY, deltaX) - myPos.h
                                - (M_PI / 2.0f));
        // cout << "Looking at landmark " << toView->getID() << " at angle of "
        //          << (visBearing * 180.0f / M_PI) << endl;

        if (visBearing > -FOV_OFFSET && visBearing < FOV_OFFSET) {
            cout << "Observed landmark: " << toView->getID() << endl;
        }
    }

    // Check concrete corners

    // Check concrete lines

    return Z_t;
}

////////////////////////
// File I/O           //
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
        (*outputFile) << p << " ";
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



// NavMove
// Constructors
NavMove::NavMove(MotionModel _p, int _t) : move(_p), time(_t)
{
};

float subPIAngle(float theta)
{
    while( theta > M_PI) {
        theta -= 2.0f*M_PI;
    }

    while( theta < -M_PI) {
        theta += 2.0f*M_PI;
    }
    return theta;
}
