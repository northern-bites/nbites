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
 * This class links a NoiseMeter and SignalMonitor together to provide
 * a first-warning system for failed sensors. Also, it provides a way
 * to quantify the amount of noise present in our data under game
 * circumstances.
 *
 * @see dsp.h
 * @author Nathan Merritt
 * @date June 2011
 */

#pragma once
#ifndef SENSOR_MONITOR_H
#define SENSOR_MONITOR_H

#include "dsp.h"

#include <string>

#define NOT_STEADY -1

static const int numberOfBins = 25;

class SensorMonitor : public Filter
{
public:
    SensorMonitor();
    SensorMonitor(std::string sensorName);
    ~SensorMonitor();

	double X(double);
	void Reset();
	void LogOutput(); // prints histograms to /home/nao/naoqi/log/{sensorName}.sensor

    // values outside will cause a print statement
    void setVarianceBounds(float low, float high);
    void disableErrors() { reportErrors = false; }

    static const int DONT_CHECK = -1;

    const int numberOfBins() const { return monitor.NumberOfBins(); }
    const double binMidPoint(int index) const;
    const int binCountAt(int index) const;

    const std::string SensorName() const { return sensorName; }
    void SensorName(std::string name) { sensorName = name; }

private:
    void reportSensorError();

    std::string sensorName;
    NoiseMeter<Butterworth> noise;
    SignalMonitor monitor;
    int steadyAtFrame;
    bool reportErrors; // warn if sensor variances exceed thresholds
    float lowVariance, highVariance;
    int seenErrors;
};


#endif
