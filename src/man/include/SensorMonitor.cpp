
#include "SensorMonitor.h"

SensorMonitor::SensorMonitor(std::string sensorName, double low, double high, bool log)
	:  noise(),
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
	// todo: log output to /tmp/sensorName.sensor
	monitor.Print("%5.2f");
}
