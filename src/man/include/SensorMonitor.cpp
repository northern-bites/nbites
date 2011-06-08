
#include "SensorMonitor.h"

SensorMonitor::SensorMonitor(std::string sensorName, double low, double high, bool log)
	:  noise(NoiseMeter<Boxcar>::ControlType(21, 21)),
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

	if (SampleCount() % 1000 == 0)
		LogOutput();

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
