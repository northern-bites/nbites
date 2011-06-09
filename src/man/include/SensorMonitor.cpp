
#include "SensorMonitor.h"

#include <iostream>
#include <fstream>
#include <sstream>

SensorMonitor::SensorMonitor(std::string sensorName, double low, double high, bool log)
	:  noise(NoiseMeter<Butterworth>::ControlType(21, 60)),
	   monitor(numberOfBins, low, high, log)
{
	SensorMonitor::sensorName = sensorName;
	Reset();
}

SensorMonitor::~SensorMonitor() {
	LogOutput();
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
