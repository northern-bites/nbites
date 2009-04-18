/* obsToNac.cpp */
#include "fakerIO.h"
#include "fakerIterators.h"
#include "NBMath.h"
#define UNIFORM_1_NEG_1 (2*(rand() / (float(RAND_MAX)+1)) - 1)
using namespace std;
using namespace boost;
using namespace NBMath;

static vector<PoseEst> realPoses;
static vector<BallPose> ballPoses;
static vector<MotionModel> odos;
static vector<vector<Observation> > sightings;
static vector<float> ballDists;
static vector<float> ballBearings;

void runMCL(char * base, string name, int numParticles, bool useBest);

int main(int argc, char** argv)
{
    // Information needed for the main method
    // IO Variables
    fstream obsFile;
    fstream ekfFile;
    fstream ekfCoreFile;

    /* Test for the correct number of CLI arguments */
    if(argc != 2) {
        cerr << "usage: " << argv[0] << " input-file" << endl;
        return 1;
    }
    try {
        obsFile.open(argv[1], ios::in);

    } catch (const exception& e) {
        cout << "Failed to open input file" << argv[1] << endl;
        return 1;
    }

    cout << "Reading in file" << endl;
    readObsInputFile(&obsFile, &realPoses, &ballPoses,
                     &odos, &sightings, &ballDists,
                     &ballBearings, BALL_ID);
    // Close the input file
    obsFile.close();

    // EKF files
    string ekfFileName(argv[1]);
    string ekfCoreFileName(argv[1]);
    ekfFileName.replace(ekfFileName.end()-3, ekfFileName.end(), "ekf");
    ekfFile.open(ekfFileName.c_str(), ios::out);
    ekfCoreFileName.replace(ekfCoreFileName.end()-3,
                            ekfCoreFileName.end(), "ekf.core");
    ekfCoreFile.open(ekfCoreFileName.c_str(), ios::out);

    // Create the EKF System
    shared_ptr<LocEKF> ekfLoc = shared_ptr<LocEKF>(new LocEKF());
    // Iterate through the path
    cout << "Running EKF loc" << endl;
    iterateObsPath(&ekfFile, &ekfCoreFile,
                   ekfLoc, &realPoses, &ballPoses, &odos,
                   &sightings, &ballDists, &ballBearings, BALL_ID);
    ekfFile.close();
    ekfCoreFile.close();

    runMCL(argv[1], "5", 5, false);
    runMCL(argv[1], "5.best", 5, true);
    runMCL(argv[1], "50", 50, false);
    runMCL(argv[1], "50.best", 50, true);
    runMCL(argv[1], "100", 100, false);
    runMCL(argv[1], "100.best", 100, true);
    runMCL(argv[1], "500", 500, false);
    runMCL(argv[1], "500.best", 500, true);
    runMCL(argv[1], "1000", 1000, false);
    runMCL(argv[1], "1000.best", 1000, true);

    return 0;
}

void runMCL(char * base, string name, int numParticles, bool useBest)
{
    fstream mclFile;
    fstream mclCoreFile;

    // Setup the output files
    string mclFileName(base);
    string mclCoreFileName(base);
    mclFileName.replace(mclFileName.end()-3, mclFileName.end(), "mcl.");
    mclFileName += name;
    mclFile.open(mclFileName.c_str(), ios::out);
    mclCoreFileName.replace(mclCoreFileName.end()-3,
                            mclCoreFileName.end(), "mcl.core.");
    mclCoreFileName += name;
    mclCoreFile.open(mclCoreFileName.c_str(), ios::out);

    shared_ptr<MCL> mcl = shared_ptr<MCL>(new MCL(numParticles));
    mcl->setUseBest(useBest);

    // Iterate through the path
    cout << "Running MCL loc with " << numParticles << " particles";
    if (useBest) {
        cout << " using best particle.";
    }
    cout << endl;

    iterateMCLObsPath(&mclFile, &mclCoreFile,
                      mcl, &realPoses, &ballPoses, &odos,
                      &sightings, &ballDists, &ballBearings, BALL_ID);
    mclFile.close();
    mclCoreFile.close();
}
