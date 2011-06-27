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

#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include "dsp.h"

class NetworkMonitor : public Boxcar
{
public:
    NetworkMonitor();
    ~NetworkMonitor();

    // Inherited from the Boxcar filter class, useful for determining
    // what the trend is in packet loss.
    void Reset();
    double X(double);

    void packetReceived(long long timeSent, long long timeReceived);
    void packetsDropped(int numDropped);

    const int totalPacketsReceived() const;
    const int totalPacketsDropped() const;
    const int totalPackets() const { return totalPacketsReceived() + totalPacketsDropped(); }

    // Returns the index of the bin with the highest number of entries; the
    // midpoint of this bin can be considered the "peak" of the latency.
    int findPeakLatency();

    // Determines whether network health has deteriorated using shifts in the 
    // peak latency and significant increases in packet loss as critera. Sends
    // warnings by printing to the log.
    void performHealthCheck();

    // Saves a report on the network to ~/naoqi/log/network.xls, including a histogram
    // of the latency and dropped packets.
    void logOutput();
    
    void setSentWarning(bool);

private:
    SignalMonitor latency;
    SignalMonitor droppedPackets;

    long long lastPacketReceivedAt;
    int initialLatencyPeak;
    bool sentWarning;
};

#endif // NETWORK_MONITOR_H
