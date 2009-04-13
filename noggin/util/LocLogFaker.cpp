/* LocLogFaker.cpp */

/**
 * Format of the navigation input file (*.nav):
 *
 * START POSITION LINE
 * x-value y-value heading-value
 *
 * NAVIGATION LINES
 * deltaForward deltaLateral deltaRotation
 *
 * Format of the log output file (*.mcl):
 *
 * PARTICLE INFO
 * x y h weight (for M particles)
 *
 * Colon signifying end of section
 *
 * DEBUG INFO
 * team_color player_number
 * x-estimate y-estimate heading-estimate deg
 * x-uncertinty y-uncert heading-uncert
 * ball-x ball-y
 * ball-uncert-x ball-uncert-y
 * ball-vel-x ball-vel-y
 * ball-vel-uncert-x ball-vel-uncert-y
 * odometery-lateral odometery-forward odometery-rotational
 *
 * ROBOT REAL INFO
 * x y h (as given by the system)
 *
 * LANDMARK INFO
 * ID dist bearing (for all landmarks observed in the frame)
 *
 */
#include "LocLogFaker.h"
#include "NBMath.h"

using namespace std;
using namespace boost;
using namespace NBMath;

int main(int argc, char** argv)
{
    // Information needed for the main method
    // Make navPath
    NavPath letsGo;
    // IO Variables
    fstream inputFile;
    fstream mclFile;
    fstream ekfFile;

    /* Test for the correct number of CLI arguments */
    if(argc != 2) {
        cerr << "usage: " << argv[0] << " input-file" << endl;
        return 1;
    }
    try {
        inputFile.open(argv[1], ios::in);

    } catch (const exception& e) {
        cout << "Failed to open input file" << argv[1] << endl;
        return 1;
    }

    // Get the info from the input file
    cout << "Reading in nav file" << endl;
    readNavInputFile(&inputFile, &letsGo);

    // Clost the input file
    inputFile.close();

    // Open output files
    string mclFileName(argv[1]);
    string ekfFileName(argv[1]);

    mclFileName.replace(mclFileName.end()-3, mclFileName.end(), "mcl.faker");
    ekfFileName.replace(ekfFileName.end()-3, ekfFileName.end(), "ekf.faker");

    mclFile.open(mclFileName.c_str(), ios::out);
    ekfFile.open(ekfFileName.c_str(), ios::out);

    // Iterate through the path
    cout << "Running loc systems" << endl;
    iterateFakerPath(&mclFile, &ekfFile, &letsGo);

    // Close the output files
    mclFile.close();
    ekfFile.close();

    return 0;
}
