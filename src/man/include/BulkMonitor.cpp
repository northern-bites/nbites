// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

/**
 * @author Nathan Merritt
 * @date June 2011
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "BulkMonitor.h"

BulkMonitor::BulkMonitor(int _numberMonitors, std:: string _bulkName,
                         const std::string sensorNames[])
    : numberMonitors(_numberMonitors),
      bulkName(_bulkName)
{
    monitors = new SensorMonitor[numberMonitors];
    for (int i = 0; i < numberMonitors; ++i)
        monitors[i].SensorName(sensorNames[i]);
}

BulkMonitor::~BulkMonitor() {
    delete [] monitors;
}

double BulkMonitor::update(int sensor, double input) {
    if (sensor < 0 || sensor > numberMonitors)
        return 0;
    return monitors[sensor].X(input);
}

SensorMonitor& BulkMonitor::Sensor(int sensor) {
    if (sensor < 0 || sensor > numberMonitors)
        printf("You requested a sensor I don't have! This is bad!\n");

    return monitors[sensor];
}

void BulkMonitor::Reset() {
    for (int i = 0; i < numberMonitors; ++i)
        monitors[i].Reset();
}

void BulkMonitor::LogOutput() {
    using namespace std;
    const int width = 12;

    stringstream filename;
    ofstream outFile;
    filename << "/home/nao/naoqi/log/" << bulkName << ".monitor.cvs";
    outFile.open(filename.str().c_str(), ifstream::out);

    // header line with sensor names
    outFile << setw(width) << "bin";
    for (int i = 0; i < numberMonitors; ++i) {
        outFile << setw(width) << monitors[i].SensorName();
    }
    outFile << endl;

    // histogram loop (data, line by line)
    const int numBins = monitors[0].numberOfBins();
    for (int bin = 0; bin < numBins; ++bin) {
        outFile << setw(width) << setprecision(4) << monitors[0].binMidPoint(bin);

        for (int i = 0; i < numberMonitors; ++i) {
            outFile << setw(width) << monitors[i].binCountAt(bin);
	}
        outFile << endl;
    }
    outFile.close();
}

void BulkMonitor::SpeechPointer(boost::shared_ptr<Speech> s) {
    speech = s;

    for (int i = 0; i < numberMonitors; ++i)
	monitors[i].SpeechPointer(speech);
}
