
#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include "dsp.h"

class NetworkMonitor
{
public:
    NetworkMonitor();
    ~NetworkMonitor();

    void reset();

    void packetReceived(long long timeSent, long long timeReceived);
    void packetsDropped(int numDropped);

    const int totalPacketsReceived() const;
    const int totalPacketsDropped() const;
    const int totalPackets() const { return totalPacketsReceived() + totalPacketsDropped(); }

    void logOutput();

private:
    SignalMonitor latency;
    SignalMonitor droppedPackets;

    long long lastPacketReceivedAt;
};

#endif // NETWORK_MONITOR_H
