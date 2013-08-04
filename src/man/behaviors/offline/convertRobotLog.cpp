/* obsToNac.cpp */
#include "fakerIO.h"

using namespace std;

int main(int argc, char** argv)
{
    // Information needed for the main method
    // IO Variables
    fstream robotFile;
    fstream toolFile;

    /* Test for the correct number of CLI arguments */
    if(argc != 3) {
        cerr << "usage: " << argv[0] << " input-file output-file" << endl;
        return 1;
    }
    try {
        robotFile.open(argv[1], ios::in);

    } catch (const exception& e) {
        cout << "Failed to open input file" << argv[1] << endl;
        return 1;
    }

    try {
        toolFile.open(argv[2], ios::out);
    } catch (const exception& e) {
        cout << "Failed to open input file" << argv[1] << endl;
        return 1;
    }

    readRobotLogFile(&robotFile, &toolFile);
    robotFile.close();
    toolFile.close();

    return 0;
}
