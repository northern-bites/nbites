/* obsToNac.cpp */
#include "fakerIO.h"
#include "fakerIterators.h"
#include "NBMath.h"
#define UNIFORM_1_NEG_1 (2*(rand() / (float(RAND_MAX)+1)) - 1)
using namespace std;
using namespace boost;
using namespace NBMath;

int main(int argc, char** argv)
{
    // Information needed for the main method
    // IO Variables
    fstream obsFile;
    fstream ekfFile;
    fstream mclFile;
    fstream mclFile2;

    fstream ekfCoreFile;
    fstream mclCoreFile;
    fstream mclCoreFile2;
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

    vector<PoseEst> realPoses;
    vector<BallPose> ballPoses;
    vector<MotionModel> odos;
    vector<vector<Observation> > sightings;
    vector<float> ballDists;
    vector<float> ballBearings;
    MotionModel noMove(0.0, 0.0, 0.0);
    cout << "Reading in file" << endl;
    readObsInputFile(&obsFile, &realPoses, &ballPoses,
                     &odos, &sightings, &ballDists,
                     &ballBearings, BALL_ID);

    // Open output files
    string ekfFileName(argv[1]);
    string mclFileName(argv[1]);
    string mclFileName2(argv[1]);
    string ekfCoreFileName(argv[1]);
    string mclCoreFileName(argv[1]);
    string mclCoreFileName2(argv[1]);

    ekfFileName.replace(ekfFileName.end()-3, ekfFileName.end(), "ekf");
    mclFileName.replace(mclFileName.end()-3, mclFileName.end(), "mcl");
    mclFileName2.replace(mclFileName2.end()-3, mclFileName2.end(), "mcl.2");
    ekfCoreFileName.replace(ekfCoreFileName.end()-3,
                            ekfCoreFileName.end(), "ekf.core");
    mclCoreFileName.replace(mclCoreFileName.end()-3,
                            mclCoreFileName.end(), "mcl.core");
    mclCoreFileName2.replace(mclCoreFileName2.end()-3,
                             mclCoreFileName2.end(), "mcl.core.2");


    ekfFile.open(ekfFileName.c_str(), ios::out);
    mclFile.open(mclFileName.c_str(), ios::out);
    mclFile2.open(mclFileName2.c_str(), ios::out);

    ekfCoreFile.open(ekfCoreFileName.c_str(), ios::out);
    mclCoreFile.open(mclCoreFileName.c_str(), ios::out);
    mclCoreFile2.open(mclCoreFileName2.c_str(), ios::out);

    // Close the input file
    obsFile.close();
    shared_ptr<LocEKF> ekfLoc = shared_ptr<LocEKF>(new LocEKF());
    // Use weighted means
    shared_ptr<MCL> mcl = shared_ptr<MCL>(new MCL());
    // Use best particle
    shared_ptr<MCL> mcl2 = shared_ptr<MCL>(new MCL());
    mcl2->setUseBest(true);

    // Iterate through the path
    cout << "Running EKF loc" << endl;
    iterateObsPath(&obsFile, &ekfFile, &ekfCoreFile,
                   ekfLoc, &realPoses, &ballPoses, &odos,
                   &sightings, &ballDists, &ballBearings, BALL_ID);
    ekfFile.close();

    // Iterate through the path
    cout << "Running MCL loc" << endl;
    iterateObsPath(&obsFile, &mclFile, &mclCoreFile,
                   mcl, &realPoses, &ballPoses, &odos,
                   &sightings, &ballDists, &ballBearings, BALL_ID);
    mclFile.close();

    // Iterate through the path
    cout << "Running MCL 2 loc" << endl;
    iterateObsPath(&obsFile, &mclFile2, &mclCoreFile2,
                   mcl2, &realPoses, &ballPoses, &odos,
                   &sightings, &ballDists, &ballBearings, BALL_ID);
    mclFile2.close();

    return 0;
}
