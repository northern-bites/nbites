
#include "SensorMonitor.h"

#include <iostream>
#include <fstream>
#include <sstream>

SensorMonitor::SensorMonitor(std::string sensorName, double low, double high, bool log)
	:  noise(NoiseMeter<Boxcar>::ControlType(21, 21)),
	   monitor(numberOfBins, low, high, log)
{
	SensorMonitor::sensorName = sensorName;
	Reset();
}

SensorMonitor::~SensorMonitor() {
	printf("SensorMonitor (%s) destructor\n", sensorName.c_str());
	LogOutput();
}

double SensorMonitor::X(double input) {
	noise.X(input);

	if (noise.Steady()) {
		monitor.X(noise.Y());

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
