/* navToObs.cpp */
#include "fakerIterators.h"
#include "fakerIO.h"
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
    fstream obsFile;

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
    readNavInputFile(&inputFile, &letsGo);

    // Clost the input file
    inputFile.close();

    // Open output files
    string obsFileName(argv[1]);
    obsFileName.replace(obsFileName.end()-3, obsFileName.end(), "obs");
    obsFile.open(obsFileName.c_str(), ios::out);

    // Iterate through the path
    iterateNavPath(&obsFile, &letsGo);

    // Close the output files
    obsFile.close();

    return 0;
}
