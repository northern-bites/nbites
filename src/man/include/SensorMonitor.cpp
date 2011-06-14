
#include "SensorMonitor.h"

#include <iostream>
#include <fstream>
#include <sstream>

// parameters for the SignalMonitor
static const bool LOG_DEFAULT = true;
static const int NUMBER_BINS = 30;
static const float LOW_BIN = 0.0001f;
static const float HIGH_BIN = 30.0f;

SensorMonitor::SensorMonitor()
	:  noise(NoiseMeter<Butterworth>::ControlType(21, 60)),
	   monitor(NUMBER_BINS, LOW_BIN, HIGH_BIN, LOG_DEFAULT)
{
	Reset();
}

SensorMonitor::SensorMonitor(std::string sensorName)
	:  noise(NoiseMeter<Butterworth>::ControlType(21, 60)),
	   monitor(NUMBER_BINS, LOW_BIN, HIGH_BIN, LOG_DEFAULT)
{
	SensorMonitor::sensorName = sensorName;
	Reset();
}

SensorMonitor::~SensorMonitor() {
	// nothing to do here!
}

double SensorMonitor::X(double input) {
	noise.X(input);

	if (noise.Steady()) {
		monitor.X(noise.Y());

		// for making sure filter output is reasonable (test)
		if (steadyAtFrame == NOT_STEADY) {
			steadyAtFrame = SampleCount();
		}
	}
	return Y(input);
}

void SensorMonitor::Reset() {
	Filter::Reset();
	noise.Reset();
	monitor.Reset();
	steadyAtFrame = NOT_STEADY;
}

void SensorMonitor::LogOutput() {
	using namespace std;
	stringstream filename;

	filename << "/tmp/" << sensorName << ".sensor.xls";

	ofstream outFile;
	outFile.open(filename.str().c_str(), ifstream::out);

	outFile << "Report for sensor: " << sensorName << endl;
	outFile << monitor.toString() << endl;

	outFile.close();
}

const int SensorMonitor::binCountAt(int index) const {
	return monitor.binCount(index);
}

const double SensorMonitor::binMidPoint(int index) const {
	return monitor.binMidPoint(index);
}
