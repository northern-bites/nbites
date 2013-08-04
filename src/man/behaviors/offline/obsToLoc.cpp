/* obsToNac.cpp */
#include "fakerIO.h"
#include "fakerIterators.h"
#include "NBMath.h"
#include "Common.h"
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
    fstream ekfFile;
    fstream ekfCoreFile;
    string ekfFileName(argv[1]);
    string ekfCoreFileName(argv[1]);
    ekfFileName.replace(ekfFileName.end()-3, ekfFileName.end(), "ekf");
    ekfFile.open(ekfFileName.c_str(), ios::out);
    ekfCoreFileName.replace(ekfCoreFileName.end()-3,
                            ekfCoreFileName.end(), "ekf.core");
    ekfCoreFile.open(ekfCoreFileName.c_str(), ios::out);

    // Create the EKF System
    shared_ptr<LocSystem> locSys = shared_ptr<MMLocEKF>(new MMLocEKF());
    // Iterate through the path
    cout << "Running EKF loc" << endl;
    long long ekfTime = -micro_time();
    ekfLoc->setUseAmbiguous(true);
    iterateObsPath(&ekfFile, &ekfCoreFile,
                   locSys, &realPoses, &ballPoses, &odos,
                   &sightings, &ballDists, &ballBearings, BALL_ID);
    ekfTime += micro_time();
    ekfTime *= 0.001;
    cout << "EKF time was " << ekfTime << endl;
    ekfFile.close();
    ekfCoreFile.close();

    // EKF no ambiguous files
    fstream ekfNoAmbigFile;
    fstream ekfNoAmbigCoreFile;
    string ekfNoAmbigFileName(argv[1]);
    string ekfNoAmbigCoreFileName(argv[1]);

    ekfNoAmbigFileName.replace(ekfNoAmbigFileName.end()-3,
                               ekfNoAmbigFileName.end(),
                               "ekf.na");
    ekfNoAmbigFile.open(ekfNoAmbigFileName.c_str(), ios::out);
    ekfNoAmbigCoreFileName.replace(ekfNoAmbigCoreFileName.end()-3,
                                   ekfNoAmbigCoreFileName.end(),
                                   "ekf.core.na");
    ekfNoAmbigCoreFile.open(ekfNoAmbigCoreFileName.c_str(), ios::out);

    // Create the EKF no ambiguous data system
    shared_ptr<LocEKF> ekfNoAmbigLoc = shared_ptr<LocEKF>(new LocEKF());
    ekfNoAmbigLoc->setUseAmbiguous(false);
    // Iterate through the path
    cout << "Running EKF loc ignoring ambiguous data" << endl;
    long long ekfNATime = -micro_time();
    iterateObsPath(&ekfNoAmbigFile, &ekfNoAmbigCoreFile,
                   ekfLoc, &realPoses, &ballPoses, &odos,
                   &sightings, &ballDists, &ballBearings, BALL_ID);
    ekfNATime += micro_time();
    ekfNATime *= 0.001;
    cout << "EKF no ambiguous time was " << ekfNATime << endl;
    ekfNoAmbigFile.close();
    ekfNoAmbigCoreFile.close();

    int sampleSizes[] = {5,10,50,100,500,1000};
    for (int x = 0; x < 6; ++x) {
        for (int i = 1; i <= 10; ++i) {
            stringstream st;
            st << sampleSizes[x] << "-" << i;
            runMCL(argv[1], st.str(), sampleSizes[x], false);
            st << ".best";
            runMCL(argv[1], st.str(), sampleSizes[x], true);
        }
    }
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
    cout << "\tThe file is " << mclFileName << endl;

    long long mclTime = -micro_time();
    iterateMCLObsPath(&mclFile, &mclCoreFile,
                      mcl, &realPoses, &ballPoses, &odos,
                      &sightings, &ballDists, &ballBearings, BALL_ID);
    mclTime += micro_time();
    mclTime *= 0.001;
    cout << "MCL " << numParticles << " particles";
    if (useBest) {
        cout << " using best particle";
    }
    cout << " time was " << mclTime << endl;

    mclFile.close();
    mclCoreFile.close();
}
