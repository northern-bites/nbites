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
 * A simple container class for monitoring several sensors at once. This
 * class will print error messages if any of the sensors begins to deviate
 * significantly from its expected level of error.
 *
 * @see SensorMonitor.h
 * @see dsp.h
 * @author Nathan Merritt
 * @date June 2011
 */

#pragma once
#ifndef BULK_MONITOR_H
#define BULK_MONITOR_H

#include <string>
using std::string;

#include "SensorMonitor.h"

class BulkMonitor
{
public:
    BulkMonitor(boost::shared_ptr<Speech> s,
		int _numberMonitors, string _bulkName, const string _sensorNames[]);
    ~BulkMonitor();

    double update(int sensor, double input);

    SensorMonitor& Sensor(int sensor);

    void Reset();
    void LogOutput();

    int NumberMonitors() const { return numberMonitors; }

private:
    boost::shared_ptr<Speech> speech;
    SensorMonitor* monitors;
    int numberMonitors;
    string bulkName; // output file we log to in /home/nao/naoqi/log/
};

#endif
