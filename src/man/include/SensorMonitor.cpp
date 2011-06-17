
#include "SensorMonitor.h"

#include <iostream>
#include <fstream>
#include <sstream>

// parameters for the SignalMonitor
static const bool LOG_DEFAULT = true;
static const int NUMBER_BINS = 30;
static const float LOW_BIN = 0.0001f;
static const float HIGH_BIN = 100.0f;

SensorMonitor::SensorMonitor()
    :  noise(NoiseMeter<Butterworth>::ControlType(21, 60)),
       monitor(NUMBER_BINS, LOW_BIN, HIGH_BIN, LOG_DEFAULT),
       reportErrors(false),
       lowVariance(DONT_CHECK), highVariance(DONT_CHECK)
{
    Reset();
}

SensorMonitor::SensorMonitor(std::string sensorName)
    :  noise(NoiseMeter<Butterworth>::ControlType(21, 60)),
       monitor(NUMBER_BINS, LOW_BIN, HIGH_BIN, LOG_DEFAULT),
       reportErrors(false),
       lowVariance(DONT_CHECK), highVariance(DONT_CHECK)
{
    SensorMonitor::sensorName = sensorName;
    Reset();
}

SensorMonitor::~SensorMonitor() {
    // nothing to do here!
}

double SensorMonitor::X(double input) {
    double variance = Y(noise.X(input));

    if (noise.Steady()) {
	monitor.X(variance);

	// for making sure filter output is reasonable (test)
	if (steadyAtFrame == NOT_STEADY) {
	    steadyAtFrame = SampleCount();
	}
    }

    //std::cout << sensorName << " input: " << input
    //<< " hpf " << noise.A().Y()
    //<< " var " << noise.Y() << std::endl;

    // error if variance is out of bounds or exactly zero (implies a dead sensor)
    if (reportErrors && noise.Steady()) {
        if ((lowVariance != DONT_CHECK && variance < lowVariance) ||
            (highVariance != DONT_CHECK && variance > highVariance) ||
            variance == 0.0) {
            reportSensorError();
        }
    }
    return Y();
}

void SensorMonitor::Reset() {
    Filter::Reset();
    noise.Reset();
    monitor.Reset();
    steadyAtFrame = NOT_STEADY;
    seenErrors = 0;
}

void SensorMonitor::LogOutput() {
    using namespace std;
    stringstream filename;

    filename << "/home/nao/naoqi/log/" << sensorName << ".sensor.xls";
    ofstream outFile;
    outFile.open(filename.str().c_str(), ifstream::out);

    outFile << "Report for sensor: " << sensorName << endl;
    outFile << "Showed unusual variance " << seenErrors << " times" << endl;
    outFile << monitor.toString() << endl;

    outFile.close();
}

void SensorMonitor::setVarianceBounds(float low, float high) {
    lowVariance = low;
    highVariance = high;

    reportErrors = true;
}

void SensorMonitor::reportSensorError() {
    ++seenErrors;
    if (seenErrors == 10)
	std::cout << "*** Potential sensor problem with " << sensorName
		  << ", saw a variance of " << Y()
		  << " (feel free to ignore this if the robot is stationary)"
		  << std::endl;
}

const int SensorMonitor::binCountAt(int index) const {
    return monitor.binCount(index);
}

const double SensorMonitor::binMidPoint(int index) const {
    return monitor.binMidPoint(index);
}
