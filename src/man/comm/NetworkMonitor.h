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
 * This class provides the necessary mechanisms to monitor
 * network health according to trends in latencies and the
 * number of packets dropped. It makes use of the SignalMonitor
 * and Boxcar classes from the DSP library to store histograms of
 * the number of packets dropped and the distributions of packet
 * latencies, as well as monitor the trend in packet loss.
 * @author Ellis Ratner
 * @date June 2011
 *
 * Modified to have more encapsulation and for use in new system.
 * @author Wils Dawson 5/13/12
 */

#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include "dsp.h"

class NetworkMonitor : public Boxcar
{
public:
    /**
     * Class constructor
     * @param time: The time at which the monitor is constructed.
     */
    NetworkMonitor(long long time);

    /**
     * Class destructor.
     */
    ~NetworkMonitor();

    /**
     * Resets the boxcar filter, latency, and droppedPackets information.
     */
    void Reset();

    /**
     * Input for boxcar filter.
     */
    double X(double);

    /**
     * Records that a packet has been received, and adds its latency to
     * the histogram, as well as notifying the dropped packets monitor
     * that a packet has been received but not dropped.
     * @param timeSent Time the packet was sent (microseconds).
     * @param timeReceived Time the packet was received (microseconds).
     */
    void packetReceived(long long timeSent, long long timeReceived);

    /**
     * Records that a number of packets have been dropped and updates
     * the packets dropped histogram.
     * @param numDropped The number of packets dropped.
     */
    void packetsDropped(int numDropped);

    /**
     * @return: The total number of packets recieved.
     */
    const int totalPacketsReceived() const;

    /**
     * @return: The total number of packets delayed.
     */
    const int totalPacketsDropped() const;

    /**
     * @return: The total number of packets that we know were sent.
     */
    const int totalPackets() const { return (totalPacketsReceived() +
                                             totalPacketsDropped()); }

    /**
     * Finds the bin in the latency histogram with the highest number of
     * entries. The midpoint of this bin is considered the peak of the
     * latency.
     * @return Returns the index of peak latency bin.
     */
    int findPeakLatency();

    /**
     * Determines whether network health has deteriorated by using shifts in
     * the latency peak and significant increases in packet loss as criteria.
     * Sends warning notifications.
     * @param time: Current time
     * @return:     1 on GOOD, 2 on OK, 3 on BAD
     */
    int performHealthCheck(long long time);

    /**
     * Saves a report on network health to ~/nbites/log/network.xls on the robot,
     * including a histogram of the latency and of dropped packets.
     * @param time: The current time to make sure we don't output too often.
     */
    void logOutput(long long time);

    /**
     * Sets a flag indicating whether or not a network health deterioration
     * warning has been sent.
     * @param sent True if sent; false if not.
     */
    void setSentWarning(bool);

private:
    SignalMonitor latency;
    SignalMonitor droppedPackets;

    int       latencyPeak;
    bool      sentWarning;
    long long warningTime;
    long long lastOutput;
};

#endif // NETWORK_MONITOR_H
