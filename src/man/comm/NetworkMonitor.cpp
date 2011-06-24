
#include <fstream>
#include <iostream>
#include <iomanip>

#include "NetworkMonitor.h"

// Constants for the latency monitor.
static const int NUM_BINS_LATENCY = 30;
static const double LOW_BIN_LATENCY = 1.0f;        // Low is 1 microsecond.
static const double HIGH_BIN_LATENCY = 1000000.0f; // High is 1 second.
static const bool LOG_SCALE_LATENCY = true;

// Constants for the dropped packets monitor.
static const int NUM_BINS_DROPPED = 2;
static const double LOW_BIN_DROPPED = 0.0f;
static const double HIGH_BIN_DROPPED = 1.0f;
static const bool LOG_SCALE_DROPPED = false;

NetworkMonitor::NetworkMonitor()
    : latency(NUM_BINS_LATENCY, LOW_BIN_LATENCY, HIGH_BIN_LATENCY, LOG_SCALE_LATENCY),
      droppedPackets(NUM_BINS_DROPPED, LOW_BIN_DROPPED, HIGH_BIN_DROPPED, LOG_SCALE_DROPPED),
      lastPacketReceivedAt(0)
{

}

NetworkMonitor::~NetworkMonitor()
{

}

void NetworkMonitor::reset()
{
    latency.Reset();
    droppedPackets.Reset();
}

void NetworkMonitor::packetReceived(long long timeSent, long long timeReceived)
{
    // Add the packet as not dropped.
    droppedPackets.X(0.0f);
    
    if(totalPackets() <= 1)
	lastPacketReceivedAt = timeReceived;
    else
    {
	// Calculate the latency. Add to latency monitor.
	latency.X(timeReceived - timeSent);
    }
}

void NetworkMonitor::packetsDropped(int numDropped)
{
    // For each dropped packet, increment the "dropped" bin.
    for(int i = 0; i < numDropped; ++i)
	droppedPackets.X(1.0f);
}

const int NetworkMonitor::totalPacketsReceived() const
{
    // 0 indicates that a packet has been received.
    return droppedPackets.binCount(0);
}

const int NetworkMonitor::totalPacketsDropped() const
{
    // 1 indicates a dropped packet.
    return droppedPackets.binCount(1);
}

// Saves the latency and dropped packets histograms to an output file.
void NetworkMonitor::logOutput()
{
    using namespace std;

    ofstream logFile;
    logFile.open("/home/nao/naoqi/log/network.xls", ios::out);

    if(logFile.is_open())
    {
	using namespace std;
	const int width = 12;

	logFile << "Network Monitor Report" << endl;
	logFile << "Latency: " << endl;
	for(int i = 0; i < NUM_BINS_LATENCY; ++i)
	{
	    logFile << setw(width) << setprecision(4) << latency.binMidPoint(i);
	    logFile << setw(width) << latency.binCount(i);
	    logFile << endl;
	}
	logFile << "Dropped packets: " << endl;
	for(int i = 0; i < NUM_BINS_DROPPED; ++i)
	{
	    logFile << setw(width) << setprecision(4) << droppedPackets.binMidPoint(i);
	    logFile << setw(width) << droppedPackets.binCount(i);
	    logFile << endl;
	}
    }
    else
	cerr << "NetworkMonitor::logOutput() : error opening log file!" << endl;
}
