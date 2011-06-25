
#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include "dsp.h"

class NetworkMonitor : public Boxcar
{
public:
    NetworkMonitor();
    ~NetworkMonitor();

    void Reset();
    double X(double);

    void packetReceived(long long timeSent, long long timeReceived);
    void packetsDropped(int numDropped);

    const int totalPacketsReceived() const;
    const int totalPacketsDropped() const;
    const int totalPackets() const { return totalPacketsReceived() + totalPacketsDropped(); }

    int findPeakLatency();
    void performHealthCheck();

    void logOutput();

private:
    SignalMonitor latency;
    SignalMonitor droppedPackets;

    long long lastPacketReceivedAt;
    int initialLatencyPeak;
};

#endif // NETWORK_MONITOR_H
